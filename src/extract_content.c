#include "ankii.h"


extern char buf[MAXLINE + 1];
extern char temp_buf[MAXLINE + 1];

char word_pos_info[MAXLINE + 1];
char word_pos_header[MAXLINE + 1];
char word_pos_grammar[MAXLINE + 1];

int IS_PART_OF_SPEECH = 0;

/*
 * the followings strings identify some key locations in the file
 */
const char *is_word_pos_header     = "{.pos-header .dpos-h}";
const char *is_word_pos_grammar    = "{.posgram .dpos-g .hdib .lmr-5}";
const char *is_part_of_speech      = "### [";
const char *is_phrase_head         = "{.phrase-head .dphrase_h}";
const char *is_english_explanation = "{.def .ddef_d .db}";
const char *is_chinese_explanation = "{.def-body .ddef_b}";
const char *is_example             = "{.examp .dexamp}";
const char *is_example_explanation = "{.eg .deg}";
const char *is_expanded_example    = "{.daccord}";

const char *is_invalid_file        = "{#popular-searches .lp-m_l-25}";

/*
 * some substr that needs to be deleted from the current line buffer
 */
const char *extra_symbols[] = { "### ", "[[\\[ ", "[", "]", "{.eg .deg} ", "*", "\\" };
const int  extra_symbols_size = 7;
const char *extra_substrs[] = { "(/help/codes.html) " };
const int  extra_substrs_size = 1;

/* 
 * delimiter and newline
 */
const char *delimiter       = "<font color='#333399'>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</font>\n";
const char *delimiter_light = "<font color='#339999'>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</font>\n";
const char *newline         = "<br>\n";
const char *newline_2       = "<br><br>\n";

/*
 * html layout
 */
const char *dark_blue  = "<font color='#333366'>";
const char *mid_blue   = "<font color='#333399'>";
const char *light_blue = "<font color='#3366cc'>";
const char *font_end   = "</font>";
const char *bold       = "<b>";
const char *bold_end   = "</b>";



/*
 * function declaration
 */
void handle_word_pos_header(FILE *restrict in_stream);
void handle_word_pos_grammar(FILE *restrict in_stream);
void handle_part_of_speech(FILE *restrict in_stream);
void handle_phrase_head(FILE *restrict in_stream);
void handle_english_explanation(FILE *restrict in_stream);
void handle_chinese_explanation(FILE *restrict in_stream);
void handle_example(FILE *restrict in_stream);
void handle_expanded_example(FILE *restrict in_stream);

void remove_extra_symbols_and_content(void);
void replace_double_quotes(void);

void add_html_layout(int is_bold, int is_blue);



