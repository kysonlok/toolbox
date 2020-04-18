#ifndef _ZLIB_API_H
#define _ZLIB_API_H

#include <stdio.h>

int compress_str(const unsigned char *src, size_t src_len, unsigned char *dest, size_t *dest_len);

int uncompress_str(const unsigned char *src, size_t src_len, unsigned char *dest, size_t *dest_len);

/**
 * @in_filename: input file, the original data filename
 * @out_filename: output file, the compressed data filename
 * @return: 0, success; -1, failure
 */
int compress_file(const char *in_filename, const char *out_filename);

/**
 * @in_filename: input file, the compressed data filename
 * @out_filename: output file, the uncompressed data filename
 * @return: 0, success; -1, failure
 */
int uncompress_file(const char *in_filename, const char *out_filename);

#endif
