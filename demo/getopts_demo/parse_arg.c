#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct config {
	char *host;
	int port;
	bool verbose;
};

enum err_t {
	ERR_SUCCESS,
	ERR_INVAL
};

void config_init(struct config *config)
{
	config->host = "127.0.0.1";
	config->port = 0;
	config->verbose = false;
}

static void print_usage(void)
{
	 printf("Usage: parse_args [-h host] [-p port] [-v verbose] [--help]\n\n");
	 printf(" -h : specify the host, default is 127.0.0.1\n");
	 printf(" -p : specify the port, default is 0\n");
	 printf(" -v : print the debug information.\n");
	 printf(" --help : print the help.\n\n");
}

int parse_config_args(struct config *config, int argc, char **argv)
{
	int i = 0;
	for (i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--host")) {
			if (i < (argc-1)) {
				config->host = strdup(argv[i+1]);
			} else {
				fprintf(stderr, "parse_config_args: -h argument given, but no host specified.\n");
				goto error;
			}
			i++;
		} else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port")) {
			if (i < (argc-1)) {
				config->port = atoi(argv[i+1]);
			} else {
				fprintf(stderr, "parse_config_args: -p argument given, but no port specified.\n");
				goto error;
			}
			i++;
		} else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
			config->verbose = true;
		} else if (!strcmp(argv[i], "--help")) {
			goto error;
		} else {
			fprintf(stderr, "Error: Unknown option '%s'.\n",argv[i]);
			goto error;
		}
	}
error:
	print_usage();
	return ERR_INVAL;
}

int main(int argc, char **argv)
{
	int ret = 0;

	struct config config;
	config_init(&config);

	parse_config_args(&config, argc, argv);
	printf("host is [%s]\n", config.host);
	printf("port is [%d]\n", config.port);

	return ret;
}
