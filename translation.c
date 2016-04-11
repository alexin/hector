#include "translation.h"

#include <stdlib.h>
#include <stdio.h>

#include "hectorc.h"

#define UNEXPECTED_NODE(N) printf("Unexpected AST node type: %s\n", get_ast_type_str((N)->type));

/*----------------------------------------------------------------------------*/

static
void
tr_expression(
  FILE *out, /* NON-NULL */
  struct ast_node *expr /* NON-NULL */
);

static
void
tr_unary_expression(
  FILE *out, /* NON-NULL */
  struct ast_node *expr /* NON-NULL */
);

static
void
tr_binary_expression(
  FILE *out, /* NON-NULL */
  struct ast_node *expr /* NON-NULL */
);

static
void
tr_intlit(
  FILE *out, /* NON-NULL */
  struct ast_node *intlit /* NON-NULL */
);

static
void
tr_floatlit(
  FILE *out, /* NON-NULL */
  struct ast_node *floatlit /* NON-NULL */
);

/*----------------------------------------------------------------------------*/

static
void
tr_expression(
  FILE *out, /* NON-NULL */
  struct ast_node *expr /* NON-NULL */
) {
  if(ast_is_unary_expression(expr)) {
    tr_unary_expression(out, expr);
  } else if(ast_is_binary_expression(expr)) {
    tr_binary_expression(out, expr);
  } else if(expr->type == ast_INTLIT) {
    tr_intlit(out, expr);
  } else if(expr->type == ast_FLOATLIT) {
    tr_floatlit(out, expr);
  } else {
    UNEXPECTED_NODE(expr)
    has_translation_errors = 1;
  }
}

static
void
tr_unary_expression(
  FILE *out, /* NON-NULL */
  struct ast_node *expr /* NON-NULL */
) {
  switch(expr->type) {
    case ast_MINUS: printf("-"); break;
    default: UNEXPECTED_NODE(expr) has_translation_errors = 1;
  }
  tr_expression(out, expr->child);
}

static
void
tr_binary_expression(
  FILE *out, /* NON-NULL */
  struct ast_node *expr /* NON-NULL */
) {
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

static
void
tr_intlit(
  FILE *out, /* NON-NULL */
  struct ast_node *intlit /* NON-NULL */
) {
  int value;
  parse_int(intlit->value, &value);
  printf("%d", value);
}

static
void
tr_floatlit(
  FILE *out, /* NON-NULL */
  struct ast_node *floatlit /* NON-NULL */
) {
  float value;
  parse_float(floatlit->value, &value);
  printf("%f", value);
}

/*----------------------------------------------------------------------------*/

void
tr_program(
  FILE *out, /* NON-NULL */
  struct ast_node *program /* NON-NULL */
) {
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