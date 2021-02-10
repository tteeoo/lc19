extern struct argp parser;

typedef struct {
	int port;
	char *dir, *cert, *key;
} env;

error_t parse_opt (int key, char *arg, struct argp_state *state);
