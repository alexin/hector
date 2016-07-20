#include "translation.h"


#define UNEXPECTED_OPERANDS(L,R) fprintf(stderr,\
  "(%s:%d) Unexpected operand types: %s and %s\n",\
  __FILE__, __LINE__, sem_type_to_str((L)->type), sem_type_to_str((R)->type));

void tr_expr_add (FILE *out, AstNode *add) {
  AstNode *lhs, *rhs;

  if (add->type != ast_ADD) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(add)
    return;
  }

  lhs = ast_get_child_at(0, add);
  rhs = ast_get_child_at(1, add);

  if (lhs->info->type == sem_INT && rhs->info->type == sem_INT) {
    fprintf(out, "");
    tr_expr(out, lhs);
    fprintf(out, " + ");
    tr_expr(out, rhs);
    fprintf(out, "");

  } else if (lhs->info->type == sem_POINT && rhs->info->type == sem_POINT) {
    fprintf(out, "vi32_add_vi32");
    fprintf(out, "((");
    tr_expr(out, lhs);
    fprintf(out, "), ");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, "))");

  } else if (lhs->info->type == sem_MATRIX && rhs->info->type == sem_MATRIX) {
    fprintf(out, "mi32_add_mi32");
    fprintf(out, "((");
    tr_expr(out, lhs);
    fprintf(out, "), ");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, "))");

  } else {
    has_translation_errors = 1;
    UNEXPECTED_OPERANDS(lhs->info, rhs->info)
    return;
  }
}

void tr_expr_mult (FILE *out, AstNode *mult) {
  AstNode *lhs, *rhs;

  if (mult->type != ast_MULT) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(mult)
    return;
  }

  lhs = ast_get_child_at(0, mult);
  rhs = ast_get_child_at(1, mult);

  // int * int
  if (lhs->info->type == sem_INT && rhs->info->type == sem_INT) {
    fprintf(out, "");
    tr_expr(out, lhs);
    fprintf(out, " * ");
    tr_expr(out, rhs);
    fprintf(out, "");

  // int * point
  } else if (lhs->info->type == sem_INT && rhs->info->type == sem_POINT) {
    fprintf(out, "vi32_mult_i32");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, ", ");
    tr_expr(out, lhs);
    fprintf(out, ")");

  // int * matrix
  } else if (lhs->info->type == sem_INT && rhs->info->type == sem_MATRIX) {
    fprintf(out, "mi32_mult_i32");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, ", ");
    tr_expr(out, lhs);
    fprintf(out, ")");

  // point * int
  } else if (lhs->info->type == sem_POINT && rhs->info->type == sem_INT) {
    fprintf(out, "vi32_mult_i32");
    fprintf(out, "(");
    tr_expr(out, lhs);
    fprintf(out, ", ");
    tr_expr(out, rhs);
    fprintf(out, ")");

  // point * matrix
  } else if (lhs->info->type == sem_POINT && rhs->info->type == sem_MATRIX) {
    fprintf(out, "vi32_mult_mi32");
    fprintf(out, "(");
    tr_expr(out, lhs);
    fprintf(out, ", ");
    tr_expr(out, rhs);
    fprintf(out, ")");

  // matrix * int
  } else if (lhs->info->type == sem_MATRIX && rhs->info->type == sem_INT) {
    fprintf(out, "mi32_mult_i32");
    fprintf(out, "(");
    tr_expr(out, lhs);
    fprintf(out, ", ");
    tr_expr(out, rhs);
    fprintf(out, ")");

  // matrix * point
  } else if (lhs->info->type == sem_MATRIX && rhs->info->type == sem_POINT) {
    fprintf(out, "mi32_mult_vi32");
    fprintf(out, "(");
    tr_expr(out, lhs);
    fprintf(out, ", ");
    tr_expr(out, rhs);
    fprintf(out, ")");

  // matrix * matrix
  } else if (lhs->info->type == sem_MATRIX && rhs->info->type == sem_MATRIX) {
    fprintf(out, "mi32_mult_mi32");
    fprintf(out, "(");
    tr_expr(out, lhs);
    fprintf(out, ", ");
    tr_expr(out, rhs);
    fprintf(out, ")");

  } else {
    has_translation_errors = 1;
    UNEXPECTED_OPERANDS(lhs->info, rhs->info)
    return;
  }
}
