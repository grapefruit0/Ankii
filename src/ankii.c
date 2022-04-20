#include "ankii.h"


extern char buf[MAXLINE + 1];
extern char temp_buf[MAXLINE + 1];

char word_name[SHORT_MAXLINE + 1];           /* +1 for terminating null byte */
char modified_word_name[SHORT_MAXLINE + 1];
char current_date[SHORT_MAXLINE + 1];


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
    strcat_wrapper(buf, MAXLINE, 2, current_date, ".csv");
    out_stream = fopen(buf, "wb");

    in_word_stream = fopen(argv[1], "rb");

    while (fgets(word_name, SHORT_MAXLINE, in_word_stream) != NULL) {
        /* remove the char '\n' at the end */
        word_name[strlen(word_name) - 1] = '\0';

        modify_input_word(word_name, modified_word_name);

        get_cambridge_dictionary_translanation(modified_word_name);

        strcpy(buf, modified_word_name);
        strcat_wrapper(buf, MAXLINE, 1, "_md");
        in_stream = fopen(buf, "rb");

        /* make anki card and output */
        make_anki_card(in_stream, out_stream, word_name);

        delete_intermediate_files(modified_word_name);

        fclose(in_stream);
    }

    if (ferror(in_word_stream))
        err_sys("main: in_word_stream input error");

    fclose(in_word_stream);
    fclose(out_stream);

    exit(EXIT_SUCCESS);
}
