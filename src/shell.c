#include "ankii.h"


void
get_cambridge_dictionary_translanation(const char *word)
{
    /*
     * delete word_xxx_html and word_xxx_md
     */
    delete_intermediate_files(word);

    /* 
     * $ wget -q https://dictionary.../word_xxx -O word_xxx_html
     *
     * use wget get cambridge dictionary translanation of word
     * and save it in file work_html
     */
    strcpy(buf, "wget -q https://dictionary.cambridge.org/dictionary/english-chinese-simplified/");
    strcat_wrapper(buf, MAXLINE, 4, word, " -O ", word, "_html");
    system(buf);

    /*
     * $ pandoc -f html -t markdown -o word_xxx_md word_xxx_html
     *
     * use pandoc to convert word_xxx_html to word_xxx_markdown
     */
    strcpy(buf, "pandoc -f html -t markdown -o ");
    strcat_wrapper(buf, MAXLINE, 4, word, "_md ", word, "_html");
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
     * $ rm -rf word_xxx_html word_xxx_md
     *
     * delete word_xxx_html and word_xxx_md
     */
    strcpy(buf, "rm -rf ");
    strcat_wrapper(buf, MAXLINE, 4, word, "_html ", word, "_md");
    system(buf);
}

char *
modify_input_word(const char *word, char *modified_word)
{
    char *temp;

    strcpy(modified_word, word);

    while ((temp = strstr(modified_word, " ")) != NULL)
       *temp = '-';

    while ((temp = strstr(modified_word, "/")) != NULL)
       *temp = '-';

    return modified_word;
}
