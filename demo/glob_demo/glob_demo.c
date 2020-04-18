#include <stdio.h>
#include <glob.h>

int main()
{
	char buf[128] = {0};
	glob_t gl;

	if (glob("/tmp/*", GLOB_NOESCAPE | GLOB_MARK, NULL, &gl) < 0) {
		printf("Failed to search dir\n");
		return -1;
	}

	int i = 0;
	for (i = 0; i < gl.gl_pathc; i++) {
		printf("%d: %s\n", i, gl.gl_pathv[i]);
	}

	globfree(&gl);

}
