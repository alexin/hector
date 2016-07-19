#include "semantics.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hectorc.h"

#define UNKNOWN_SYMBOL(L,C,S) printf(\
  "Line %d, column %d: Unknown symbol: %s\n", (L), (C), (S));

#define SYMBOL_ALREADY_DEFINED(L,C,S) printf(\
  "Line %d, column %d: Symbol already defined: %s\n", (L), (C), (S));

#define INVALID_INTLIT(L,C,S) printf(\
  "Line %d, column %d: Invalid integer literal: %s\n", (L), (C), (S));

#define CONFLICT(L,C,O,RHS,LHS) printf(\
  "Line %d, column %d: Operator %s cannot be applied to types %s and %s\n",\
  (L), (C), (O), (LHS), (RHS));

/*----------------------------------------------------------------------------*/

static const char *sem_type_str[] = {
  "INT", "MATRIX", "POINT", "UNDEF"
};

const char* sem_type_to_str (SemType type) {
  return sem_type_str[type];
}

static SemInfo* sem_create_info (SemType type, int lvalue) {
  SemInfo *info;
  info = (SemInfo*) malloc(sizeof(SemInfo));
  if (info == NULL) return NULL;
  info->type = type;
  info->is_lvalue = lvalue;
  return info;
}

void sem_free (SemInfo *info) {
  if (info == NULL) return;
  free(info);
}

static int is_assignable (SemType lhs, SemType rhs) {
  switch (lhs) {
    case sem_INT: return rhs == sem_INT;
    case sem_MATRIX: return rhs == sem_MATRIX;
    case sem_POINT: return rhs == sem_POINT;
    case sem_UNDEF: return 0;
  }
}

static SemType can_add (SemType lhs, SemType rhs) {
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

/*----------------------------------------------------------------------------*/

static void check_stat (SymTab *tab, AstNode *stat);
static void check_stat_vardecl (SymTab *tab, AstNode *decl);
static void check_stat_print (SymTab *tab, AstNode *print);

static void check_expr (SemInfo *info, SymTab *tab, AstNode *expr);
static void check_expr_assign (SemInfo *info, SymTab *tab, AstNode *assign);
static void check_expr_add (SemInfo *info, SymTab *tab, AstNode *add);
static void check_expr_id (SemInfo *info, SymTab *tab, AstNode *id);

static void check_matrixlit (SemInfo *info, AstNode *matrixlit);
static void check_pointlit (SemInfo *info, AstNode *pointlit);
static void check_intlit (SemInfo *info, AstNode *intlit);

/*----------------------------------------------------------------------------*/

void check_stat (SymTab *tab, AstNode *stat) {
  SemInfo info;
  if (stat->type == ast_PRINT) check_stat_print(tab, stat);
  else if (stat->type == ast_VARDECL) check_stat_vardecl(tab, stat);
  else check_expr(&info, tab, stat);
}

void check_stat_print (SymTab *tab, AstNode *print) {
  AstNode *expr;
  SemInfo info;

  if (print->type != ast_PRINT) {
    has_semantic_errors = 1;
    UNEXPECTED_NODE(print)
    return;
  }

  expr = ast_get_child_at(0, print);
  check_expr(&info, tab, expr);
}

void check_stat_vardecl (SymTab *tab, AstNode *decl) {
  char *id;
  AstNode *type, *nid, *init;
  Symbol *sym;
  SemInfo info;

  if (decl->type != ast_VARDECL) {
    has_semantic_errors = 1;
    UNEXPECTED_NODE(decl)
    return;
  }

  type = ast_get_child_at(0, decl);
  nid = ast_get_child_at(1, decl);
  init = ast_get_child_at(2, decl);
  id = (char*) nid->value;
  sym = sym_get(tab, id);

  // The symbol has already been used elsewhere.
  if (sym != NULL) {
    has_semantic_errors = 1;
    SYMBOL_ALREADY_DEFINED(nid->line, nid->column, id)

  // It's OK to use this symbol.
  } else {
    if (type->type == ast_INT) sym_put(tab, sym_VAR, sem_INT, id);
    else if (type->type == ast_POINT) sym_put(tab, sym_VAR, sem_POINT, id);
    else if (type->type == ast_MATRIX) sym_put(tab, sym_VAR, sem_MATRIX, id);
    else UNEXPECTED_NODE(type)
    sym = sym_get(tab, id);
  }

  // Finally, we check the initializer.
  if (init != NULL) {
    check_expr(&info, tab, init);
    if (!is_assignable(sym->sem_type, info.type)) {
      has_semantic_errors = 1;
      info.type = sem_UNDEF;
      CONFLICT(
        decl->line, decl->column, "=",
        sem_type_to_str(info.type), sem_type_to_str(sym->sem_type)
      )
    }
  }

  nid->info = sem_create_info(sym->sem_type, TRUE);
  if (nid->info == NULL) {
    has_semantic_errors = 1;
    FAILED_MALLOC
    return;
  }
}

void check_expr (SemInfo *info, SymTab *tab, AstNode *expr) {
  if (expr->type == ast_ASSIGN) check_expr_assign(info, tab, expr);
  else if (expr->type == ast_ADD) check_expr_add(info, tab, expr);
  else if (expr->type == ast_ID) check_expr_id(info, tab, expr);
  else if (expr->type == ast_INTLIT) check_intlit(info, expr);
  else if (expr->type == ast_POINTLIT) check_pointlit(info, expr);
  else if (expr->type == ast_MATRIXLIT) check_matrixlit(info, expr);
  else {
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(expr)
  }
}

void check_expr_assign (SemInfo *info, SymTab *tab, AstNode *assign) {
  AstNode *lhs, *rhs;
  SemInfo lhs_info, rhs_info;

  if (assign->type != ast_ASSIGN) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(assign)
    return;
  }

  // LHS
  lhs = ast_get_child_at(0, assign);
  check_expr_id(&lhs_info, tab, lhs);

  // RHS
  rhs = ast_get_child_at(1, assign);
  check_expr(&rhs_info, tab, rhs);

  if (!is_assignable(lhs_info.type, rhs_info.type)) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    CONFLICT(
      assign->line, assign->column, "=",
      sem_type_to_str(rhs_info.type), sem_type_to_str(lhs_info.type)
    )
  } else {
    info->type = lhs_info.type;
    info->is_lvalue = TRUE;
  }

  assign->info = sem_create_info(info->type, info->is_lvalue);
  if (assign->info == NULL) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    FAILED_MALLOC
    return;
  }
}

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
    CONFLICT(
      add->line, add->column, "=",
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

void check_expr_id (SemInfo *info, SymTab *tab, AstNode *id) {
  char *id_str;
  Symbol *sym;

  if (id->type != ast_ID) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(id)
    return;
  }

  id_str = (char*) id->value;
  sym = sym_get(tab, id_str);

  // This symbol was never declared.
  if (sym == NULL) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNKNOWN_SYMBOL(id->line, id->column, id_str)
  } else {
    info->type = sym->sem_type; // OK
    info->is_lvalue = TRUE;
  }

  id->info = sem_create_info(info->type, info->is_lvalue);
  if (id->info == NULL) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    FAILED_MALLOC
    return;
  }
}