void
make_anki_card(FILE *restrict in_stream, FILE *restrict out_stream, const char *word)
{
    char *is_extracted_content_beg_tag = "{.pos-header .dpos-h}";
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

    /*
     * ERROR: invalid file
     * print info and terminate
     */
    if (fgets(buf, MAXLINE, in_stream) == NULL) {
        fprintf(stderr, "[-] the card of \"%s\" making failed\n", word);
        return;
    } else {
        if (strcmp(buf, "\n") == 0) {
            fprintf(stderr, "[-] the card of \"%s\" making failed\n", word);
            return;
        }
    }

    do { 
        /*
         * ERROR: invalid file
         * print info and terminate
         */
        if (strstr(buf, is_invalid_file) != NULL) {
            fprintf(stderr, "[-] the card of \"%s\" making failed\n", word);
            return;
        }

        /* the translanation begin at the word pos-header */
        if (strstr(buf, is_extracted_content_beg_tag) != NULL)
            break;
    } while (fgets(buf, MAXLINE, in_stream) != NULL);

    /* 
     * output some info at the begin of out_stream
     *     "<b>word_name</b>" "
     */
    strcpy(temp_buf, "\"<b>");
    strcat_wrapper(temp_buf, MAXLINE, 3, word, "</b>\"", " \"");
    fputs_wrapper(temp_buf, out_stream);

    if (ferror(in_stream))
        err_sys("make_anki_card: input error");
    
    do {
        if (strstr(buf, is_extracted_content_end_tag) != NULL)
            break;

        /*
         * handle `word pos header`
         * save the string of word pos header to word_pos_header
         */
        if (strstr(buf, is_word_pos_header) != NULL) {
            handle_word_pos_header(in_stream);
        }

        /*
         * handle `word pos grammar`
         * save the string of word pos grammar to word_pos_grammar
         */
        else if (strstr(buf, is_word_pos_grammar) != NULL) {
            handle_word_pos_grammar(in_stream);
        }

        /*
         * handle `part of speech`
         */
        else if (strstr(buf, is_part_of_speech) != NULL) {
            if (is_begin == 0)
                fputs_wrapper(newline_2, out_stream);
            else
                is_begin = 0;
                
            fputs_wrapper(delimiter, out_stream);
            fputs_wrapper(newline, out_stream);
            handle_part_of_speech(in_stream);
            fputs_wrapper(buf, out_stream);

            /* set IS_PART_OF_SPEECH */
            IS_PART_OF_SPEECH = 1;
        }

        /* 
         * handle `phrase head`
         */
        else if (strstr(buf, is_phrase_head) != NULL) {
            fputs_wrapper(newline_2, out_stream);
            fputs_wrapper(delimiter_light, out_stream);
            fputs_wrapper(newline, out_stream);
            handle_phrase_head(in_stream);
            fputs_wrapper(buf, out_stream);

            /* set IS_PART_OF_SPEECH */
            IS_PART_OF_SPEECH = 1;
        }

        /*
         * handle `english explanation`
         */
        else if (strstr(buf, is_english_explanation) != NULL) {
            if (IS_PART_OF_SPEECH == 0) {
                /* the translanation begin at english explanation */
                if (is_begin == 0)
                    fputs_wrapper(newline_2, out_stream);
                else
                    is_begin = 0;
                fputs_wrapper(delimiter, out_stream);
                fputs_wrapper(newline, out_stream);

                strcpy(word_pos_info, word_pos_header);
                strcat_wrapper(word_pos_info, MAXLINE, 2, " ", word_pos_grammar);

                /* save the string of buf to prevent overwriting */
                /* add html layout and output word pos header and grammar */
                strcpy(temp_buf, buf);
                strcpy(buf, word_pos_info);
                add_html_layout(IS_BOLD, MID_BLUE_COLOR);
                strcpy(word_pos_info, buf);
                strcpy(buf, temp_buf);
                fputs_wrapper(word_pos_info, out_stream);

                fputs_wrapper(newline_2, out_stream);
            } else {
                fputs_wrapper(newline_2, out_stream);
            }

            handle_english_explanation(in_stream);
            fputs_wrapper(buf, out_stream);

            /* set IS_PART_OF_SPEECH */
            IS_PART_OF_SPEECH = 0;
        }

        /*
         * handle chinese explanation
         */
        else if (strstr(buf, is_chinese_explanation) != NULL) {
            fputs_wrapper(newline, out_stream);
            handle_chinese_explanation(in_stream);
            fputs_wrapper(buf, out_stream);
        }

        /*
         * handle example
         */
        else if (strstr(buf, is_example) != NULL) {
            fputs_wrapper(newline_2, out_stream);
            handle_example(in_stream);
            fputs_wrapper(buf, out_stream);
        }

        /*
         * handle expanded example
         */
        else if (strstr(buf, is_expanded_example) != NULL) {
            handle_expanded_example(in_stream);
        }

        /*
         * handle the other line
         */
        else {
            strcpy(buf, "");
        }
    } while (fgets(buf, MAXLINE, in_stream) != NULL);

    if (ferror(in_stream))
        err_sys("make_anki_card: input error");

    /* 
     * output some date at the end
     */
    fputs_wrapper("\"\n", out_stream);

    /*
     * success
     * print info
     */
    fprintf(stderr, "[+] the card of \"%s\" was made successfully\n", word);
}

void
handle_word_pos_header(FILE *restrict in_stream)
{
    /*
     * ::: {.pos-header .dpos-h}
     * ::: {.di-title}
     * [[moderate]{.hw .dhw}]{.headword .hdb .tw-bw .dhw .dpos-h_hw}
     * :::
     */

    char *is_word_pos_header_end_tag = ":::";

    /* exclude the first line and the second line */
    /* read the thrid line only */
    for (int i = 0; i < 2; ++i)
        fgets(buf, MAXLINE, in_stream);

    if (ferror(in_stream))
        err_sys("handle_word_pos_header: input error");

    remove_extra_symbols_and_content();
    replace_double_quotes();

    /* copy the string to word_pos_header */
    strcpy(word_pos_header, buf);
}

