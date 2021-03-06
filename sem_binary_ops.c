#include "semantics.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hectorc.h"

#define BINARY_CONFLICT(L,C,O,LHS,RHS) printf(\
  "Line %d, column %d: Operator %s cannot be applied to types %s and %s\n",\
  (L), (C), (O), sem_type_to_str(LHS), sem_type_to_str(RHS));

#define CANT_ASSIGN(L,C,LHS,RHS) printf(\
  "Line %d, column %d: Cannot assign %s to %s\n",\
  (L), (C), sem_type_to_str(RHS), sem_type_to_str(LHS));

#define LHS_NOT_LVALUE(L,C) printf(\
  "Line %d, column %d: Left-hand expression is not an Lvalue\n",\
  (L), (C));

/*----------------------------------------------------------------------------*/

SemType can_add (SemType lhs, SemType rhs) {
  switch (lhs) {

    case sem_INT:
      if (rhs == sem_INT) return sem_INT;
      return sem_UNDEF;

    case sem_MATRIX:
      if (rhs == sem_MATRIX) return sem_MATRIX;
      return sem_UNDEF;

    case sem_POINT:
      if (rhs == sem_POINT) return sem_VECTOR;
      if (rhs == sem_VECTOR) return sem_POINT;
      return sem_UNDEF;

    case sem_UNDEF: return sem_UNDEF;

    case sem_VECTOR:
      if (rhs == sem_POINT) return sem_POINT;
      if (rhs == sem_VECTOR) return sem_VECTOR;
      return sem_UNDEF;
  }
}

SemType can_cross (SemType lhs, SemType rhs) {
  switch (lhs) {

    case sem_INT:
      return sem_UNDEF;

    case sem_MATRIX:
      return sem_UNDEF;

    case sem_POINT:
      if (rhs == sem_POINT) return sem_VECTOR;
      if (rhs == sem_VECTOR) return sem_VECTOR;
      return sem_UNDEF;

    case sem_UNDEF: return sem_UNDEF;

    case sem_VECTOR:
      if (rhs == sem_POINT) return sem_VECTOR;
      if (rhs == sem_VECTOR) return sem_VECTOR;
      return sem_UNDEF;
  }
}

SemType can_dot (SemType lhs, SemType rhs) {
  switch (lhs) {

    case sem_INT:
      return sem_UNDEF;

    case sem_MATRIX:
      return sem_UNDEF;

    case sem_POINT:
      if (rhs == sem_POINT) return sem_INT;
      if (rhs == sem_VECTOR) return sem_INT;
      return sem_UNDEF;

    case sem_UNDEF: return sem_UNDEF;

    case sem_VECTOR:
      if (rhs == sem_POINT) return sem_INT;
      if (rhs == sem_VECTOR) return sem_INT;
      return sem_UNDEF;
  }
}

SemType can_assign (SemType lhs, SemType rhs) {
  switch (lhs) {
    case sem_INT:
      if (rhs == sem_INT) return sem_INT;
      return sem_UNDEF;

    case sem_MATRIX:
      if (rhs == sem_MATRIX) return sem_MATRIX;
      return sem_UNDEF;

    case sem_POINT:
      if (rhs == sem_POINT) return sem_POINT;
      if (rhs == sem_VECTOR) return sem_POINT;
      return sem_UNDEF;

    case sem_UNDEF:
      return sem_UNDEF;

    case sem_VECTOR:
      if (rhs == sem_POINT) return sem_VECTOR;
      if (rhs == sem_VECTOR) return sem_VECTOR;
      return sem_UNDEF;
  }
}

SemType can_mult (SemType lhs, SemType rhs) {
  switch (lhs) {

    case sem_INT:
      if (rhs == sem_INT) return sem_INT;
      if (rhs == sem_MATRIX) return sem_MATRIX;
      if (rhs == sem_POINT) return sem_POINT;
      if (rhs == sem_VECTOR) return sem_VECTOR;
      return sem_UNDEF;

    case sem_MATRIX:
      if (rhs == sem_INT) return sem_MATRIX;
      if (rhs == sem_MATRIX) return sem_MATRIX;
      if (rhs == sem_POINT) return sem_POINT;
      if (rhs == sem_VECTOR) return sem_VECTOR;
      return sem_UNDEF;

    case sem_POINT:
      if (rhs == sem_INT) return sem_POINT;
      if (rhs == sem_MATRIX) return sem_POINT;
      return sem_UNDEF;

    case sem_UNDEF: return sem_UNDEF;

    case sem_VECTOR:
      if (rhs == sem_INT) return sem_VECTOR;
      if (rhs == sem_MATRIX) return sem_VECTOR;
      return sem_UNDEF;
  }
}