void check_pointlit (SemInfo *info, AstNode *pointlit) {
  AstNode *comps;
  SemInfo comp_info;

  if (pointlit->type != ast_POINTLIT) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(pointlit)
    return;
  }

  // We start by assuming this POINT is semantically correct...
  info->type = sem_POINT;
  info->is_lvalue = FALSE;

  //.. then we check the components, one by one.
  comps = pointlit->child;
  while (comps != NULL) {
    check_intlit(&comp_info, comps);
    // A single invalid component invalidates the whole POINT.
    if (comp_info.type == sem_UNDEF) info->type = sem_UNDEF;
    comps = comps->sibling;
  }

  pointlit->info = sem_create_info(info->type, info->is_lvalue);
  if (pointlit->info == NULL) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    FAILED_MALLOC
    return;
  }
}

void check_matrixlit (SemInfo *info, AstNode *matrixlit) {
  AstNode *comps;
  SemInfo comp_info;

  if (matrixlit->type != ast_MATRIXLIT) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(matrixlit)
    return;
  }

  // We start by assuming this MATRIX is semantically correct...
  info->type = sem_MATRIX;
  info->is_lvalue = FALSE;

  //.. then we check the components, one by one.
  comps = matrixlit->child;
  while (comps != NULL) {
    check_intlit(&comp_info, comps);
    // A single invalid component invalidates the whole MATRIX.
    if (comp_info.type == sem_UNDEF) info->type = sem_UNDEF;
    comps = comps->sibling;
  }

  matrixlit->info = sem_create_info(info->type, info->is_lvalue);
  if (matrixlit->info == NULL) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    FAILED_MALLOC
    return;
  }
}

void check_intlit (SemInfo *info, AstNode *intlit) {
  int ivalue;
  char *svalue;

  if (intlit->type != ast_INTLIT) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(intlit)
    return;
  }

  svalue = (char*) intlit->value;

  // We reject integers with leading zeros.
  if (strlen(svalue) > 1 && svalue[0] == '0') {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    INVALID_INTLIT(intlit->line, intlit->column, svalue)

  // This is not an integer at all.
  } else if (!parse_int(svalue, &ivalue)) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    INVALID_INTLIT(intlit->line, intlit->column, svalue)
  } else {
    info->type = sem_INT; // OK
    info->is_lvalue = FALSE;
  }

  intlit->info = sem_create_info(info->type, info->is_lvalue);
  if (intlit->info == NULL) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    FAILED_MALLOC
    return;
  }
}

/*----------------------------------------------------------------------------*/

int check_program (AstNode *program) {
  AstNode *stat;

  if(program->type != ast_PROGRAM) {
    has_semantic_errors = 1;
    UNEXPECTED_NODE(program)
    return 0;
  }

  stat = program->child;
  while (stat != NULL) {
    check_stat(tab, stat);
    stat = stat->sibling;
  }

  return !has_semantic_errors;
}
