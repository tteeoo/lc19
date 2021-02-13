#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#include "endpoint.h"

#define SCHEME "gemini://"
#define SCHEME_LEN 9

#define E_FILE "endpoints"
#define E_FILE_LEN 9

// Gets the number of lines in a file
int line_count(char *file) {
	int c, count = 0;
	FILE *fp; 
	if (!(fp = fopen(file, "r"))) {
		fprintf(stderr, "Error: Failed to open %s. (Does it exist?)\n", file);
		abort();
	}
	for (c = getc(fp); c != EOF; c = getc(fp)) 
		if (c == '\n')
			count++;
	fclose(fp);
	return count;
}

// Gets the number of a specific character in a string
int char_count(char *str, char c) {
	int count = 0;
	for (int i = 0; str[i] != '\0'; i++)
		if (str[i] == c)
			count++;
	return count;
}

// Returns a response given a URL
response url_to_response(char *url, endpoint *endpoints) {

	// Verify scheme is gemini://
	url[strlen(url) - 2] = '\0';
	int le = strlen(url);
	char r_scheme[SCHEME_LEN + 1] = { '\0' };
	if (le <= SCHEME_LEN)
		return (response){ .code = 59, .mime = "Request too short" };
	strncpy(r_scheme, url, SCHEME_LEN);
	if (strcmp(r_scheme, SCHEME) != 0) {
		printf("%s %s\n", r_scheme, SCHEME);
		return (response){ .code = 59, .mime = "Improper protocol scheme" };
	}

	// Get requested path, ignoring host
	char url_path[256];
	int slashes = char_count(url, '/');
	strcpy(url_path, "/");
	if (!(slashes == 3 && url[le - 1] == '/') && slashes > 2) {
		int num_slash = 0, path_i = 0;
		for (int i = 0; i < strlen(url); i++) {
			if (num_slash == 3) {
				path_i = i;
				break;
			}
			if (url[i] == '/')
				num_slash++;
		}
		strcat(url_path, &url[path_i]);
	}

	// Find the endpoint at the url
	int e_index = -1;
	for (int i = 0; endpoints[i].end; i++)  {
		if (strcmp(url_path, endpoints[i].url_path) == 0) {
			e_index = i;
			break;
		}
	}
	if (e_index == -1)
		return (response){ .code = 51, .mime = "Not found, à la 404" };
	endpoint e = endpoints[e_index];

	response resp = (response){ .code = 20 };
	strcpy(resp.file_path, e.file_path);
	strcpy(resp.mime, e.mime);
	return resp;
}

// Populates an array with endpoints parsed from the endpoints file
void get_endpoints(endpoint *endpoints, char *path) {

	// Open the endpoints file
	FILE *fp = fopen(path, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error: Failed to open %s\n", path);
		abort();
	}
	free(path);

	// Iterate each line
	int line_num = 0;
	char line[1024];
	while (fgets(line, sizeof(line), fp)) {

		char mime[256], file_path[256], url_path[256];
		url_path[0] = '\0';
		int field = 0;

		// Iterate each token
		for (char *tok = strtok(line, "\t"); tok != NULL; tok = strtok(NULL, "\t")) {
			switch (field) {
			case 0:
				strcpy(mime, tok);
				break;
			case 1:
				strcpy(file_path, tok);
				break;
			case 2:
				strcpy(url_path, tok);
				break;
			}
			field++;
		}

		// Copy the parsed data
		endpoints[line_num] = (endpoint){ .end = 1 };
		strcpy(endpoints[line_num].mime, mime);
		if (url_path[0] == '\0') {
			strtok(file_path, "\n");
			strcpy(endpoints[line_num].file_path, file_path);
			strcpy(endpoints[line_num].url_path, "/");
			strcat(endpoints[line_num].url_path, file_path);
		} else {
			strtok(url_path, "\n");
			strcpy(endpoints[line_num].file_path, file_path);
			strcpy(endpoints[line_num].url_path, url_path);
		}

		line_num++;
	}

	endpoints[line_num] = (endpoint){ .end = 0 };
	fclose(fp);
}
