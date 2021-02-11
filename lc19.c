#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <argp.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "args.h"
#include "ssl.h"
#include "endpoint.h"

// TODO: multithreading
void handle(SSL *ssl, endpoint *e) {

	// Initialize buffer and response data
	char buf[1027] = {0};
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
	if (resp.code == 20) {
		char *header = malloc(2 + strlen(resp.mime));
		sprintf(header, "%d ", resp.code);
		strcat(header, resp.mime);
		strcat(header, "\r\n");
		SSL_write(ssl, header, strlen(header));
		free(header);

		// Send response body
		SSL_write(ssl, resp.file_path, strlen(resp.file_path));
		printf("S: %s %s\n", resp.mime, resp.file_path);
	} else {
		char header[11];
		sprintf(header, "%d error\r\n", resp.code);
		SSL_write(ssl, header, strlen(header));
		printf("Error: %s\n", header);
	}

	fd = SSL_get_fd(ssl);
	SSL_free(ssl);
	close(fd);
}

int main(int argc, char **argv) {

	// Parse command-line arguments
	env arguments = { 1965, ".", "../ssl/server.crt", "../ssl/server.key" };
	argp_parse(&parser, argc, argv, 0, 0, &arguments);

	// Initialize SSL
	SSL_library_init();
	SSL_CTX *ctx = init_ctx();

	// Load certificates
	load_cert(ctx, arguments.cert, arguments.key);

	// Initialize server
	int server = init_fd(arguments.port);

	// Create endpoints
	int ne = line_count("./endpoints");
	endpoint *e = malloc((ne + 1) * sizeof(endpoint)); // + 1 to compensate for end marker
	get_endpoints(e, ".");

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
		printf("Connection: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
		SSL_set_fd(ssl, client);

		handle(ssl, e);
	}

	close(server);
	SSL_CTX_free(ctx);

	return EXIT_SUCCESS;
}
