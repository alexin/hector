#include "semantics.h"

#include <stdlib.h>
#include <stdio.h>

#include "hectorc.h"

static
int
check_expression(
  struct ast_node *expr
);

static
int
check_binary_expression(
  struct ast_node *expr
);

static
int
check_unary_expression(
  struct ast_node *expr
);

static
int
check_intlit(
  struct ast_node *intlit
);

/*----------------------------------------------------------------------------*/

int
check_expression(
  struct ast_node *expr
) {
  if(expr == NULL) {
    printf("Expression is null!\n");
    return 0;
  }
  switch(expr->type) {
    case ast_MINUS: return check_unary_expression(expr);

    case ast_ADD: return check_binary_expression(expr);
    case ast_SUB: return check_binary_expression(expr);
    case ast_MUL: return check_binary_expression(expr);
    case ast_DIV: return check_binary_expression(expr);
    case ast_POW: return check_binary_expression(expr);

    case ast_INTLIT: return check_intlit(expr);

    default:
      printf("Unexpected AST node type: %s\n", get_ast_type_str(expr->type));
      return 0;
  }
}

int
check_unary_expression(
  struct ast_node *expr
) {
  struct ast_node *rhs;
  if(expr == NULL) {
    printf("Unary expression is null!\n");
    return 0;
  }
  rhs = expr->child;
  return check_expression(rhs);
}

int
check_binary_expression(
  struct ast_node *expr
) {
  struct ast_node *lhs, *rhs;
  if(expr == NULL) {
    printf("Binary expression is null!\n");
    return 0;
  }
  lhs = expr->child;
  if(lhs == NULL) return 0;
  rhs = expr->child->sibling;
  return check_expression(lhs) && check_expression(rhs);
}

int
check_intlit(
  struct ast_node *intlit
) {
  int value;
  if(intlit == NULL) {
    printf("IntLit is null!\n");
    return 0;
  }
  if(!is_decimal(intlit->value)) {
    printf("Invalid integer literal: %s\n", intlit->value);
    return 0;
  }
  if(!parse_int(intlit->value, &value)) {
    printf("Invalid integer literal: %s\n", intlit->value);
    return 0;
  }
  return 1;
}

/*----------------------------------------------------------------------------*/

int
check_program(
  struct ast_node *program
) {
  if(program == NULL) {
    printf("Program is null!\n");
    return 0;
  }
  if(program->type != ast_PROGRAM) {
    printf("Unknown AST node type: %s\n", get_ast_type_str(program->type));
    return 0;
  }
  return check_expression(program->child);
}
