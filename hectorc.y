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

static AstNode *ast;

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
%type <v_node> Type
%type <v_node> Stat
%type <v_node> StatList
%type <v_node> Expr
%type <v_node> AssignExpr
%type <v_node> PrimaryExpr
%type <v_node> Literal
%type <v_node> PointLit
%type <v_node> MatrixLit

%token <v_str> ID
%token <v_int> INTLIT

%token POINT
%token MATRIX
%token PRINT
%token SEMI

%left EQUAL
%left OBRACKET CBRACKET

%start Program

/*%destructor { ast_free($$); } Program*/

%%

Program
  : StatList {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = program = ast_create_program($1);
      if($$ == NULL) {
        has_syntax_errors = 1;
        ast_free($1);
      }
    }
  }
  ;

Declaration
  : Type ID EQUAL Expr {
    if (has_syntax_errors) {
      free($2);
      ast_free($4);
      $$ = NULL;
    } else {
      $$ = ast = ast_create_vardecl($1, $2, $4);
      if($$ == NULL) {
        has_syntax_errors = 1;
        free($2);
        ast_free($4);
      } else {
        ast_set_location($$, @3.first_line, @3.first_column);
        ast_set_location(ast_get_child_at(1, $$), @2.first_line, @2.first_column);
      }
    }
  }

  | Type ID {
    if (has_syntax_errors) {
      free($2);
      $$ = NULL;
    } else {
      $$ = ast = ast_create_vardecl($1, $2, NULL);
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

Type
  : POINT {
    if (has_syntax_errors) {
      $$ = NULL;
    } else {
      $$ = ast = ast_create_type(ast_POINT);
      if ($$ == NULL) {
        has_syntax_errors = 1;
      } else {
        ast_set_location($$, @1.first_line, @1.first_column);
      }
    }
  }

  | MATRIX {
    if (has_syntax_errors) {
      $$ = NULL;
    } else {
      $$ = ast = ast_create_type(ast_MATRIX);
      if ($$ == NULL) {
        has_syntax_errors = 1;
      } else {
        ast_set_location($$, @1.first_line, @1.first_column);
      }
    }
  }
  ;

Stat
  : Declaration SEMI {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
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

  | Expr SEMI {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
  }
  ;

StatList
  : Stat {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
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

Expr
  : AssignExpr {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
  }
  ;

AssignExpr
  : PrimaryExpr {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
  }

  | ID EQUAL Expr {
    if (has_syntax_errors) {
      $$ = NULL;
      free($1);
      ast_free($3);
    } else {
      $$ = ast = ast_create_assign($1, $3);
      if($$ == NULL) {
        has_syntax_errors = 1;
        free($1);
        ast_free($3);
      } else {
        ast_set_location($$->child, @1.first_line, @1.first_column);
        ast_set_location($$, @2.first_line, @2.first_column);
      }
    }
  }
  ;

PrimaryExpr
  : ID {
    if (has_syntax_errors) {
      $$ = NULL;
      free($1);
    } else {
      $$ = ast = ast_create_id($1);
      if($$ == NULL) {
        has_syntax_errors = 1;
        free($1);
      } else {
        ast_set_location($$, @1.first_line, @1.first_column);
      }
    }
  }

  | Literal {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
  }
  ;

Literal
  : PointLit {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
  }

  | MatrixLit {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
  }
  ;

PointLit
  : OBRACKET INTLIT INTLIT INTLIT CBRACKET {
    if (has_syntax_errors) {
      $$ = NULL;
      free($2);
      free($3);
      free($4);
    } else {
      $$ = ast = ast_create_pointlit($2, $3, $4);
      if($$ == NULL) {
        has_syntax_errors = 1;
        free($2);
        free($3);
        free($4);
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

MatrixLit
  : OBRACKET INTLIT INTLIT INTLIT INTLIT INTLIT INTLIT INTLIT INTLIT INTLIT INTLIT INTLIT INTLIT INTLIT INTLIT INTLIT INTLIT CBRACKET {
    if (has_syntax_errors) {
      $$ = NULL;
      free($2 ); free($3 ); free($4 ); free($5 );
      free($6 ); free($7 ); free($8 ); free($9 );
      free($10); free($11); free($12); free($13);
      free($14); free($15); free($16); free($17);
    } else {
      $$ = ast = ast_create_matrixlit(
        $2, $3, $4, $5,
        $6, $7, $8, $9,
        $10, $11, $12, $13,
        $14, $15, $16, $17
      );
      if($$ == NULL) {
        has_syntax_errors = 1;
        free($2 ); free($3 ); free($4 ); free($5 );
        free($6 ); free($7 ); free($8 ); free($9 );
        free($10); free($11); free($12); free($13);
        free($14); free($15); free($16); free($17);
      } else {
        ast_set_location(ast_get_child_at( 0, $$),  @2.first_line,  @2.first_column);
        ast_set_location(ast_get_child_at( 1, $$),  @3.first_line,  @3.first_column);
        ast_set_location(ast_get_child_at( 2, $$),  @4.first_line,  @4.first_column);
        ast_set_location(ast_get_child_at( 3, $$),  @5.first_line,  @5.first_column);
        ast_set_location(ast_get_child_at( 4, $$),  @6.first_line,  @6.first_column);
        ast_set_location(ast_get_child_at( 5, $$),  @7.first_line,  @7.first_column);
        ast_set_location(ast_get_child_at( 6, $$),  @8.first_line,  @8.first_column);
        ast_set_location(ast_get_child_at( 7, $$),  @9.first_line,  @9.first_column);
        ast_set_location(ast_get_child_at( 8, $$), @10.first_line, @10.first_column);
        ast_set_location(ast_get_child_at( 9, $$), @11.first_line, @11.first_column);
        ast_set_location(ast_get_child_at(10, $$), @12.first_line, @12.first_column);
        ast_set_location(ast_get_child_at(11, $$), @13.first_line, @13.first_column);
        ast_set_location(ast_get_child_at(12, $$), @14.first_line, @14.first_column);
        ast_set_location(ast_get_child_at(13, $$), @15.first_line, @15.first_column);
        ast_set_location(ast_get_child_at(14, $$), @16.first_line, @16.first_column);
        ast_set_location(ast_get_child_at(15, $$), @17.first_line, @17.first_column);
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