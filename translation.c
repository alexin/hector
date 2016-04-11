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
    case ast_MINUS: printf("-"); break;
    case ast_PLUS: printf("+"); break;
    default: UNEXPECTED_NODE(expr) has_translation_errors = 1;
  }
  tr_expression(out, expr->child);
}

void tr_binary_expression(FILE *out, struct ast_node *expr) {
  struct ast_node *lhs, *rhs;

  lhs = expr->child;
  rhs = expr->child->sibling;

  if(expr->type == ast_POW) {
    printf("pow(");
    tr_expression(out, lhs);
    printf(",");
    tr_expression(out, rhs);
    printf(")");
  } else {
    tr_expression(out, lhs);
    switch(expr->type) {
      case ast_ADD: printf(" + "); break;
      case ast_SUB: printf(" - "); break;
      case ast_MUL: printf(" * "); break;
      case ast_DIV: printf(" / "); break;
      default: UNEXPECTED_NODE(expr) has_translation_errors = 1;
    }
    tr_expression(out, rhs);
  }
}

void tr_intlit(FILE *out, struct ast_node *intlit) {
  int value;
  parse_int(intlit->value, &value);
  printf("%d", value);
}

void tr_floatlit(FILE *out, struct ast_node *floatlit) {
  float value;
  parse_float(floatlit->value, &value);
  printf("%f", value);
}

/*----------------------------------------------------------------------------*/

void tr_vector(FILE *out, struct ast_node *vector) {
  struct ast_node *component;
  printf("VECTOR(");
  component = vector->child;
  while(component != NULL) {
    tr_expression(out, component);
    component = component->sibling;
    if(component != NULL) printf(",");
  }
  printf(")");
}

/*----------------------------------------------------------------------------*/

void tr_program(FILE *out, struct ast_node *program) {
  printf("#include <stdio.h>\n");
  printf("#include <stdlib.h>\n");
  printf("#include <math.h>\n\n");
  printf("int main(int argc, char **argv) {\n");
  printf("  float expr;\n");
  printf("  expr = ");
  tr_expression(out, program->child);
  printf(";\n");
  printf("  printf(\"%%f\\n\", expr);\n");
  printf("  return EXIT_SUCCESS;\n");
  printf("}\n");
}