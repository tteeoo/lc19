typedef struct {
	char mime[256];
	char file_path[256];
	char url_path[256];
	int end;
} endpoint;

typedef struct {
	char file_path[256];
	char mime[256];
	int code;
} response;

typedef struct {
	char file_path[256];
	char *data;
} cache;

int line_count(char *file);
response url_to_response(char *url, endpoint *endpoints);
void get_endpoints(endpoint *endpoints, char *dir);
