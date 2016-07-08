%{

#include <stdio.h>
#include <string.h>

#include "hectorc.h"
#include "ast.h"
#include "args.h"

/*----------------------------------------------------------------------------*/

extern int yylex ();
extern char *yytext;

void yyerror (char *message);

AstNode *ast;

/*----------------------------------------------------------------------------*/

%}

%locations

%union {
  char *v_int;
  char *v_str;
  struct ast_node *v_node;
}

%type <v_node> Program
%type <v_node> Declaration
%type <v_node> Stat
%type <v_node> StatList
%type <v_node> PointLiteral

%token <v_str> ID
%token <v_int> INTLIT

%token POINT
%token PRINT
%token SEMI

%left EQUAL
%left OBRACKET CBRACKET

%start Program

/*%destructor { ast_free($$); } Program
%destructor { ast_free($$); } Declaration
%destructor { ast_free($$); } Stat
%destructor { ast_free($$); } StatList
%destructor { ast_free($$); } PointLiteral
%destructor { free($$); } ID
%destructor { free($$); } INTLIT*/

%%

Program
  : StatList {
    if (has_syntax_errors) {
      $$ = NULL;
    } else {
      $$ = ast = program = ast_create_program($1);
      if($$ == NULL) {
        has_syntax_errors = 1;
      }
    }
  }
  ;

Declaration
  : POINT ID EQUAL PointLiteral {
    if (has_syntax_errors) {
      free($2);
      ast_free($4);
      $$ = NULL;
    } else {
      $$ = ast = ast_create_vardecl($2, $4);
      if($$ == NULL) {
        has_syntax_errors = 1;
        free($2);
        ast_free($4);
      } else {
        ast_set_location(
          ast_get_sibling_by_type(ast_ID, $$->child),
          @2.first_line, @2.first_column
        );
      }
    }
  }
  ;

Stat
  : Declaration SEMI {
    $$ = ast = $1;
  }
  | PRINT ID SEMI {
    if (has_syntax_errors) {
      free($2);
      $$ = NULL;
    } else {
      $$ = ast = ast_create_print($2);
      if($$ == NULL) {
        has_syntax_errors = 1;
        free($2);
      } else {
        ast_set_location(
          ast_get_sibling_by_type(ast_ID, $$->child),
          @2.first_line, @2.first_column
        );
      }
    }
  }
  ;

StatList
  : Stat {
    $$ = ast = $1;
  }

  | StatList Stat {
    if(has_syntax_errors) {
      ast_free($1);
      ast_free($2);
      $$ = NULL;
    } else {
      $$ = ast = ast_add_sibling($1, $2);
      if($$ == NULL) {
        has_syntax_errors = 1;
        ast_free($1);
        ast_free($2);
      }
    }
  }
  ;

PointLiteral
  : OBRACKET INTLIT INTLIT INTLIT CBRACKET {
    if (has_syntax_errors) {
      $$ = NULL;
    } else {
      $$ = ast = ast_create_pointlit($2, $3, $4);
      if($$ == NULL) {
        has_syntax_errors = 1;
      } else {
        ast_set_location($$->child, @2.first_line, @2.first_column);
        ast_set_location($$->child->sibling, @3.first_line, @3.first_column);
        ast_set_location(
          $$->child->sibling->sibling, @4.first_line, @4.first_column
        );
      }
    }
  }
  ;

%%

int main (int argc, char **argv) {
  return hc_init(argc, argv);
}

/*----------------------------------------------------------------------------*/

void yyerror (char *message) {
  has_syntax_errors = 1;

  ast_free(ast);

  printf(
    "Line %lu, column %lu: %s: %s\n",
    hc_line, hc_column - strlen(yytext),
    message,
    yytext
  );
}