void
handle_word_pos_grammar(FILE *restrict in_stream)
{
    /*
     * ::: {.posgram .dpos-g .hdib .lmr-5}
     * [noun]{.pos .dpos
     * title="A word that refers to a person, place, idea, event or thing."}
     * [[\[ [C]{.gc .dgc} \]](/help/codes.html)]{.gram .dgram}
     * :::
     */

    char *is_word_pos_grammar_end_tag = ":::";

    /* exclude the first line */
    strcpy(buf, "");

    while (fgets(temp_buf, MAXLINE, in_stream) != NULL) {
        /* exclude the last line */
        if (strstr(temp_buf, is_word_pos_grammar_end_tag) != NULL)
            break;

        /* remove the last char '\n' */
        temp_buf[strlen(temp_buf) - 1] = ' ';

        strcat_wrapper(buf, MAXLINE, 1, temp_buf);
    }

    if (ferror(in_stream))
        err_sys("handle_word_pos_grammar: input error");

    remove_extra_symbols_and_content();
    replace_double_quotes();

    /* copy the string to word_pos_header */
    strcpy(word_pos_grammar, buf);
}

void
handle_part_of_speech(FILE *restrict in_stream)
{
    /*
     * ### [simple]{.hw .dsense_hw} [adjective] {omitting ...}
     */

    remove_extra_symbols_and_content();
    replace_double_quotes();

    /*
     * add html layout
     */
    add_html_layout(IS_BOLD, MID_BLUE_COLOR);

    /* add the char '\n' at the end */
    strcat_wrapper(buf, MAXLINE, 1, "\n");
}

void
handle_phrase_head(FILE *restrict in_stream)
{
    /*
     * ::: {.phrase-head .dphrase_h}
     * \* *[**provisions**]{.phrase-title .dphrase-title} [[[\[ [plural]{.gc
     * .dgc} \]](/help/codes.html)]{.gram .dgram}]{.phrase-info .dphrase-info}
     * :::
     */

    char *is_phrase_head_end_tag = ":::";

    /* exclude the first line */
    strcpy(buf, "");

    while (fgets(temp_buf, MAXLINE, in_stream) != NULL) {
        /* exclude the last line */
        if (strstr(temp_buf, is_phrase_head_end_tag) != NULL)
            break;

        /* remove the last char '\n' */
        temp_buf[strlen(temp_buf) - 1] = ' ';

        strcat_wrapper(buf, MAXLINE, 1, temp_buf);
    }

    if (ferror(in_stream))
        err_sys("handle_phrase_head: input error");

    remove_extra_symbols_and_content();
    replace_double_quotes();

    /*
     * add html layout
     */
    add_html_layout(IS_BOLD, MID_BLUE_COLOR);

    /* add the char '\n' at the end */
    strcat_wrapper(buf, MAXLINE, 1, "\n");
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

        strcat_wrapper(buf, MAXLINE, 1, temp_buf);
    }

    if (ferror(in_stream))
        err_sys("handle_english_explanation: input error");

    remove_extra_symbols_and_content();
    replace_double_quotes();

    /* 
     * add html layout
     */
    add_html_layout(IS_BOLD, DARK_BLUE_COLOR);

    /* add the char '\n' at the end */
    strcat_wrapper(buf, MAXLINE, 1, "\n");
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
    replace_double_quotes();

    /* 
     * add html layout
     */
    add_html_layout(NO_BOLD, DARK_BLUE_COLOR);

    /* add the char '\n' at the end */
    strcat_wrapper(buf, MAXLINE, 1, "\n");
}

