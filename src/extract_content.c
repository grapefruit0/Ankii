#include "ankii.h"


extern char buf[MAXLINE];
extern char temp_buf[MAXLINE];


/*
 * the followings strings identify some key locations in the file
 */
const char *is_part_of_speech      = "### [";
const char *is_phrase_head         = "{.phrase-head .dphrase_h}";
const char *is_english_explanation = "{.def .ddef_d .db}";
const char *is_chinese_explanation = "{.def-body .ddef_b}";
const char *is_example             = "{.examp .dexamp}";
const char *is_example_explanation = "{.eg .deg}";
const char *is_expanded_example    = "{.daccord}";

/*
 * some substr that needs to be deleted from the current line buffer
 */
const char *extra_symbols[] = { "### ", "[", "]", "{.eg .deg} ", "*", "\\" };
const int  extra_symbols_size = 6;

/* 
 * delimiter and newline
 */
const char *delimiter      = "<font color='#333399'>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</font>\n";
const char *delimiter_dark = "<font color='#339999'>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</font>\n";
const char *newline        = "<br>\n";
const char *newline_2      = "<br><br>\n";

/*
 * html layout
 */
const char *dark_blue  = "<font color='#333366'>";
const char *mid_blue   = "<font color='#333399'>";
const char *light_blue = "<font color='#3366cc'>";
const char *font_end   = "</font>";
const char *bold       = "<b>";
const char *bold_end   = "</b>";


void
extract_relevant_content(FILE *restrict in_stream, FILE *restrict out_stream)
{
    char *is_extracted_content_beg_tag = "###";
    char *is_extracted_content_end_tag = "[(Translation of ";

    int  is_begin = 1;

    /* 
     * filter irrelevant content
     * 
     * the input markdown file is following
     *   beg: "### ..."
     *   mid: "omitting ..."
     *   end: "[(Translation of ..."
     */

    while (fgets(buf, MAXLINE, in_stream) != NULL) {
        if (strstr(buf, is_extracted_content_beg_tag) != NULL)
            break;
    }

    if (ferror(in_stream))
        err_sys("extract_relevant_content: input error");
    
    do {
        if (strstr(buf, is_extracted_content_end_tag) != NULL)
            break;

        if (strstr(buf, is_part_of_speech) != NULL) {
            if (is_begin == 0)
                fputs_wrapper(newline_2, out_stream);
            else
                is_begin = 0;
                
            fputs_wrapper(delimiter, out_stream);
            fputs_wrapper(newline, out_stream);
            handle_part_of_speech(in_stream);
            fputs_wrapper(buf, out_stream);
        } else if (strstr(buf, is_phrase_head) != NULL) {
            fputs_wrapper(newline_2, out_stream);
            fputs_wrapper(delimiter_dark, out_stream);
            fputs_wrapper(newline, out_stream);
            handle_phrase_head(in_stream);
            fputs_wrapper(buf, out_stream);
        } else if (strstr(buf, is_english_explanation) != NULL) {
            fputs_wrapper(newline_2, out_stream);
            handle_english_explanation(in_stream);
            fputs_wrapper(buf, out_stream);
        } else if (strstr(buf, is_chinese_explanation) != NULL) {
            fputs_wrapper(newline, out_stream);
            handle_chinese_explanation(in_stream);
            fputs_wrapper(buf, out_stream);
        } else if (strstr(buf, is_example) != NULL) {
            fputs_wrapper(newline_2, out_stream);
            handle_example(in_stream);
            fputs_wrapper(buf, out_stream);
        } else if (strstr(buf, is_expanded_example) != NULL) {
            handle_expanded_example(in_stream);
        } else {
            strcpy(buf, "");
        }
    } while (fgets(buf, MAXLINE, in_stream) != NULL);

    if (ferror(in_stream))
        err_sys("extract_relevant_content: input error");
}

void
handle_part_of_speech(FILE *restrict in_stream)
{
    /*
     * ### [simple]{.hw .dsense_hw} [adjective] {omitting ...}
     */

    remove_extra_symbols_and_content();

    /*
     * add html layout
     */
    add_html_layout(IS_BOLD, MID_BLUE_COLOR);

    /* add the char '\n' at the end */
    strcat_wrapper(buf, "\n");
}

