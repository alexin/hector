#include "translation.h"

#define UNEXPECTED_OPERAND(O) fprintf(stderr,\
  "(%s:%d) Unexpected operand type: %s\n",\
  __FILE__, __LINE__, sem_type_to_str((O)->type));

void tr_expr_neg (FILE *out, AstNode *neg) {
  AstNode *expr;

  if (neg->type != ast_NEG) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(neg)
    return;
  }

  expr = ast_get_child_at(0, neg);

  if (expr->info->type == sem_INT) {
    fprintf(out, "-(");
    tr_expr(out, expr);
    fprintf(out, ")");

  } else if (expr->info->type == sem_POINT) {
    fprintf(out, "vi32_neg");
    fprintf(out, "(");
    tr_expr(out, expr);
    fprintf(out, ")");

  } else if (expr->info->type == sem_VECTOR) {
    fprintf(out, "vi32_neg");
    fprintf(out, "(");
    tr_expr(out, expr);
    fprintf(out, ")");

  } else {
    has_translation_errors = 1;
    UNEXPECTED_OPERAND(expr->info)
    return;
  }
}

void tr_expr_transpose (FILE *out, AstNode *trp) {
  AstNode *expr;

  if (trp->type != ast_TRANSPOSE) {
    has_translation_errors = 1;
    UNEXPECTED_NODE(trp)
    return;
  }

  expr = ast_get_child_at(0, trp);

  if (expr->info->type == sem_MATRIX) {
    fprintf(out, "mi32_transpose(");
    tr_expr(out, expr);
    fprintf(out, ")");

  } else {
    has_translation_errors = 1;
    UNEXPECTED_OPERAND(expr->info)
    return;
  }
}
