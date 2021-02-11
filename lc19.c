#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <argp.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "args.h"
#include "ssl.h"

void handle(SSL *ssl) {

	// Initialize buffer and response data
	char buf[1024] = {0};
	int fd, mlen;
	const char *resp = "# lc19";

	if (SSL_accept(ssl) == -1)
		ERR_print_errors_fp(stderr);
	else {
		// Read into buffer
		mlen = SSL_read(ssl, buf, sizeof(buf));
		buf[mlen] = '\0';
		printf("C: %s\n", buf);
		if (mlen > 0)
			SSL_write(ssl, resp, strlen(resp));
		else
			ERR_print_errors_fp(stderr);
	}

	// Close socket
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

		handle(ssl);
	}

	close(server);
	SSL_CTX_free(ctx);

	return EXIT_SUCCESS;
}
