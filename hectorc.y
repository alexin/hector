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
%type <v_node> ExprList
%type <v_node> Expr
%type <v_node> AssignExpr
%type <v_node> AddExpr
%type <v_node> MultExpr
%type <v_node> UnaryExpr
%type <v_node> PrefixExpr
%type <v_node> PrimaryExpr
%type <v_node> Literal
%type <v_node> IntLitList

%token <v_str> ID
%token <v_int> INTLIT

%token INT
%token POINT
%token MATRIX
%token VECTOR

%token PRINT
%token SEMI

%right EQUAL
%left PLUS MINUS
%left AST
%right AT
%left OBRACKET CBRACKET OPAR CPAR

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
  : INT {
    if (has_syntax_errors) {
      $$ = NULL;
    } else {
      $$ = ast = ast_create_type(ast_INT);
      if ($$ == NULL) {
        has_syntax_errors = 1;
      } else {
        ast_set_location($$, @1.first_line, @1.first_column);
      }
    }
  }

  | POINT {
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

  | VECTOR {
    if (has_syntax_errors) {
      $$ = NULL;
    } else {
      $$ = ast = ast_create_type(ast_VECTOR);
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

  | PRINT Expr SEMI {
    if (has_syntax_errors) {
      free($2);
      $$ = NULL;
    } else {
      $$ = ast = ast_create_print($2);
      if($$ == NULL) {
        has_syntax_errors = 1;
        free($2);
      } else {
        ast_set_location(ast_get_child_at(0, $$),
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

ExprList
  : Expr ExprList {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
      ast_free($2);
    } else {
      $$ = ast = ast_add_sibling($1, $2);
      if($$ == NULL) {
        has_syntax_errors = 1;
        ast_free($1);
        ast_free($2);
      }
    }
  }

  | Expr {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
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
  : AddExpr {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
  }

  | UnaryExpr EQUAL AssignExpr {
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

AddExpr
  : MultExpr {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
  }

  | AddExpr PLUS MultExpr {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
      ast_free($3);
    } else {
      $$ = ast = ast_create_binary(ast_ADD, $1, $3);
      if($$ == NULL) {
        has_syntax_errors = 1;
        ast_free($1);
        ast_free($3);
      } else {
        ast_set_location($$, @2.first_line, @2.first_column);
      }
    }
  }

  | AddExpr MINUS MultExpr {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
      ast_free($3);
    } else {
      $$ = ast = ast_create_binary(ast_SUB, $1, $3);
      if($$ == NULL) {
        has_syntax_errors = 1;
        ast_free($1);
        ast_free($3);
      } else {
        ast_set_location($$, @2.first_line, @2.first_column);
      }
    }
  }
  ;

MultExpr
  : UnaryExpr {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
  }

  | MultExpr AST UnaryExpr {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
      ast_free($3);
    } else {
      $$ = ast = ast_create_binary(ast_MULT, $1, $3);
      if($$ == NULL) {
        has_syntax_errors = 1;
        ast_free($1);
        ast_free($3);
      } else {
        ast_set_location($$, @2.first_line, @2.first_column);
      }
    }
  }
  ;

UnaryExpr
  : PrefixExpr {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
  }

  | MINUS UnaryExpr {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($2);
    } else {
      $$ = ast = ast_create_unary(ast_NEG, $2);
      if($$ == NULL) {
        has_syntax_errors = 1;
        ast_free($2);
      } else {
        ast_set_location($$, @1.first_line, @1.first_column);
      }
    }
  }
  ;

PrefixExpr
  : PrimaryExpr {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
  }

  | ID AT PrefixExpr {
    if (has_syntax_errors) {
      $$ = NULL;
      free($1);
      ast_free($3);
    } else {
      $$ = ast = ast_create_at($1, $3);
      if($$ == NULL) {
        has_syntax_errors = 1;
        free($1);
        ast_free($3);
      } else {
        ast_set_location($$, @2.first_line, @2.first_column);
        ast_set_location($$->child, @1.first_line, @1.first_column);
      }
    }
  }

  | INTLIT AT PrefixExpr {
    if (has_syntax_errors) {
      $$ = NULL;
      free($1);
      ast_free($3);
    } else {
      $$ = ast = ast_create_at($1, $3);
      if($$ == NULL) {
        has_syntax_errors = 1;
        free($1);
        ast_free($3);
      } else {
        ast_set_location($$, @2.first_line, @2.first_column);
        ast_set_location($$->child, @1.first_line, @1.first_column);
      }
    }
  }
  ;

PrimaryExpr
  : OPAR Expr CPAR {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($2);
    } else {
      $$ = ast = $2;
    }
  }

  | ID {
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
  : INTLIT {
    if (has_syntax_errors) {
      $$ = NULL;
      free($1);
    } else {
      $$ = ast = ast_create_intlit($1);
      if($$ == NULL) {
        has_syntax_errors = 1;
        free($1);
      } else {
        ast_set_location($$, @1.first_line, @1.first_column);
      }
    }
  }

  | IntLitList {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($1);
    } else {
      $$ = ast = $1;
    }
  }
  ;

IntLitList
  : OBRACKET ExprList CBRACKET {
    if (has_syntax_errors) {
      $$ = NULL;
      ast_free($2);
    } else {
      switch (ast_count_siblings($2)) {
        case 3: $$ = ast = ast_create_pointlit($2); break;
        case 16: $$ = ast = ast_create_matrixlit($2); break;
        default: $$ = NULL;
      }
      if($$ == NULL) {
        has_syntax_errors = 1;
        ast_free($2);
      } else {
        ast_set_location($$, @1.first_line, @1.first_column);
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