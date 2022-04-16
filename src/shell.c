#include "ankii.h"


void
make_anki_card(const char *word, FILE *restrict in_stream, FILE *restrict out_stream)
{
    strcpy(buf, "<b>");
    strcat_wrapper(buf, word);
    strcat_wrapper(buf, "</b>");
    strcat_wrapper(buf, " \"");
    fputs_wrapper(buf, out_stream);

    extract_relevant_content(in_stream, out_stream);

    fputs_wrapper("\"\n", out_stream);
}

void
get_cambridge_dictionary_translanation(const char *word)
{
    /*
     * delete word_xxx_html and word_xxx_md
     */
    delete_intermediate_files(word);

    /* 
     * $ wget https://dictionary.cambridge.org/.../word_xxx > word_xxx_html
     *
     * use wget get cambridge dictionary translanation of word
     * and save it in file work_html
     */
    strcpy(buf, "wget https://dictionary.cambridge.org/dictionary/english-chinese-simplified/");
    strcat_wrapper(buf, word);
    strcat_wrapper(buf, " -O ");
    strcat_wrapper(buf, word);
    strcat_wrapper(buf, "_html");

    system(buf);

    /*
     * $ pandoc -f html -t markdown -o word_xxx_md word_xxx_html
     *
     * use pandoc to convert word_xxx_html to word_xxx_markdown
     */
    strcpy(buf, "pandoc -f html -t markdown -o ");
    strcat_wrapper(buf, word);
    strcat_wrapper(buf, "_md ");
    strcat_wrapper(buf, word);
    strcat_wrapper(buf, "_html");

    system(buf);
}

void
get_current_date(char *s, int size)
{
    system("date +%Y%m%d_%H%M%S > /tmp/current_date");

    FILE *fp = fopen("/tmp/current_date", "rb");

    fgets(s, size, fp);

    /* remove the char '\n' at the end */
    s[strlen(s) - 1] = '\0';

    if (ferror(fp))
        err_sys("get_current_date: input error");

    fclose(fp);
}

void
delete_intermediate_files(const char *word)
{
    /*
     * delete word_xxx_html and word_xxx_md
     */
    strcpy(buf, "rm -r ");
    strcat_wrapper(buf, word);
    strcat_wrapper(buf, "_html ");
    strcat_wrapper(buf, word);
    strcat_wrapper(buf, "_md");
    system(buf);
}