SemType can_sub (SemType lhs, SemType rhs) {
  switch (lhs) {

    case sem_INT:
      if (rhs == sem_INT) return sem_INT;
      return sem_UNDEF;

    case sem_MATRIX:
      if (rhs == sem_MATRIX) return sem_MATRIX;
      return sem_UNDEF;

    case sem_POINT:
      if (rhs == sem_POINT) return sem_VECTOR;
      if (rhs == sem_VECTOR) return sem_POINT;
      return sem_UNDEF;

    case sem_UNDEF: return sem_UNDEF;

    case sem_VECTOR:
      if (rhs == sem_POINT) return sem_VECTOR;
      if (rhs == sem_VECTOR) return sem_VECTOR;
      return sem_UNDEF;
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
    BINARY_CONFLICT(add->line, add->column, "+", lhs_info.type, rhs_info.type)
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

void check_expr_assign (SemInfo *info, SymTab *tab, AstNode *assign) {
  AstNode *lhs, *rhs;
  SemInfo lhs_info, rhs_info;
  SemType result_type;

  if (assign->type != ast_ASSIGN) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(assign)
    return;
  }

  // LHS
  lhs = ast_get_child_at(0, assign);
  check_expr(&lhs_info, tab, lhs);

  // RHS
  rhs = ast_get_child_at(1, assign);
  check_expr(&rhs_info, tab, rhs);

  // Checks if the RHS can be assigned to the LHS.
  if (lhs_info.is_lvalue) {
    result_type = can_assign(lhs_info.type, rhs_info.type);
    if (result_type == sem_UNDEF) {
      has_semantic_errors = 1;
      info->type = sem_UNDEF;
      CANT_ASSIGN(assign->line, assign->column, lhs_info.type, rhs_info.type)
    } else {
      info->type = result_type;
      info->is_lvalue = TRUE;
    }

  // The LHS is not an Lvalue.
  } else {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    LHS_NOT_LVALUE(assign->line, assign->column)
  }

  assign->info = sem_create_info(info->type, info->is_lvalue);
  if (assign->info == NULL) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    FAILED_MALLOC
    return;
  }
}

void check_expr_cross (SemInfo *info, SymTab *tab, AstNode *cross) {
  AstNode *lhs, *rhs;
  SemInfo lhs_info, rhs_info;
  SemType result_type;

  if (cross->type != ast_CROSS) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(cross)
    return;
  }

  // LHS
  lhs = ast_get_child_at(0, cross);
  check_expr(&lhs_info, tab, lhs);

  // RHS
  rhs = ast_get_child_at(1, cross);
  check_expr(&rhs_info, tab, rhs);

  result_type = can_cross(lhs_info.type, rhs_info.type);

  if (result_type == sem_UNDEF) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    BINARY_CONFLICT(cross->line, cross->column, ":", lhs_info.type, rhs_info.type)
  } else {
    info->type = result_type;
    info->is_lvalue = FALSE;
  }

  cross->info = sem_create_info(info->type, info->is_lvalue);
  if (cross->info == NULL) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    FAILED_MALLOC
    return;
  }
}

void check_expr_dot (SemInfo *info, SymTab *tab, AstNode *dot) {
  AstNode *lhs, *rhs;
  SemInfo lhs_info, rhs_info;
  SemType result_type;

  if (dot->type != ast_DOT) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(dot)
    return;
  }

  // LHS
  lhs = ast_get_child_at(0, dot);
  check_expr(&lhs_info, tab, lhs);

  // RHS
  rhs = ast_get_child_at(1, dot);
  check_expr(&rhs_info, tab, rhs);

  result_type = can_dot(lhs_info.type, rhs_info.type);

  if (result_type == sem_UNDEF) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    BINARY_CONFLICT(dot->line, dot->column, ".", lhs_info.type, rhs_info.type)
  } else {
    info->type = result_type;
    info->is_lvalue = FALSE;
  }

  dot->info = sem_create_info(info->type, info->is_lvalue);
  if (dot->info == NULL) {
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
    BINARY_CONFLICT(mult->line, mult->column, "*", lhs_info.type, rhs_info.type)
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

void check_expr_sub (SemInfo *info, SymTab *tab, AstNode *sub) {
  AstNode *lhs, *rhs;
  SemInfo lhs_info, rhs_info;
  SemType result_type;

  if (sub->type != ast_SUB) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(sub)
    return;
  }

  // LHS
  lhs = ast_get_child_at(0, sub);
  check_expr(&lhs_info, tab, lhs);

  // RHS
  rhs = ast_get_child_at(1, sub);
  check_expr(&rhs_info, tab, rhs);

  result_type = can_sub(lhs_info.type, rhs_info.type);

  if (result_type == sem_UNDEF) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    BINARY_CONFLICT(sub->line, sub->column, "+", lhs_info.type, rhs_info.type)
  } else {
    info->type = result_type;
    info->is_lvalue = FALSE;
  }

  sub->info = sem_create_info(info->type, info->is_lvalue);
  if (sub->info == NULL) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    FAILED_MALLOC
    return;
  }
}
