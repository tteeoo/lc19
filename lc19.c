#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <signal.h>
#include <argp.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "args.h"
#include "ssl.h"
#include "endpoint.h"

#define SEND_BUF 1027

int server;
SSL_CTX *ctx;

void handle_sigint(int signum) {
	printf("\nGot signal %d, shutting down...\n", signum);
	close(server);
	SSL_CTX_free(ctx);
	exit(0);
}

void close_ssl(SSL *ssl) {
	int fd = SSL_get_fd(ssl);
	SSL_free(ssl);
	close(fd);
}

void send_error(SSL *ssl, response resp) {
	char header[11];
	sprintf(header, "%d error\r\n", resp.code);
	SSL_write(ssl, header, strlen(header));
	printf("Error: %s", header);
}

// TODO: multithreading
void handle(SSL *ssl, endpoint *e) {

	// Initialize buffer and response data
	char buf[SEND_BUF] = {0};
	int fd, mlen;

	if (SSL_accept(ssl) == -1) {
		ERR_print_errors_fp(stderr);
		fd = SSL_get_fd(ssl);
		SSL_free(ssl);
		close(fd);
		return;
	}

	// Read into buffer
	mlen = SSL_read(ssl, buf, sizeof(buf));
	buf[mlen] = '\0';

	if (mlen <= 0) {
		ERR_print_errors_fp(stderr);
		fd = SSL_get_fd(ssl);
		SSL_free(ssl);
		close(fd);
		return;
	}
	printf("C: %s", buf);

	// Send response header
	response resp = url_to_response(buf, e);
	if (resp.code != 20) {
		send_error(ssl, resp);
		close_ssl(ssl);
		return;
	}

	char *header = malloc(2 + strlen(resp.mime));
	sprintf(header, "%d ", resp.code);
	strcat(header, resp.mime);
	strcat(header, "\r\n");
	SSL_write(ssl, header, strlen(header));
	free(header);

	// Send response body
	int sfd;
	struct stat file_stat;
	if ((sfd = open(resp.file_path, O_RDONLY)) < 0) {
		resp.code = 40;
		send_error(ssl, resp);
		close_ssl(ssl);
		return;
	}
	if ((fstat(sfd, &file_stat)) < 0) {
		resp.code = 40;
		send_error(ssl, resp);
		close_ssl(ssl);
		return;
	}
	printf("S: %s %s\n", resp.mime, resp.file_path);
	while (read(sfd, buf, SEND_BUF)) {
		SSL_write(ssl, buf, SEND_BUF);
	}

	close(sfd);
	close_ssl(ssl);
}

int main(int argc, char **argv) {

	// Assign signal handlers
	signal(SIGINT, handle_sigint);

	// Parse command-line arguments
	env arguments = { 1965, ".", "../ssl/cert.pem", "../ssl/key.pem" };
	argp_parse(&parser, argc, argv, 0, 0, &arguments);

	// Initialize SSL
	SSL_library_init();
	ctx = init_ctx();

	// Load certificates
	load_cert(ctx, arguments.cert, arguments.key);

	// Initialize server
	server = init_fd(arguments.port);

	// Create endpoints
	char *e_file = malloc(strlen(arguments.dir) + 11);
	sprintf(e_file, "%s/%s", arguments.dir, "endpoints");
	int ne = line_count(e_file);
	endpoint *e = malloc((ne + 1) * sizeof(endpoint)); // + 1 to compensate for end marker
	get_endpoints(e, e_file);

	// Accept connections
	while (1) {

		// Initialize incoming client
		struct sockaddr_in addr;
		socklen_t addrlen = sizeof(addr);
		SSL *ssl = SSL_new(ctx);

		// Accept connection
		int client;
		if ((client = accept(server, (struct sockaddr *)&addr, (socklen_t*)&addrlen)) < 0) {
			fprintf(stderr, "Error: Failed to accept incoming connection");
			continue;
		}
		printf("\nNew: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
		SSL_set_fd(ssl, client);

		handle(ssl, e);
	}

	close(server);
	SSL_CTX_free(ctx);

	return EXIT_SUCCESS;
}
