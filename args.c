#include <argp.h>
#include <stdlib.h>

#include "args.h"

const char *argp_program_version = "lc19 version 0.1.0";
const char *argp_program_bug_address = "<theodorehenson at protonmail dot com>";
const char doc[] = "A simple gemini server";

struct argp_option options[] = {
	{ "port", 'p', "PORT", 0, "The port to listen on" },
	{ "dir",  'd', "DIR",  0, "The directory to serve" },
	{ "cert", 'c', "CERT", 0, "The path to the PEM format SSL certificate to use" },
	{ "key",  'k', "KEY",  0, "The path to the corresponding PEM format SSL private key to use" },
	{ 0 }
};

error_t parse_opt (int key, char *arg, struct argp_state *state) {
	env *arguments = state->input;
	switch (key) {
		case 'p':
			arguments->port = (int) strtol(arg, NULL, 10);
			break;
		case 'd':
			arguments->dir = arg;
			break;
		case 'c':
			arguments->cert = arg;
			break;
		case 'k':
			arguments->key = arg;
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

struct argp parser = { options, parse_opt, 0, doc };
