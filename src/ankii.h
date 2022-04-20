#ifndef _ANKI_H
#define _ANKI_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>


#define	MAXLINE	4096			/* max line length */
#define SHORT_MAXLINE  100      /* short max line length */

                                /* +1 for terminating null byte */
char buf[MAXLINE + 1];          /* current line buffer */
char temp_buf[MAXLINE + 1];     /* temporary line buffer */


/* 
 * some macros that identify some line html layout
 */
#define  NO_BOLD  0
#define  IS_BOLD  1

#define  BLANK_COLOR       0
#define  DARK_BLUE_COLOR   1
#define  MID_BLUE_COLOR    2
#define  LIGHT_BLUE_COLOR  3


/*
 * file extract_content.c
 */
void make_anki_card(FILE *restrict in_stream, FILE *restrict out_stream, const char *word);

/*
 * file shell.c
 */
void get_current_date(char *s, int size);
void get_cambridge_dictionary_translanation(const char *word);
void delete_intermediate_files(const char *word);
char *modify_input_word(const char *word, char *modified_word);

/*
 * file func_wrapper.c
 */
char *strcat_wrapper(char *dest, const int maxsize, const int va_size, ...);
int  fputs_wrapper(const char *s, FILE *restrict stream);

/*
 * file error.c
 */
void err_sys(const char *, ...) __attribute__((noreturn));


#endif  /* _ANKI_H */
