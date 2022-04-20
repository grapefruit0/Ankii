#include "ankii.h"


char *
strcat_wrapper(char *dest, const int maxsize, const int va_size, ...)
{
    char *src;
    va_list ap;

    va_start(ap, va_size);
    for (int i = 0; i < va_size; ++i) {
        src = va_arg(ap, char *);
        if (strlen(dest) + strlen(src) > maxsize)
            err_sys("strcat_wrapper: strcat error");
        strcat(dest, src);
    }
    va_end(ap);

    return dest;
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
