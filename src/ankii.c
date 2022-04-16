#include "ankii.h"


#define SHORT_MAXLINE  100

extern char buf[MAXLINE];
extern char temp_buf[MAXLINE];

char word_name[SHORT_MAXLINE];
char current_date[SHORT_MAXLINE];


int
main(int argc, char *argv[])
{
    FILE *in_stream;
    FILE *out_stream;
    FILE *in_word_stream;

    if (argc != 2) {
        err_sys("usage: a.out <input_pathname>");
        exit(EXIT_FAILURE);
    }

    get_current_date(current_date, SHORT_MAXLINE);

    strcpy(buf, "anki_");
    strcat_wrapper(buf, current_date);
    strcat_wrapper(buf, ".csv");
    out_stream = fopen(buf, "wb");

    in_word_stream = fopen(argv[1], "rb");

    while (fgets(word_name, SHORT_MAXLINE, in_word_stream) != NULL) {
        /* remove the char '\n' at the end */
        word_name[strlen(word_name) - 1] = '\0';

        get_cambridge_dictionary_translanation(word_name);

        strcpy(buf, word_name);
        strcat_wrapper(buf, "_md");
        in_stream = fopen(buf, "rb");

        make_anki_card(word_name, in_stream, out_stream);

        delete_intermediate_files(word_name);

        fclose(in_stream);
    }

    if (ferror(in_word_stream))
        err_sys("main: in_word_stream input error");

    fclose(in_word_stream);
    fclose(out_stream);

    exit(EXIT_SUCCESS);
}
