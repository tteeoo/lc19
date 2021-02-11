#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#include "endpoint.h"

const char *e_file = "endpoints";

// Get the number of lines in a file
int line_count(char *file) {
	FILE *fp; 
	int c, count = 0;
	fp = fopen(file, "r"); 
	for(c = getc(fp); c != EOF; c = getc(fp)) 
		if(c == '\n')
			count++;
	fclose(fp);
	return count;
}

// Load data from file insto array of endpoints
int get_endpoints(endpoint *endpoints, char *dir) {

	// Initialize the path
	char *path = malloc(sizeof(dir) + sizeof(e_file) + 2);
	strcpy(path, dir);
	strcat(path, "/");
	strcat(path, e_file);


	// Open the endpoints file
	FILE *fp = fopen(path, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error: Failed to open %s\n", path);
		return -1;
	}
	free(path);

	// Iterate each line
	int line_num = 0;
	char line[1024];
	while (fgets(line, sizeof(line), fp)) {

		int field = 0;
		char *mime, *file_path, *url_path;

		// Iterate each token
		for (char *tok = strtok(line, "\t"); tok != NULL; tok = strtok(NULL, "\t")) {
			switch (field) {
			case 0:
				mime = tok;
				break;
			case 1:
				file_path = tok;
				break;
			case 2:
				url_path = tok;
				break;
			}
			field++;
		}
		if (cache == 0) {
			if (url_path != NULL)
				strtok(url_path, "\n");
			else
				url_path = file_path;
		}

		// Copy the parsed data
		endpoints[line_num] = (endpoint){ .cache = 0 };
		strcpy(endpoints[line_num].mime, mime);
		strcpy(endpoints[line_num].file_path, file_path);
		strcpy(endpoints[line_num].url_path, url_path);

		line_num++;
	}
	fclose(fp);

	return 0;
}

// TODO: remove (here for testing)
int main() {
	endpoint e[line_count("./endpoints")];
	get_endpoints(e, ".");
	for (int i = 0; i < (sizeof(e) / sizeof(*e)); i++)
		printf("mime:%s\nfile_path:%s\nurl_path:%s\ncache:%d\n\n", e[i].mime, e[i].file_path, e[i].url_path, e[i].cache);
}