void
handle_phrase_head(FILE *restrict in_stream)
{
    /*
     * ::: {.phrase-head .dphrase_h}
     * \* *[**be hard on [sb]{.obj .dobj}**]{.phrase-title .dphrase-title}
     * :::
     */

    /* exclude the first line */
    /* read the next line only */
    fgets(buf, MAXLINE, in_stream);

    if (ferror(in_stream))
        err_sys("handle_phrase_head: input error");

    remove_extra_symbols_and_content();

    /*
     * add html layout
     */
    add_html_layout(IS_BOLD, MID_BLUE_COLOR);

    /* add the char '\n' at the end */
    strcat_wrapper(buf, "\n");
}

void
handle_english_explanation(FILE *restrict in_stream)
{
    /*
     * ::: {.def .ddef_d .db}
     * [easy](https://...){.query}
     * to
     * {omitting ...}
     * :::
     */

    char *is_english_explanation_end_tag = ":::";

    /* exclude the first line */
    strcpy(buf, "");

    while (fgets(temp_buf, MAXLINE, in_stream) != NULL) {
        /* exclude the last line */
        if (strstr(temp_buf, is_english_explanation_end_tag) != NULL)
            break;

        /* remove the last char '\n' */
        temp_buf[strlen(temp_buf) - 1] = ' ';

        strcat_wrapper(buf, temp_buf);
    }

    if (ferror(in_stream))
        err_sys("handle_english_explanation: input error");

    remove_extra_symbols_and_content();

    /* 
     * add html layout
     */
    add_html_layout(IS_BOLD, DARK_BLUE_COLOR);

    /* add the char '\n' at the end */
    strcat_wrapper(buf, "\n");
}

void
handle_chinese_explanation(FILE *restrict in_stream)
{
    /*
     * ::: {.def-body .ddef_b}
     * [简单的，容易的]{.trans .dtrans .dtrans-se .break-cj lang="zh-Hans"}
     */

    /* exclude the first line */
    /* read the next line only */
    fgets(buf, MAXLINE, in_stream);

    if (ferror(in_stream))
        err_sys("handle_chinese_explanation: input error");

    remove_extra_symbols_and_content();

    /* 
     * add html layout
     */
    add_html_layout(NO_BOLD, DARK_BLUE_COLOR);

    /* add the char '\n' at the end */
    strcat_wrapper(buf, "\n");
}

void
handle_example(FILE *restrict in_stream)
{
    /*
     * ::: {.examp .dexamp}
     * [The
     * {omitting ...}
     * [English](https://...){.query}.]{.eg
     * .deg} [这些说明书 ...]
     * {omitting ...}
     * :::
     */

    char *is_example_end_tag = ":::";
    char *buf_mid_substr;

    char temp_buf_2[MAXLINE];    /* temporary storage temp_buf */


    /* exclude the first line */
    strcpy(buf, "");

    while (fgets(temp_buf, MAXLINE, in_stream) != NULL) {
        /* exclude the last line */
        if (strstr(temp_buf, is_example_end_tag) != NULL)
            break;

        /* remove the last char '\n' */
        temp_buf[strlen(temp_buf) - 1] = ' ';

        strcat_wrapper(buf, temp_buf);
    }

    if (ferror(in_stream))
        err_sys("handle_example_english: input error");

    /*
     * split buf into Chinese translation and English example
     *
     * replace ']' to '\n'
     * english example \n relevant chinese translanation
     */
    buf_mid_substr = strstr(buf, is_example_explanation);
    *(buf_mid_substr - 1) = '\0';
    strcpy(temp_buf, buf_mid_substr);
    strcat_wrapper(buf, "\n");
    strcat_wrapper(buf, temp_buf);
    strcat_wrapper(buf, "\n");

    remove_extra_symbols_and_content();

    /* 
     * split buf into chinese translation saved at buf
     * and english example saved at temp_buf
     *
     * add html layout for chinese translation
     *
     * final merge buf and temp_buf
     */
    buf_mid_substr = strstr(buf, "\n");
    *buf_mid_substr = '\0';
    strcpy(temp_buf, buf);
    strcpy(buf, buf_mid_substr + 1);

    strcpy(temp_buf_2, temp_buf);

    add_html_layout(NO_BOLD, LIGHT_BLUE_COLOR);
    
    strcpy(temp_buf, temp_buf_2);

    strcat_wrapper(temp_buf, "\n");
    strcat_wrapper(temp_buf, newline);
    strcat_wrapper(temp_buf, buf);
    strcpy(buf, temp_buf);

    /* add the char '\n' at the end */
    strcat_wrapper(buf, "\n");
}

