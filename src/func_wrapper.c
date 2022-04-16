#include "ankii.h"


char *
strcat_wrapper(char *dest, const char *src)
{
    if (strlen(dest) + strlen(src) >= MAXLINE)
        err_sys("strcat_wrapper: strcat error");

    return strcat(dest, src);
}

int
fputs_wrapper(const char *s, FILE *restrict stream)
{
    int ret_val;

    if ((ret_val = fputs(s, stream)) == EOF)
        err_sys("fputs_wrapper: fputs error");

    fflush(stream);

    return ret_val;
}
