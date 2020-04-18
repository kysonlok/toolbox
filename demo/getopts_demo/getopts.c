#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

struct student {
	int age;
	char *name;
};

static void print_usage(void)
{
	 printf("Usage: getopts <-n name> <-a age> [-h]\n\n");
	 printf(" -n : specify the name.\n");
	 printf(" -a : specify the age\n");
	 printf(" -h : print the help.\n\n");
	 exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	int ret = 0;
	char opt;

	struct student s1;

	if (argc < 5) {
		print_usage();
	}

	while ((opt = getopt(argc, argv, "n:a:h")) != -1) {
		switch (opt) {
		case 'n':
			s1.name = strdup(optarg);
			break;
		case 'a':
			s1.age = atoi(optarg);
			break;
		case 'h':
			break;
		case '?':
			print_usage();
		}
	}

	printf("student's age is: %d, name: %s\n", s1.age, s1.name);

	return ret;
}
