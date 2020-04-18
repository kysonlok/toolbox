#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <string.h>

#define VERSION "1.0.0"
enum {
	GETOPT_VAL_ADD = 257,
	GETOPT_VAL_APPEND,
	GETOPT_VAL_DELETE,
	GETOPT_VAL_VERBOSE
};

static void print_usage(void)
{
	printf("\n");
	printf(" getopts_long %s\n\n", VERSION);
	printf("  usage:\n\n");
	printf("	[--add <value>]		Add a value.\n");
	printf("	[--append <value>]	Append a value.\n");
	printf("	[--delete <value>]	Delete a value.\n");
	printf("	[-v|--verbose]	Verbose mode.\n");
	printf("	[-c|--command <cmd>]	Execute command.\n");
	printf("	[-f|--file <file>]	The file path to store pid.\n");
	printf("	[-h|--help]		Print help.\n\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	int c;
	int ret = 0;
	int option_index = 0;

	static struct option long_options[] = {
		{"add",      required_argument, NULL,  GETOPT_VAL_ADD },
		{"append",   required_argument, NULL,  GETOPT_VAL_APPEND },
		{"delete",   required_argument, NULL,  GETOPT_VAL_DELETE },
		{"verbose",  no_argument,       NULL,  'v' },
		{"command",  required_argument, NULL,  'c' },
		{"file",     required_argument, NULL,  'f' },
		{"help",     no_argument,		NULL,  'h' },
		{0,          0,                 NULL,  0 }
	};

	while ((c = getopt_long(argc, argv, "c:f:vh", long_options, &option_index)) != -1) {
		switch (c) {
		case GETOPT_VAL_ADD:
			printf("print add.\n");
			break;
		case GETOPT_VAL_APPEND:
			printf("print append.\n");
			break;
		case GETOPT_VAL_DELETE:
			printf("print delete.\n");
			break;
		case 'v':
			printf("print verbose.\n");
			break;
		case 'c':
			printf("print command with param: [%s].\n", strdup(optarg));
			break;
		case 'f':
			printf("print file with param: [%s].\n", strdup(optarg));
			break;
		case 'h':
			print_usage();
			break;
		case '?':
			printf("Unrecognized option: %s", optarg);
			break;
		}
	}

	if (optind < argc) {
		printf("non-option ARGV-elements: ");
		while (optind < argc)
			printf("%s ", argv[optind++]);

		printf("\n");
	}

	return ret;
}

