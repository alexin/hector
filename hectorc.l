%{
#include "hectorc.tab.h"

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "hectorc.h"
#include "args.h"

 /* Increments the column count. */
#define IC (hc_column += yyleng)

int lastLine, lastColumn;

 /* Prints to STDOUT if print@mccompiler.h is true. */
static void tprintf(const char *fmt, ...);
static void vtprintf(const char *fmt, va_list argp);

 /* Illegal character error. */
static void ic(const char character);

 /* Unterminated comment error. */
static void uc();

static void on_intlit();
static void on_floatlit();

%}

 /* Decimal integer. May start with zeros. */
intlit                    [0-9]+

  /* Real number. */
floatlit                  [+-]?[0-9]*\.?[0-9]+

 /* Exclusive state to parse comments. This is necessary to "remember"
    the line and column the comment started. */
%x COMMENT
%%

 /* Matches the start of a multiline comment.
    Saves the current line and column. */
"/*"                      { lastLine = hc_line; lastColumn = hc_column+1; IC; BEGIN COMMENT; }

 /* Matches a newline while parsing a comment.
    Increments the line count and resets the column count. */
<COMMENT>\n               { hc_line++; hc_column = 0; }

 /* Matches anything inside a comment. */
<COMMENT>.                { IC; }

 /* Matches an end-of-file while parsing a comment.
    The comment is then unterminated. */
<COMMENT><<EOF>>          { uc(); yyterminate(); }

 /* Matches the end of a multiline comment.
    Returns to the default state. */
<COMMENT>"*/"             { IC; BEGIN INITIAL; }

 /* Matches an integer literal and optionally prints it. */
{intlit}                  { IC; on_intlit(); return INTLIT; }
{floatlit}                 { IC; on_floatlit(); return FLOATLIT; }

"-"                       { IC; tprintf("MINUS\n");   return MINUS; }
"+"                       { IC; tprintf("PLUS\n");    return PLUS; }
"*"                       { IC; tprintf("AST\n");     return AST; }
"/"                       { IC; tprintf("DIV\n");     return DIV; }
"^"                       { IC; tprintf("POW\n");     return POW; }

"("                       { IC; tprintf("LPAR\n");    return LPAR; }
")"                       { IC; tprintf("RPAR\n");    return RPAR; }

 /* Ingores spaces and tabs. */
[ \t]                     { IC; }

 /* Matches a newline.
    Increments the line count and resets the column count. */
\n                        { hc_line++; hc_column = 0; }

 /* Matches anything else and prints an error. */
.                         { IC; ic(yytext[0]); }

%%

int yywrap() {
  return 1;
}

void tprintf(const char *fmt, ...) {
  if(!hc_debug) return;
  va_list argp;
  va_start(argp, fmt);
  vtprintf(fmt, argp);
  va_end(argp);
}

void vtprintf(const char *fmt, va_list argp) {
  vfprintf(stdout, fmt, argp);
}

void uc() {
  has_lexical_errors = 1;
  fprintf(
    stdout,
    "Line %d, column %d: unterminated comment\n",
    lastLine, lastColumn
  );
}

void ic(const char character) {
  has_lexical_errors = 1;
  fprintf(
    stdout,
    "Line %lu, column %lu: illegal character (%c)\n",
    hc_line,
    hc_column - yyleng + 1,
    character
  );
}

void on_intlit() {
  tprintf("INTLIT(%s)\n", yytext);
  yylval.vint = strdup(yytext);
}

void on_floatlit() {
  tprintf("FLOATLIT(%s)\n", yytext);
  yylval.vfloat = strdup(yytext);
}
