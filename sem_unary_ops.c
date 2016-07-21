#include "semantics.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hectorc.h"

#define UNARY_CONFLICT(L,C,O,E) printf(\
  "Line %d, column %d: Operator %s cannot be applied to type %s\n",\
  (L), (C), (O), (E));

SemType can_negate (SemType type) {
  switch (type) {
    case sem_INT: return sem_INT;
    case sem_MATRIX: return sem_UNDEF;
    case sem_POINT: return sem_POINT;
    case sem_UNDEF: return sem_UNDEF;
    case sem_VECTOR: return sem_VECTOR;
  }
}

/*----------------------------------------------------------------------------*/

void check_expr_neg (SemInfo *info, SymTab *tab, AstNode *neg) {
  AstNode *expr;
  SemInfo expr_info;
  SemType result_type;

  if (neg->type != ast_NEG) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(neg)
    return;
  }

  expr = ast_get_child_at(0, neg);
  check_expr(&expr_info, tab, expr);

  result_type = can_negate(expr_info.type);

  if (result_type == sem_UNDEF) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNARY_CONFLICT(
      neg->line, neg->column, "-",
      sem_type_to_str(expr_info.type)
    )
  } else {
    info->type = result_type;
    info->is_lvalue = FALSE;
  }

  neg->info = sem_create_info(info->type, info->is_lvalue);
  if (neg->info == NULL) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    FAILED_MALLOC
    return;
  }
}
