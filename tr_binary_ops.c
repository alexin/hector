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

  // int + int
  if (lhs->info->type == sem_INT && rhs->info->type == sem_INT) {
    fprintf(out, "");
    tr_expr(out, lhs);
    fprintf(out, " + ");
    tr_expr(out, rhs);
    fprintf(out, "");

  // matrix + matrix
  } else if (lhs->info->type == sem_MATRIX && rhs->info->type == sem_MATRIX) {
    fprintf(out, "mi32_add_mi32");
    fprintf(out, "((");
    tr_expr(out, lhs);
    fprintf(out, "), ");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, "))");

  // point + point
  } else if (lhs->info->type == sem_POINT && rhs->info->type == sem_POINT) {
    fprintf(out, "vi32_add_vi32");
    fprintf(out, "((");
    tr_expr(out, lhs);
    fprintf(out, "), ");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, "))");

  // point + vector
  } else if (lhs->info->type == sem_POINT && rhs->info->type == sem_VECTOR) {
    fprintf(out, "vi32_add_vi32");
    fprintf(out, "((");
    tr_expr(out, lhs);
    fprintf(out, "), ");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, "))");

  // vector + point
  } else if (lhs->info->type == sem_VECTOR && rhs->info->type == sem_POINT) {
    fprintf(out, "vi32_add_vi32");
    fprintf(out, "((");
    tr_expr(out, lhs);
    fprintf(out, "), ");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, "))");

  // vector + vector
  } else if (lhs->info->type == sem_VECTOR && rhs->info->type == sem_VECTOR) {
    fprintf(out, "vi32_add_vi32");
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

void tr_expr_assign (FILE *out, AstNode *assign) {
  AstNode *lhs, *rhs;

  if (assign->type != ast_ASSIGN) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(assign)
    return;
  }

  lhs = ast_get_child_at(0, assign);
  rhs = ast_get_child_at(1, assign);

  tr_expr(out, lhs);
  fprintf(out, " = ");
  tr_expr(out, rhs);
  //TODO Warning: self assign
}

void tr_expr_dot (FILE *out, AstNode *dot) {
  AstNode *lhs, *rhs;

  if (dot->type != ast_DOT) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(dot)
    return;
  }

  lhs = ast_get_child_at(0, dot);
  rhs = ast_get_child_at(1, dot);

  // point . point
  if (lhs->info->type == sem_POINT && rhs->info->type == sem_POINT) {
    fprintf(out, "vi32_dot_vi32");
    fprintf(out, "((");
    tr_expr(out, lhs);
    fprintf(out, "), ");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, "))");

  // point . vector
  } else if (lhs->info->type == sem_POINT && rhs->info->type == sem_VECTOR) {
    fprintf(out, "vi32_dot_vi32");
    fprintf(out, "((");
    tr_expr(out, lhs);
    fprintf(out, "), ");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, "))");

  // vector . point
  } else if (lhs->info->type == sem_VECTOR && rhs->info->type == sem_POINT) {
    fprintf(out, "vi32_dot_vi32");
    fprintf(out, "((");
    tr_expr(out, lhs);
    fprintf(out, "), ");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, "))");

  // vector . vector
  } else if (lhs->info->type == sem_VECTOR && rhs->info->type == sem_VECTOR) {
    fprintf(out, "vi32_dot_vi32");
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

  // int * matrix
  } else if (lhs->info->type == sem_INT && rhs->info->type == sem_MATRIX) {
    fprintf(out, "mi32_mult_i32");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, ", ");
    tr_expr(out, lhs);
    fprintf(out, ")");

  // int * point
  } else if (lhs->info->type == sem_INT && rhs->info->type == sem_POINT) {
    fprintf(out, "vi32_mult_i32");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, ", ");
    tr_expr(out, lhs);
    fprintf(out, ")");

  // int * vector
  } else if (lhs->info->type == sem_INT && rhs->info->type == sem_VECTOR) {
    fprintf(out, "vi32_mult_i32");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, ", ");
    tr_expr(out, lhs);
    fprintf(out, ")");

  // matrix * int
  } else if (lhs->info->type == sem_MATRIX && rhs->info->type == sem_INT) {
    fprintf(out, "mi32_mult_i32");
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

  // matrix * point
  } else if (lhs->info->type == sem_MATRIX && rhs->info->type == sem_POINT) {
    fprintf(out, "mi32_mult_vi32");
    fprintf(out, "(");
    tr_expr(out, lhs);
    fprintf(out, ", ");
    tr_expr(out, rhs);
    fprintf(out, ")");

  // matrix * vector
  } else if (lhs->info->type == sem_MATRIX && rhs->info->type == sem_VECTOR) {
    fprintf(out, "mi32_mult_vi32");
    fprintf(out, "(");
    tr_expr(out, lhs);
    fprintf(out, ", ");
    tr_expr(out, rhs);
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

  // vector * int
  } else if (lhs->info->type == sem_VECTOR && rhs->info->type == sem_INT) {
    fprintf(out, "vi32_mult_i32");
    fprintf(out, "(");
    tr_expr(out, lhs);
    fprintf(out, ", ");
    tr_expr(out, rhs);
    fprintf(out, ")");

  // vector * matrix
  } else if (lhs->info->type == sem_VECTOR && rhs->info->type == sem_MATRIX) {
    fprintf(out, "vi32_mult_mi32");
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

void tr_expr_sub (FILE *out, AstNode *sub) {
  AstNode *lhs, *rhs;

  if (sub->type != ast_SUB) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(sub)
    return;
  }

  lhs = ast_get_child_at(0, sub);
  rhs = ast_get_child_at(1, sub);

  // int - int
  if (lhs->info->type == sem_INT && rhs->info->type == sem_INT) {
    fprintf(out, "");
    tr_expr(out, lhs);
    fprintf(out, " - ");
    tr_expr(out, rhs);
    fprintf(out, "");

  // matrix - matrix
  } else if (lhs->info->type == sem_MATRIX && rhs->info->type == sem_MATRIX) {
    fprintf(out, "mi32_sub_mi32");
    fprintf(out, "((");
    tr_expr(out, lhs);
    fprintf(out, "), ");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, "))");

  // point - point
  } else if (lhs->info->type == sem_POINT && rhs->info->type == sem_POINT) {
    fprintf(out, "vi32_sub_vi32");
    fprintf(out, "((");
    tr_expr(out, lhs);
    fprintf(out, "), ");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, "))");

  // point - vector
  } else if (lhs->info->type == sem_POINT && rhs->info->type == sem_VECTOR) {
    fprintf(out, "vi32_sub_vi32");
    fprintf(out, "((");
    tr_expr(out, lhs);
    fprintf(out, "), ");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, "))");

  // vector - point
  } else if (lhs->info->type == sem_VECTOR && rhs->info->type == sem_POINT) {
    fprintf(out, "vi32_sub_vi32");
    fprintf(out, "((");
    tr_expr(out, lhs);
    fprintf(out, "), ");
    fprintf(out, "(");
    tr_expr(out, rhs);
    fprintf(out, "))");

  // vector - vector
  } else if (lhs->info->type == sem_VECTOR && rhs->info->type == sem_VECTOR) {
    fprintf(out, "vi32_sub_vi32");
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
