#ifndef H_HECTORC
#define H_HECTORC

#include <stdio.h>
#include <stdint.h>

#define TRUE 1
#define FALSE 0

typedef uint8_t u8;

/* The include below undefines some macros. What's the point? */
/* #include "hectorc.lex.h" */

/* 0  = errors only. */
/* 0 != errors and tokens. */
int hc_debug;

/* The current line and column in the source file being parsed by the lexical */
/* analyzer. */
/* Can't be 'yy_size_t' because 'hectorc.lex.h' can't be included. */
unsigned long hc_line, hc_column;

struct ast_node *program;
struct sym_tab *tab;

int has_lexical_errors;
int has_syntax_errors;
int has_semantic_errors;
int has_translation_errors;

/*----------------------------------------------------------------------------*/

int hc_init (int argc, char **argv);

/*----------------------------------------------------------------------------*/

void tprintf (u8 depth, const char *fmt, ...);
void tfprintf (FILE *out, u8 depth, const char *fmt, ...);

int parse_int (const char *str, int *value);

#endif//H_HECTORC
