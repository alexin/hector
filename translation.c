#include "translation.h"

#include <stdlib.h>
#include <stdio.h>

#include "hectorc.h"

#define UNEXPECTED_OPERANDS(L,R) fprintf(stderr,\
  "(%s:%d) Unexpected operand types: %s and %s\n",\
  __FILE__, __LINE__, sem_type_to_str((L)->type), sem_type_to_str((R)->type));

static FILE *tr_out;

/*----------------------------------------------------------------------------*/

static void tr_stat (u8 depth, AstNode *stat);
static void tr_stat_print (u8 depth, AstNode *print);

static void tr_expr_id (AstNode *id);

static void tr_pointlit (AstNode *pointlit);
static void tr_matrixlit (AstNode *matrixlit);
static void tr_intlit (AstNode *intlit);

static void tr_declare_vars (AstNode *program);
static void tr_init_vars (AstNode *program);
static void tr_init_int (AstNode *stat);
static void tr_init_matrix (AstNode *stat);
static void tr_init_point (AstNode *stat);
static void tr_init_vector (AstNode *stat);

/*----------------------------------------------------------------------------*/

void tr_stat (u8 depth, AstNode *stat) {
  if (stat->type == ast_VARDECL) {/* ignore */}
  else if (stat->type == ast_PRINT) tr_stat_print(depth, stat);
  else { // Defaults to expressions.
    tfprintf(tr_out, depth, "");
    tr_expr(tr_out, stat);
    fprintf(tr_out, ";\n");
  }
}

void tr_stat_print (u8 depth, AstNode *print) {
  AstNode *expr;

  if (print->type != ast_PRINT) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(print)
    return;
  }

  expr = ast_get_child_at(0, print);

  switch (expr->info->type) {

    case sem_INT:
      tfprintf(tr_out, depth, "printf(\"%%d\\n\", ");
      tr_expr(tr_out, expr);
      fprintf(tr_out, ");\n");
      break;

    case sem_MATRIX:
      tfprintf(tr_out, depth, "mi32_print(");
      tr_expr(tr_out, expr);
      fprintf(tr_out, ");\n");
      break;

    case sem_POINT:
      tfprintf(tr_out, depth, "vi32_print(");
      tr_expr(tr_out, expr);
      fprintf(tr_out, ");\n");
      break;

    case sem_VECTOR:
      tfprintf(tr_out, depth, "vi32_print(");
      tr_expr(tr_out, expr);
      fprintf(tr_out, ");\n");
      break;

    default:
      has_translation_errors = 1;
      UNEXPECTED_SEM_TYPE(expr->info->type)
      return;
  }
}

void tr_expr (FILE *out, AstNode *expr) {
       if (expr->type == ast_ADD) tr_expr_add(tr_out, expr);
  else if (expr->type == ast_ASSIGN) tr_expr_assign(tr_out, expr);
  else if (expr->type == ast_ID) tr_expr_id(expr);
  else if (expr->type == ast_INTLIT) tr_intlit(expr);
  else if (expr->type == ast_MATRIXLIT) tr_matrixlit(expr);
  else if (expr->type == ast_MULT) tr_expr_mult(tr_out, expr);
  else if (expr->type == ast_NEG) tr_expr_neg(tr_out, expr);
  else if (expr->type == ast_POINTLIT) tr_pointlit(expr);
  else {
    has_translation_errors = 1;
    UNEXPECTED_NODE(expr)
    return;
  }
}

void tr_expr_id (AstNode *id) {
  char *id_str;

  if (id->type != ast_ID) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(id)
    return;
  }

  id_str = (char*) id->value;
  fprintf(tr_out, "%s", id_str);
  //TODO Trigger a warning when used as a statement.
}

void tr_pointlit (AstNode *pointlit) {
  AstNode *comp;

  if (pointlit->type != ast_POINTLIT) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(pointlit)
    return;
  }

  fprintf(tr_out, "vi32_from_comps(");
  comp = pointlit->child;
  while (comp != NULL) {
    tr_expr(tr_out, comp);
    if (comp->sibling == NULL) fprintf(tr_out, ", 1)");
    else fprintf(tr_out, ", ");
    comp = comp->sibling;
  }
}

void tr_matrixlit (AstNode *matrixlit) {
  AstNode *comp;

  if (matrixlit->type != ast_MATRIXLIT) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(matrixlit)
    return;
  }

  fprintf(tr_out, "mi32_from_comps(");
  comp = matrixlit->child;
  while (comp != NULL) {
    tr_intlit(comp);
    if (comp->sibling == NULL) fprintf(tr_out, ")");
    else fprintf(tr_out, ", ");
    comp = comp->sibling;
  }
}

