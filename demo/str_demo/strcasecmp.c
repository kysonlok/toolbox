#if !defined(HAVE_STRCASECMP) || !defined(HAVE_STRNCASECMP)
#include <stdlib.h>
#include <string.h>
#endif

#ifndef HAVE_STRCASECMP
int strcasecmp(const char *s1, const char *s2)
{
    int i, ret;
    char *cp1, *cp2;

    cp1 = malloc(strlen(s1) + 1);
    cp2 = malloc(strlen(s2) + 1);

    for (i = 0; i < strlen(s1) + 1; i++)
        cp1[i] = tolower((int) (unsigned char) s1[i]);
    for (i = 0; i < strlen(s2) + 1; i++)
        cp2[i] = tolower((int) (unsigned char) s2[i]);

    ret = strcmp(cp1, cp2);

    free(cp1);
    free(cp2);

    return ret;
}
#endif

#ifndef HAVE_STRNCASECMP
int strncasecmp(const char *s1, const char *s2, size_t n)
{
    int i, ret;
    char *cp1, *cp2;

    cp1 = malloc(strlen(s1) + 1);
    cp2 = malloc(strlen(s2) + 1);

    for (i = 0; i < strlen(s1) + 1; i++)
        cp1[i] = tolower((int) (unsigned char) s1[i]);
    for (i = 0; i < strlen(s2) + 1; i++)
        cp2[i] = tolower((int) (unsigned char) s2[i]);

    ret = strncmp(cp1, cp2, n);

    free(cp1);
    free(cp2);

    return ret;
}
#endif

