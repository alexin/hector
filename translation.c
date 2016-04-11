#include "translation.h"

#include <stdlib.h>
#include <stdio.h>

#include "hectorc.h"

#define UNEXPECTED_NODE(N) printf("Unexpected AST node type: %s\n", get_ast_type_str((N)->type));

/*----------------------------------------------------------------------------*/

static void tr_expression(FILE *out, struct ast_node *expr);
static void tr_unary_expression(FILE *out, struct ast_node *expr);
static void tr_binary_expression(FILE *out, struct ast_node *expr);
static void tr_intlit(FILE *out, struct ast_node *intlit);
static void tr_floatlit(FILE *out, struct ast_node *floatlit);
static void tr_vector(FILE *out, struct ast_node *vector);

/*----------------------------------------------------------------------------*/

void tr_expression(FILE *out, struct ast_node *expr) {
  if(ast_is_unary_expression(expr)) {
    tr_unary_expression(out, expr);

  } else if(ast_is_binary_expression(expr)) {
    tr_binary_expression(out, expr);

  } else if(expr->type == ast_INTLIT) {
    tr_intlit(out, expr);

  } else if(expr->type == ast_FLOATLIT) {
    tr_floatlit(out, expr);

  } else if(expr->type == ast_VECTOR) {
    tr_vector(out, expr);

  } else {
    UNEXPECTED_NODE(expr)
    has_translation_errors = 1;
  }
}

void tr_unary_expression(FILE *out, struct ast_node *expr) {
  switch(expr->type) {
    case ast_MINUS: fprintf(out, "-"); break;
    case ast_PLUS: fprintf(out, "+"); break;
    default: UNEXPECTED_NODE(expr) has_translation_errors = 1;
  }
  tr_expression(out, expr->child);
}

void tr_binary_expression(FILE *out, struct ast_node *expr) {
  struct ast_node *lhs, *rhs;

  lhs = expr->child;
  rhs = expr->child->sibling;

  if(expr->type == ast_POW) {
    fprintf(out, "pow(");
    tr_expression(out, lhs);
    fprintf(out, ",");
    tr_expression(out, rhs);
    fprintf(out, ")");
  } else {
    tr_expression(out, lhs);
    switch(expr->type) {
      case ast_ADD: fprintf(out, " + "); break;
      case ast_SUB: fprintf(out, " - "); break;
      case ast_MUL: fprintf(out, " * "); break;
      case ast_DIV: fprintf(out, " / "); break;
      default: UNEXPECTED_NODE(expr) has_translation_errors = 1;
    }
    tr_expression(out, rhs);
  }
}

void tr_intlit(FILE *out, struct ast_node *intlit) {
  int value;
  parse_int(intlit->value, &value);
  fprintf(out, "%d", value);
}

void tr_floatlit(FILE *out, struct ast_node *floatlit) {
  float value;
  parse_float(floatlit->value, &value);
  fprintf(out, "%f", value);
}

/*----------------------------------------------------------------------------*/

void tr_vector(FILE *out, struct ast_node *vector) {
  struct ast_node *component;
  fprintf(out, "VECTOR(");
  component = vector->child;
  while(component != NULL) {
    tr_expression(out, component);
    component = component->sibling;
    if(component != NULL) fprintf(out, ",");
  }
  fprintf(out, ")");
}

/*----------------------------------------------------------------------------*/

void tr_program(FILE *out, struct ast_node *program) {
  fprintf(out, "#include <stdio.h>\n");
  fprintf(out, "#include <stdlib.h>\n");
  fprintf(out, "#include <math.h>\n\n");
  fprintf(out, "int main(int argc, char **argv) {\n");
  fprintf(out, "  float expr;\n");
  fprintf(out, "  expr = ");
  tr_expression(out, program->child);
  fprintf(out, ";\n");
  fprintf(out, "  printf(\"%%f\\n\", expr);\n");
  fprintf(out, "  return EXIT_SUCCESS;\n");
  fprintf(out, "}\n");
}