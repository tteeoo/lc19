#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <resolv.h>
#include <netdb.h>
#include <argp.h>

#include "args.h"

#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX* init_ctx() {

	// Load crypto suite and error messages
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	// Determine the SSL method
	const SSL_METHOD *method = TLS_server_method();

	// Initialize the context
	SSL_CTX *ctx = SSL_CTX_new(method);
	if (ctx == NULL) {
		ERR_print_errors_fp(stderr);
		abort();
	}

	return ctx;
}

void load_cert(SSL_CTX *ctx, char *cert, char *key) {
	if (SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		abort();
	}
	if ( SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM) <= 0 ) {
		ERR_print_errors_fp(stderr);
		abort();
	}
	if (!SSL_CTX_check_private_key(ctx)) {
		fprintf(stderr, "Private key does not match the public certificate\n");
		abort();
	}
}

int init_fd(int port) {
	int fd;
	struct sockaddr_in addr;

	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == 0) {
		fprintf(stderr, "error: failed to create socket");
		return EXIT_FAILURE;
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("error: failed to bind port");
		abort();
	}

	if (listen(fd, 12) < 0) {
		printf("error: failed to listen");
		abort();
	}

	return fd;
}

void handle(SSL *ssl) {
	char buf[1024] = {0};
	int fd, mlen;
	const char *resp = "# lc19";
	if (SSL_accept(ssl) == -1) {
		ERR_print_errors_fp(stderr);
	} else {
		mlen = SSL_read(ssl, buf, sizeof(buf));
		buf[mlen] = '\0';
		printf("C: %s\n", buf);
		if (mlen > 0) {
			SSL_write(ssl, resp, strlen(resp));
		} else {
			ERR_print_errors_fp(stderr);
		}
	}

	fd = SSL_get_fd(ssl);
	SSL_free(ssl);
	close(fd);
}

int main(int argc, char **argv) {

	// Parse command-line arguments
	env arguments = { 1965, ".", "./ssl/selfsigned.crt", "./ssl/selfsigned.key" };
	argp_parse(&parser, argc, argv, 0, 0, &arguments);

	// Initialize SSL
	SSL_library_init();
	SSL_CTX *ctx = init_ctx();

	// Load certificates
	load_cert(ctx, arguments.cert, arguments.key);

	// Initialize server
	int server = init_fd(arguments.port);

	// Accept connections
	while (1) {

		// Initialize incoming client
		struct sockaddr_in addr;
		socklen_t addrlen = sizeof(addr);
		SSL *ssl = SSL_new(ctx);

		// Accept connection
		int client;
		if ((client = accept(server, (struct sockaddr *)&addr, (socklen_t*)&addrlen)) < 0) {
			fprintf(stderr, "error: failed to accept");
			continue;
		}
		printf("Connected: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
		SSL_set_fd(ssl, client);

		handle(ssl);
	}

	close(server);
	SSL_CTX_free(ctx);

	return EXIT_SUCCESS;
}
