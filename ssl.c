#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ssl.h"

// N for listen(FD, N)
const int lq = 12;

// Initializes OpenSSL and returns an SSL context
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

// Loads a certificate and private key
void load_cert(SSL_CTX *ctx, char *cert, char *key) {

	// Load certificate
	if (SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		abort();
	}

	// Load private key
	if (SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM) <= 0 ) {
		ERR_print_errors_fp(stderr);
		abort();
	}

	// Validate private key
	if (!SSL_CTX_check_private_key(ctx)) {
		fprintf(stderr, "Error: Private key does not match the public certificate\n");
		abort();
	}
}

int init_fd(int port) {

	// Create socket
	int fd;
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == 0) {
		fprintf(stderr, "Error: Failed to create socket");
		return EXIT_FAILURE;
	}

	// Bind port
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Error: Failed to bind port");
		abort();
	}

	// Configure
	if (listen(fd, lq) < 0) {
		fprintf(stderr, "Error: Failed to configure listening port");
		abort();
	}

	return fd;
}