void
handle_example(FILE *restrict in_stream)
{
    /*
     * ::: {.examp .dexamp}
     * [[\[ [+ that]{.gc .dgc} \]](/zhs/help/codes.html)]{.gram .dgram} [From
     * what she said, the implication was [that]{.b .db} they were
     * [splitting](https://dictionary.cambridge.org/zhs/...
     * up.]{.eg .deg} [她的话中暗示他们将要分手。]{.trans .dtrans .dtrans-se
     * .hdb .break-cj lang="zh-Hans"}
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

        strcat_wrapper(buf, MAXLINE, 1, temp_buf);
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
    strcat_wrapper(buf, MAXLINE, 3, "\n", temp_buf, "\n");

    remove_extra_symbols_and_content();
    replace_double_quotes();

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

    strcat_wrapper(temp_buf, MAXLINE, 3, "\n", newline, buf);
    strcpy(buf, temp_buf);

    /* add the char '\n' at the end */
    strcat_wrapper(buf, MAXLINE, 1, "\n");
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
     * in "[[\[ omitting ... \]]" substr, 
     * replace '[' and ']' to '(' and ')' to prevent deletion
     */
    char *substr_beg_tag  = "[[\\[";
    char *substr_end_tag  = "\\]]";
    char *temp_char;

    if ((substr_beg = strstr(buf, substr_beg_tag)) != NULL) {
        substr_end = strstr(buf, substr_end_tag);

        temp_char = substr_beg + strlen(substr_beg_tag);
        while (temp_char != substr_end) {
            if (*temp_char == '[')
                *temp_char = '(';
            if (*temp_char == ']')
                *temp_char = ')';
            
            ++temp_char;
        }
    }

    /*
     * remove extra symbols
     */
    for (int i = 0; i < extra_symbols_size; ++i) {
        while ((substr_beg = strstr(buf, extra_symbols[i])) != NULL) {
            substr_end = substr_beg + strlen(extra_symbols[i]);

            *substr_beg = '\0';
            strcpy(temp_buf, substr_end);
            strcat_wrapper(buf, MAXLINE, 1, temp_buf);
        }
    }

    /*
     * appends the " " string to the buf string
     * to prevent "Segmentation fault (core dumped)" in subsequent operations
     * 
     * strcat_wrapper(buf, MAXLINE, 1, " ");
     */

    /*
     * remove extra content { some text ... }
     *   or
     * remove extra content { some text ...
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
        strcat_wrapper(buf, MAXLINE, 1, temp_buf);
    }

    /*
     * remove extra content (https:// .... )
     */
    while ((substr_beg = strstr(buf, "(http")) != NULL) {
        substr_end = strstr(substr_beg, ")") + 1;

        *substr_beg = '\0';
        strcpy(temp_buf, substr_end);
        strcat_wrapper(buf, MAXLINE, 1, temp_buf);
    }

    /*
     * remove extra substrs
     */
    for (int i = 0; i < extra_substrs_size; ++i) {
        while ((substr_beg = strstr(buf, extra_substrs[i])) != NULL) {
            substr_end = substr_beg + strlen(extra_substrs[i]);

            *substr_beg = '\0';
            strcpy(temp_buf, substr_end);
            strcat_wrapper(buf, MAXLINE, 1, temp_buf);
        }
    }

    /*
     * remove '\n' at the end
     */
    if ((substr_end = strrchr(buf, '\n')) != NULL)
        *substr_end = '\0';
}

/*
 * replace double quotes with single quotes in string buf
 */
void
replace_double_quotes(void)
{
    char *substr_beg;

    while ((substr_beg = strstr(buf, "\"")) != NULL) {
        *substr_beg = '\'';
    }
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
        strcat_wrapper(buf, MAXLINE, 2, temp_buf, bold_end);
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
        strcat_wrapper(buf, MAXLINE, 2, temp_buf, font_end);
        break;

    case (MID_BLUE_COLOR):
        strcpy(temp_buf, buf);
        strcpy(buf, mid_blue);
        strcat_wrapper(buf, MAXLINE, 2, temp_buf, font_end);
        break;

    case (LIGHT_BLUE_COLOR):
        strcpy(temp_buf, buf);
        strcpy(buf, light_blue);
        strcat_wrapper(buf, MAXLINE, 2, temp_buf, font_end);
        break;

    case (BLANK_COLOR):
        /* do nothing */
        break;

    default:
        err_sys("add_html_layout: is_blue error");
    }
}  
