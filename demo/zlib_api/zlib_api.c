#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "zlib_api.h"

int compress_str(const unsigned char *src, size_t src_len, unsigned char *dest, size_t *dest_len)
{
	int ret = 0;
	size_t len = strlen(src) + 1;

	/* check input and output */
	if (src == NULL || dest == NULL) {
		ret = -1;
		printf("check compress or uncompress data is NULL.\n");
		return ret;
	}

	/* fault - tolerance */
	if (src_len <= 0 || src_len > len) {
		src_len = len;
	}

	/* compress data */
	ret = compress(dest, dest_len, src, src_len);
	if (ret != Z_OK) {
		printf("failed to compress data.\n");
		return ret;
	}

	return 0;
}

int uncompress_str(const unsigned char *src, size_t src_len, unsigned char *dest, size_t *dest_len)
{
	int ret = 0;

	/* check input and output */
	if (src == NULL || dest == NULL) {
		ret = -1;
		printf("check compress or uncompress data is NULL.\n");
		return ret;
	}

	/* compress data */
	ret = uncompress(dest, dest_len, src, src_len);
	if (ret != Z_OK) {
		printf("failed to uncompress data.\n");
		return ret;
	}

	return 0;
}

int compress_file(const char *in_filename, const char *out_filename)
{
	/* TODO */
	return 0;
}

int uncompress_file(const char *in_filename, const char *out_filename)
{
	/* TODO */
	return 0;
}
