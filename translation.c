#include "translation.h"

#include <stdlib.h>
#include <stdio.h>

#include "hectorc.h"

#define UNEXPECTED_NODE(N) printf("Unexpected AST node type (%d): %s\n",\
  __LINE__, get_ast_type_str((N)->type));

static FILE *tr_out;

/*----------------------------------------------------------------------------*/

static void tr_stat (u8 depth, AstNode *stat);
static void tr_stat_print (u8 depth, AstNode *print);

static void tr_expr (u8 depth, AstNode *expr);
static void tr_expr_assign (u8 depth, AstNode *assign);
static void tr_expr_id (u8 depth, AstNode *id);

static void tr_pointlit (AstNode *pointlit);
static void tr_intlit (AstNode *intlit);

static void tr_declare_vars (AstNode *program);
static void tr_init_vars (AstNode *program);

/*----------------------------------------------------------------------------*/

void tr_stat (u8 depth, AstNode *stat) {
  if (stat->type == ast_VARDECL) {/* ignore */}
  else if (stat->type == ast_PRINT) tr_stat_print(depth, stat);
  else { // Defaults to expressions.
    tfprintf(tr_out, depth, "");
    tr_expr(depth, stat);
    fprintf(tr_out, ";\n");
  }
}

void tr_stat_print (u8 depth, AstNode *print) {
  char *id;

  if(print->type != ast_PRINT) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(print)
  }

  id = (char*) print->child->value;
  tfprintf(tr_out, depth, "print_vi32(&%s);\n", id);
}

void tr_expr (u8 depth, AstNode *expr) {
  if (expr->type == ast_ASSIGN) tr_expr_assign(depth, expr);
  else if (expr->type == ast_ID) tr_expr_id(depth, expr);
  else if (expr->type == ast_POINTLIT) tr_pointlit(expr);
  else UNEXPECTED_NODE(expr)
}

void tr_expr_assign (u8 depth, AstNode *assign) {
  char *lhs_id;
  AstNode *rhs;

  if(assign->type != ast_ASSIGN) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(assign)
  }

  lhs_id = (char*) assign->child->value;
  fprintf(tr_out, "%s = ", lhs_id);
  rhs = assign->child->sibling;
  tr_expr(depth, rhs);
  //TODO Warning: self assign
}

void tr_expr_id (u8 depth, AstNode *id) {
  char *id_str;

  if(id->type != ast_ID) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(id)
  }

  id_str = (char*) id->value;
  fprintf(tr_out, "%s", id_str);
}

void tr_pointlit (AstNode *pointlit) {
  fprintf(tr_out, "comps_to_vi32(");
  tr_intlit(pointlit->child);
  fprintf(tr_out, ", ");
  tr_intlit(pointlit->child->sibling);
  fprintf(tr_out, ", ");
  tr_intlit(pointlit->child->sibling->sibling);
  fprintf(tr_out, ", 1)");
}

void tr_intlit (AstNode *intlit) {
  int value;
  parse_int(intlit->value, &value);
  fprintf(tr_out, "%d", value);
}

void tr_declare_vars (AstNode *program) {
  AstNode *stat;
  char *id;

  if(program->type != ast_PROGRAM) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(program)
  }

  stat = program->child;
  while (stat != NULL) {
    if (stat->type == ast_VARDECL) {
      id = (char*) stat->child->value;
      tfprintf(tr_out, 0, "static vi32 %s;\n", id);
    }
    stat = stat->sibling;
  }
}

void tr_init_vars (AstNode *program) {
  AstNode *stat, *pointlit;
  char *id;
  int x, y, z;

  if(program->type != ast_PROGRAM) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(program)
  }

  stat = program->child;
  while (stat != NULL) {
    if (stat->type == ast_VARDECL) {
      id = (char*) stat->child->value;
      pointlit = stat->child->sibling;

      if (pointlit == NULL) {
        x = y = z = 0;
      } else {
        parse_int(pointlit->child->value, &x);
        parse_int(pointlit->child->sibling->value, &y);
        parse_int(pointlit->child->sibling->sibling->value, &z);
      }

      tfprintf(tr_out, 1, "set_vi32(&%s, %d, %d, %d, 1);\n", id, x, y, z);
    }
    stat = stat->sibling;
  }
}

/*----------------------------------------------------------------------------*/

int tr_program (FILE *out, AstNode *program) {
  AstNode *stat;

  if(program->type != ast_PROGRAM) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(program)
    return 0;
  }

  tr_out = out;

  tfprintf(tr_out, 0, "#include <stdio.h>\n");
  tfprintf(tr_out, 0, "#include <stdlib.h>\n");
  tfprintf(tr_out, 0, "#include \"lib.h\"\n");
  tfprintf(tr_out, 0, "\n");

  tr_declare_vars(program);

  tfprintf(tr_out, 0, "\n");
  tfprintf(tr_out, 0, "int main (int argc, char **argv) {\n");

  tr_init_vars(program);

  stat = program->child;
  while (stat != NULL) {
    tr_stat(1, stat);
    stat = stat->sibling;
  }

  tfprintf(tr_out, 1, "return EXIT_SUCCESS;\n");
  tfprintf(tr_out, 0, "}\n");

  return !has_translation_errors;
}