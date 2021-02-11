typedef struct {
	char mime[256];
	char file_path[256];
	char url_path[256];
	int cache;
	char *data;
} endpoint;

int get_endpoints(endpoint *endpoints, char *dir);
