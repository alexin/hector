%{

#include <stdio.h>
#include <string.h>

#include "hectorc.h"
#include "ast.h"
#include "args.h"

/*----------------------------------------------------------------------------*/

extern int yylex();
extern char *yytext;

void yyerror(char *message);

/*----------------------------------------------------------------------------*/

%}

%union {
  char *vint;
  char *vfloat;
  struct ast_node *vnode;
}

%type <vnode> Program
%type <vnode> Expression

/*%destructor { ast_free($$); $$ = NULL; } Program
%destructor { ast_free($$); $$ = NULL; } Expression
%destructor { free($$); $$ = NULL; } INTLIT*/

%token <vint> INTLIT
%token <vfloat> FLOATLIT

%left MINUS PLUS
%left AST DIV
%right NEG
%right POW
%left LPAR RPAR

%start Program

%%

Program
  : Expression
    {
      $$ = program = ast_create_program($1);
      if($$ == NULL) {
        ast_free($1);
      }
    }
  ;

Expression
  : INTLIT
    {
      $$ = ast_create_intlit($1);
      if($$ == NULL) {
        free($1);
      }
    }
  | FLOATLIT
    {
      $$ = ast_create_floatlit($1);
      if($$ == NULL) {
        free($1);
      }
    }
  | Expression PLUS Expression
    {
      $$ = ast_create_binary(ast_ADD, $1, $3);
      if($$ == NULL) {
        ast_free($1);
        ast_free($3);
      }
    }
  | Expression MINUS Expression
    {
      $$ = ast_create_binary(ast_SUB, $1, $3);
      if($$ == NULL) {
        ast_free($1);
        ast_free($3);
      }
    }
  | Expression AST Expression
    {
      $$ = ast_create_binary(ast_MUL, $1, $3);
      if($$ == NULL) {
        ast_free($1);
        ast_free($3);
      }
    }
  | Expression DIV Expression
    {
      $$ = ast_create_binary(ast_DIV, $1, $3);
      if($$ == NULL) {
        ast_free($1);
        ast_free($3);
      }
    }
  | MINUS Expression %prec NEG
    {
      $$ = ast_create_unary(ast_MINUS, $2);
      if($$ == NULL) {
        ast_free($2);
      }
    }
  | Expression POW Expression
    {
      $$ = ast_create_binary(ast_POW, $1, $3);
      if($$ == NULL) {
        ast_free($1);
        ast_free($3);
      }
    }
  | LPAR Expression RPAR
    {
      $$ = $2;
    }
  ;


%%

int main(int argc, char **argv) {
  return hc_init(argc, argv);
}

/*----------------------------------------------------------------------------*/

void yyerror(char *message) {
  has_syntax_errors = 1;
  printf(
    "Line %lu, column %lu: %s: %s\n",
    hc_line,
    hc_column - strlen(yytext) + 1,
    message,
    yytext
  );
}