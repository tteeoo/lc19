SSL_CTX *init_ctx();
void load_cert(SSL_CTX *ctx, char *cert, char *key);
int init_fd(int port);
