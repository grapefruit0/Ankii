#ifndef _ANKI_H
#define _ANKI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define	MAXLINE	4096			/* max line length */


char buf[MAXLINE];              /* current line buffer */
char temp_buf[MAXLINE];         /* temporary line buffer */


/* 
 * some macros that identify some line html layout
 */
#define  NO_BOLD  0
#define  IS_BOLD  1

#define  BLANK_COLOR       0
#define  DARK_BLUE_COLOR   1
#define  MID_BLUE_COLOR    2
#define  LIGHT_BLUE_COLOR  3


int  fputs_wrapper(const char *s, FILE *restrict stream);
char *strcat_wrapper(char *dest, const char *src);


void	err_sys(const char *, ...) __attribute__((noreturn));


void extract_relevant_content(FILE *restrict in_stream, FILE *restrict out_stream);

void handle_part_of_speech(FILE *restrict in_stream);
void handle_phrase_head(FILE *restrict in_stream);
void handle_english_explanation(FILE *restrict in_stream);
void handle_chinese_explanation(FILE *restrict in_stream);
void handle_example(FILE *restrict in_stream);
void handle_example_chinese(FILE *restrict in_stream);
void handle_expanded_example(FILE *restrict in_stream);

void remove_extra_symbols_and_content(void);

void add_html_layout(int is_bold, int is_blue);


void get_current_date(char *s, int size);
void get_cambridge_dictionary_translanation(const char *word);
void make_anki_card(const char *word, FILE *restrict in_stream, FILE *restrict out_stream);

void delete_intermediate_files(const char *word);


#endif  /* _ANKI_H */
