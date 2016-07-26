%{
#include "hectorc.tab.h"

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "hectorc.h"
#include "args.h"

#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = hc_line;\
  yylloc.first_column = hc_column; yylloc.last_column = hc_column+yyleng-1;

 /* Increments the column count. */
#define IC (hc_column += yyleng)

 /* Prints to STDOUT if debug is on. */
static void dbg_printf (const char *fmt, ...);

 /* Illegal character error. */
static void on_ic (const char character);

static void on_intlit ();
static void on_id ();

%}

 /* Whitespace character. */
ws                        [ \t\n\r\f]

 /* Valid identifier. May not start with a digit. */
id                        [a-zA-Z_][0-9a-zA-Z_]*

 /* Decimal integer. May start with zeros. */
intlit                    [0-9]+

 /* Real number. */
floatlit                  [0-9]*\.?[0-9]+

 /* Exclusive state to parse comments. This is necessary to "remember"
    the line and column the comment started. */
%x COMMENT
%%

 /* Matches an integer literal and optionally prints it. */
{intlit}                  { IC; on_intlit(); return INTLIT; }

","                       { IC; dbg_printf("COMMA\n"); return COMMA; }
";"                       { IC; dbg_printf("SEMI\n"); return SEMI; }
"["                       { IC; dbg_printf("OBRACKET\n"); return OBRACKET; }
"]"                       { IC; dbg_printf("CBRACKET\n"); return CBRACKET; }
"("                       { IC; dbg_printf("OPAR\n"); return OPAR; }
")"                       { IC; dbg_printf("CPAR\n"); return CPAR; }

"="                       { IC; dbg_printf("EQUAL\n"); return EQUAL; }
"+"                       { IC; dbg_printf("PLUS\n"); return PLUS; }
"-"                       { IC; dbg_printf("MINUS\n"); return MINUS; }
"*"                       { IC; dbg_printf("AST\n"); return AST; }
"@"                       { IC; dbg_printf("AT\n"); return AT; }

"int"                     { IC; dbg_printf("INT\n"); return INT; }
"point"                   { IC; dbg_printf("POINT\n"); return POINT; }
"matrix"                  { IC; dbg_printf("MATRIX\n"); return MATRIX; }
"vector"                  { IC; dbg_printf("VECTOR\n"); return VECTOR; }

"print"                   { IC; dbg_printf("PRINT\n"); return PRINT; }

 /* Matches an identifier and optionally prints it.
    Must come after keywords. */
{id}                      { IC; on_id(); return ID; }

 /* Ingores spaces and tabs. */
[ \t]                     { IC; }

 /* Matches a newline.
    Increments the line count and resets the column count. */
\n                        { hc_line++; hc_column = 1; }

 /* Matches anything else and prints an error. */
.                         { IC; on_ic(yytext[0]); }

%%

int yywrap () {
  return 1;
}

void dbg_printf (const char *fmt, ...) {
  if (!hc_debug) return;
  va_list argp;
  va_start(argp, fmt);
  vfprintf(stdout, fmt, argp);
  va_end(argp);
}

void on_ic (const char character) {
  has_lexical_errors = 1;
  printf(
    "Line %lu, column %lu: illegal character (%c)\n",
    hc_line, hc_column - yyleng,
    character
  );
}

void on_intlit () {
  dbg_printf("INTLIT(%s)\n", yytext);
  yylval.v_int = strdup(yytext);
}

void on_id () {
  dbg_printf("ID(%s)\n", yytext);
  yylval.v_str = strdup(yytext);
}
