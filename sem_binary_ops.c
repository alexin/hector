#include "semantics.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hectorc.h"

#define BINARY_CONFLICT(L,C,O,RHS,LHS) printf(\
  "Line %d, column %d: Operator %s cannot be applied to types %s and %s\n",\
  (L), (C), (O), (LHS), (RHS));

SemType can_add (SemType lhs, SemType rhs) {
  switch (lhs) {

    case sem_INT:
      if (rhs == sem_INT) return sem_INT;
      return sem_UNDEF;

    case sem_MATRIX:
      if (rhs == sem_MATRIX) return sem_MATRIX;
      return sem_UNDEF;

    case sem_POINT:
      if (rhs == sem_POINT) return sem_POINT;
      return sem_UNDEF;

    case sem_UNDEF: return sem_UNDEF;
  }
}

SemType can_mult (SemType lhs, SemType rhs) {
  switch (lhs) {

    case sem_INT:
      if (rhs == sem_INT) return sem_INT;
      if (rhs == sem_MATRIX) return sem_MATRIX;
      if (rhs == sem_POINT) return sem_POINT;
      return sem_UNDEF;

    case sem_MATRIX:
      if (rhs == sem_INT) return sem_MATRIX;
      if (rhs == sem_MATRIX) return sem_MATRIX;
      if (rhs == sem_POINT) return sem_POINT;
      return sem_UNDEF;

    case sem_POINT:
      if (rhs == sem_INT) return sem_POINT;
      if (rhs == sem_MATRIX) return sem_POINT;
      return sem_UNDEF;

    case sem_UNDEF: return sem_UNDEF;
  }
}

/*----------------------------------------------------------------------------*/

void check_expr_add (SemInfo *info, SymTab *tab, AstNode *add) {
  AstNode *lhs, *rhs;
  SemInfo lhs_info, rhs_info;
  SemType result_type;

  if (add->type != ast_ADD) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(add)
    return;
  }

  // LHS
  lhs = ast_get_child_at(0, add);
  check_expr(&lhs_info, tab, lhs);

  // RHS
  rhs = ast_get_child_at(1, add);
  check_expr(&rhs_info, tab, rhs);

  result_type = can_add(lhs_info.type, rhs_info.type);

  if (result_type == sem_UNDEF) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    BINARY_CONFLICT(
      add->line, add->column, "+",
      sem_type_to_str(rhs_info.type), sem_type_to_str(lhs_info.type)
    )
  } else {
    info->type = result_type;
    info->is_lvalue = FALSE;
  }

  add->info = sem_create_info(info->type, info->is_lvalue);
  if (add->info == NULL) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    FAILED_MALLOC
    return;
  }
}

void check_expr_mult (SemInfo *info, SymTab *tab, AstNode *mult) {
  AstNode *lhs, *rhs;
  SemInfo lhs_info, rhs_info;
  SemType result_type;

  if (mult->type != ast_MULT) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(mult)
    return;
  }

  // LHS
  lhs = ast_get_child_at(0, mult);
  check_expr(&lhs_info, tab, lhs);

  // RHS
  rhs = ast_get_child_at(1, mult);
  check_expr(&rhs_info, tab, rhs);

  result_type = can_mult(lhs_info.type, rhs_info.type);

  if (result_type == sem_UNDEF) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    BINARY_CONFLICT(
      mult->line, mult->column, "*",
      sem_type_to_str(rhs_info.type), sem_type_to_str(lhs_info.type)
    )
  } else {
    info->type = result_type;
    info->is_lvalue = FALSE;
  }

  mult->info = sem_create_info(info->type, info->is_lvalue);
  if (mult->info == NULL) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    FAILED_MALLOC
    return;
  }
}