void tr_intlit (AstNode *intlit) {
  int value;

  if (intlit->type != ast_INTLIT) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(intlit)
    return;
  }

  parse_int(intlit->value, &value);
  fprintf(tr_out, "%d", value);
}

void tr_declare_vars (AstNode *program) {
  AstNode *stat, *type;
  char *id;

  if (program->type != ast_PROGRAM) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(program)
    return;
  }

  stat = program->child;
  while (stat != NULL) {
    if (stat->type == ast_VARDECL) {
      type = ast_get_child_at(0, stat);
      id = (char*) ast_get_child_at(1, stat)->value;

      if (type->type == ast_INT)
        tfprintf(tr_out, 0, "static i32 %s;\n", id);

      else if (type->type == ast_POINT)
        tfprintf(tr_out, 0, "static vi32 %s;\n", id);

      else if (type->type == ast_MATRIX)
        tfprintf(tr_out, 0, "static mi32 %s;\n", id);

      else if (type->type == ast_VECTOR)
        tfprintf(tr_out, 0, "static vi32 %s;\n", id);

      else UNEXPECTED_NODE(type)
    }
    stat = stat->sibling;
  }
}

void tr_init_vars (AstNode *program) {
  AstNode *stat, *type;

  if (program->type != ast_PROGRAM) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(program)
    return;
  }

  stat = program->child;
  while (stat != NULL) {
    if (stat->type == ast_VARDECL) {
      type = ast_get_child_at(0, stat);

           if (type->type == ast_INT) tr_init_int(stat);
      else if (type->type == ast_MATRIX) tr_init_matrix(stat);
      else if (type->type == ast_POINT) tr_init_point(stat);
      else if (type->type == ast_VECTOR) tr_init_vector(stat);
      else UNEXPECTED_NODE(type)
    }
    stat = stat->sibling;
  }
}

void tr_init_int (AstNode *stat) {
  AstNode *expr;
  char *id;

  if (stat->type != ast_VARDECL) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(stat)
    return;
  }
  if (ast_get_child_at(0, stat)->type != ast_INT) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(stat->child)
    return;
  }

  id = (char*) ast_get_child_at(1, stat)->value;
  expr = ast_get_child_at(2, stat);

  if (expr == NULL) {
    tfprintf(tr_out, 1, "%s = 0;\n", id);
  } else {
    tfprintf(tr_out, 1, "%s = ", id);
    tr_expr(tr_out, expr);
    fprintf(tr_out, ";\n");
  }
}

void tr_init_matrix (AstNode *stat) {
  AstNode *expr;
  char *id;

  if (stat->type != ast_VARDECL) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(stat)
    return;
  }
  if (ast_get_child_at(0, stat)->type != ast_MATRIX) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(stat->child)
    return;
  }

  id = (char*) ast_get_child_at(1, stat)->value;
  expr = ast_get_child_at(2, stat);

  if (expr == NULL) {
    tfprintf(tr_out, 1, "mi32_identity(&%s);\n", id);
  } else {
    tfprintf(tr_out, 1, "mi32_set_mi32(&%s, ", id);
    tr_expr(tr_out, expr);
    fprintf(tr_out, ");\n");
  }
}

void tr_init_point (AstNode *stat) {
  AstNode *expr;
  char *id;

  if (stat->type != ast_VARDECL) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(stat)
    return;
  }
  if (ast_get_child_at(0, stat)->type != ast_POINT) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(stat->child)
    return;
  }

  id = (char*) ast_get_child_at(1, stat)->value;
  expr = ast_get_child_at(2, stat);

  if (expr == NULL) {
    tfprintf(tr_out, 1, "vi32_zero(&%s);\n", id);
  } else {
    tfprintf(tr_out, 1, "vi32_set_vi32(&%s, ", id);
    tr_expr(tr_out, expr);
    fprintf(tr_out, ");\n");
  }
}

void tr_init_vector (AstNode *stat) {
  AstNode *expr;
  char *id;

  if (stat->type != ast_VARDECL) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(stat)
    return;
  }
  if (ast_get_child_at(0, stat)->type != ast_VECTOR) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(stat->child)
    return;
  }

  id = (char*) ast_get_child_at(1, stat)->value;
  expr = ast_get_child_at(2, stat);

  if (expr == NULL) {
    tfprintf(tr_out, 1, "vi32_zero(&%s);\n", id);
  } else {
    tfprintf(tr_out, 1, "vi32_set_vi32(&%s, ", id);
    tr_expr(tr_out, expr);
    fprintf(tr_out, ");\n");
  }
}

/*----------------------------------------------------------------------------*/

int tr_program (FILE *out, AstNode *program) {
  AstNode *stat;

  if (program->type != ast_PROGRAM) {
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