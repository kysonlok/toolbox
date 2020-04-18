#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "zlib_api.h"

int main()
{
	int ret = 0;
	int i = 0;

	const unsigned char *str = "hello, hello111111111111111111111111111111111!";

	unsigned char *compr = (unsigned char *)malloc(1024);
	size_t compr_len = 1024;
	unsigned char *uncompr = (unsigned char *)malloc(1024);
	size_t uncompr_len = 1024;

	/* original data len */
	printf("original data len is: %ld\n", strlen(str));
	printf("original data is: \n%s\n\n", str);

	/* test compress */
	ret = compress_str(str, strlen(str) + 1, compr, &compr_len);
	if (ret < 0) {
		printf("error: failed to compress.\n");
		exit(1);
	}

	printf("compressed len is: %ld\n", compr_len);
	printf("compressed data:\n%.*s\n", (int)compr_len, compr);
	printf("convert to hex :\n");
	for (i = 0; i < compr_len; i++) {
		printf("%x ", compr[i]);
	}
	printf("\n\n");

	ret = uncompress_str(compr, compr_len, uncompr, &uncompr_len);
	if (ret < 0) {
		printf("error: failed to uncompress.\n");
		exit(1);
	}

	printf("uncompressed data: \n%s\n", uncompr);

	if (compr != NULL) {
		free(compr);
	}

	if (uncompr != NULL) {
		free(uncompr);
	}

	return ret;
}