void
handle_expanded_example(FILE *restrict in_stream)
{
    /*
     * ::: {.daccord}
     * ::: {.section expanded=""}
     * More examples
     *
     * -   The
     *     [scheme](https://...){.query}
     * {omitting ...}
     */

    char *is_expanded_example_end_tag = ":::";

    /*
     * exclude all line
     */
    while (fgets(temp_buf, MAXLINE, in_stream) != NULL) {
        if (strstr(temp_buf, is_expanded_example_end_tag) != NULL)
            break;
    }

    if (ferror(in_stream))
        err_sys("handle_expanded_example: input error");

    strcpy(buf, "");
}

void
remove_extra_symbols_and_content(void)
{
    char *substr_beg;
    char *substr_end;

    /*
     * remove extra symbols
     */
    for (int i = 0; i < extra_symbols_size; ++i) {
        while ((substr_beg = strstr(buf, extra_symbols[i])) != NULL) {
            substr_end = substr_beg + strlen(extra_symbols[i]);

            *substr_beg = '\0';
            strcpy(temp_buf, substr_end);
            strcat_wrapper(buf, temp_buf);
        }
    }

    /*
     * appends the " " string to the buf string
     * to prevent "Segmentation fault (core dumped)" in subsequent operations
     */
    // strcat_wrapper(buf, " ");

    /*
     * remove extra content { some text ... }
     */
    while ((substr_beg = strstr(buf, "{")) != NULL) {
        /* 
         * substr_end = strstr(buf, "}") + 1;
         *
         * for the case of missing char '}'
         */

        substr_end = strstr(buf, "}");

        if (substr_end == NULL)
            substr_end = &buf[strlen(buf) - 1];
        else
            substr_end = substr_end + 1;

        *substr_beg = '\0';
        strcpy(temp_buf, substr_end);
        strcat_wrapper(buf, temp_buf);
    }

    /*
     * remove extra content (https:// .... )
     */
    while ((substr_beg = strstr(buf, "(http")) != NULL) {
        substr_end = strstr(substr_beg, ")") + 1;

        *substr_beg = '\0';
        strcpy(temp_buf, substr_end);
        strcat_wrapper(buf, temp_buf);
    }

    /*
     * remove '\n' at the end
     */
    if ((substr_end = strrchr(buf, '\n')) != NULL)
        *substr_end = '\0';
}

void
add_html_layout(int is_bold, int is_blue)
{
    /* 
     * is_bold = IS_BOLD : add <b> buf </b>
     *
     * is_blue = DARK_BLUE_COLOR  : add <font color='#333366'> buf </font>
     * is_blue = MID_BLUE_COLOR   : add <font color='#333399'> buf </font>
     * is_blue = LIGHT_BLUE_COLOR : add <font color='#3366cc'> buf </font>
     */

    switch (is_bold) {
    case (IS_BOLD):
        strcpy(temp_buf, buf);
        strcpy(buf, bold);
        strcat_wrapper(buf, temp_buf);
        strcat_wrapper(buf, bold_end);
        break;

    case (NO_BOLD):
        /* do nothing */
        break;

    default:
        err_sys("add_html_layout: is_bold error");
    }

    switch (is_blue) {
    case (DARK_BLUE_COLOR):
        strcpy(temp_buf, buf);
        strcpy(buf, dark_blue);
        strcat_wrapper(buf, temp_buf);
        strcat_wrapper(buf, font_end);
        break;

    case (MID_BLUE_COLOR):
        strcpy(temp_buf, buf);
        strcpy(buf, mid_blue);
        strcat_wrapper(buf, temp_buf);
        strcat_wrapper(buf, font_end);
        break;

    case (LIGHT_BLUE_COLOR):
        strcpy(temp_buf, buf);
        strcpy(buf, light_blue);
        strcat_wrapper(buf, temp_buf);
        strcat_wrapper(buf, font_end);
        break;

    case (BLANK_COLOR):
        /* do nothing */
        break;

    default:
        err_sys("add_html_layout: is_blue error");
    }
}  
