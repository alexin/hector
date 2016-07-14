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

#define CANT_ASSIGN(L,C,RHS,LHS) printf(\
  "Line %d, column %d: Cannot assign %s to %s\n", (L), (C), (RHS), (LHS));

#define UNEXPECTED_NODE(N) fprintf(stderr,\
  "Unexpected AST node type (%d): %s\n",\
  __LINE__, get_ast_type_str((N)->type));

#define UNDEFINE(I) (I)->type = sem_UNDEF;

/*----------------------------------------------------------------------------*/

typedef struct sem_info {
  SemType type;
} SemInfo;

/*----------------------------------------------------------------------------*/

const char* get_sem_type_str (SemType type) {
  return sem_type_str[type];
}

int is_assignable (SemType lhs, SemType rhs) {
  switch (lhs) {
    case sem_INT: return rhs == sem_INT;
    case sem_MATRIX: return rhs == sem_MATRIX;
    case sem_POINT: return rhs == sem_POINT;
    case sem_UNDEF: return 0;
  }
}

/*----------------------------------------------------------------------------*/

static void check_stat (SymTab *tab, AstNode *stat);
static void check_stat_vardecl (SymTab *tab, AstNode *decl);
static void check_stat_print (SymTab *tab, AstNode *print);

static void check_expr (SemInfo *info, SymTab *tab, AstNode *expr);
static void check_expr_assign (SemInfo *info, SymTab *tab, AstNode *assign);
static void check_expr_id (SemInfo *info, SymTab *tab, AstNode *id);

static void check_pointlit (SemInfo *info, AstNode *pointlit);
static void check_matrixlit (SemInfo *info, AstNode *matrixlit);
static void check_intlit (SemInfo *info, AstNode *intlit);

/*----------------------------------------------------------------------------*/

void check_stat (SymTab *tab, AstNode *stat) {
  SemInfo info;
  if (stat->type == ast_PRINT) check_stat_print(tab, stat);
  else if (stat->type == ast_VARDECL) check_stat_vardecl(tab, stat);
  else check_expr(&info, tab, stat);
}

void check_stat_print (SymTab *tab, AstNode *print) {
  char *id;
  AstNode *id_node;
  Symbol *sym;

  if (print->type != ast_PRINT) {
    has_semantic_errors = 1;
    UNEXPECTED_NODE(print)
    return;
  }

  id_node = print->child;
  id = (char*) id_node->value;
  sym = sym_get(tab, id);

  // This symbol was never declared.
  if (sym == NULL) {
    has_semantic_errors = 1;
    UNKNOWN_SYMBOL(id_node->line, id_node->column, id)
    return;
  }
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

  // it is OK to use this symbol.
  } else {
    if (type->type == ast_POINT) sym_put(tab, sym_VAR, sem_POINT, id);
    else if (type->type == ast_MATRIX) sym_put(tab, sym_VAR, sem_MATRIX, id);
    else UNEXPECTED_NODE(type)
    sym = sym_get(tab, id);
  }

  // Finally, we check the initializer.
  if (init != NULL) {
    check_expr(&info, tab, init);
    if (!is_assignable(sym->sem_type, info.type)) {
      has_semantic_errors = 1;
      UNDEFINE(&info)
      CANT_ASSIGN(
        decl->line, decl->column,
        get_sem_type_str(info.type), get_sem_type_str(sym->sem_type)
      )
    }
  }
}

void check_expr (SemInfo *info, SymTab *tab, AstNode *expr) {
  if (expr->type == ast_ASSIGN) check_expr_assign(info, tab, expr);
  else if (expr->type == ast_ID) check_expr_id(info, tab, expr);
  else if (expr->type == ast_POINTLIT) check_pointlit(info, expr);
  else if (expr->type == ast_MATRIXLIT) check_matrixlit(info, expr);
  else {
    UNDEFINE(info)
    UNEXPECTED_NODE(expr)
  }
}

void check_expr_assign (SemInfo *info, SymTab *tab, AstNode *assign) {
  char *lhs_id;
  AstNode *lhs_id_node, *rhs;
  Symbol *sym;
  SemInfo rhs_info;

  if (assign->type != ast_ASSIGN) {
    has_semantic_errors = 1;
    UNDEFINE(info)
    UNEXPECTED_NODE(assign)
    return;
  }

  // LHS
  lhs_id_node = assign->child;
  lhs_id = (char*) lhs_id_node->value;
  sym = sym_get(tab, lhs_id);
  // This symbol was never declared.
  if (sym == NULL) {
    has_semantic_errors = 1;
    UNDEFINE(info)
    UNKNOWN_SYMBOL(lhs_id_node->line, lhs_id_node->column, lhs_id)
    return;
  }

  // RHS
  rhs = assign->child->sibling;
  check_expr(&rhs_info, tab, rhs);
  if (!is_assignable(sym->sem_type, rhs_info.type)) {
    has_semantic_errors = 1;
    UNDEFINE(info)
    CANT_ASSIGN(
      assign->line, assign->column,
      get_sem_type_str(rhs_info.type), get_sem_type_str(sym->sem_type)
    )
    return;
  }

  info->type = sym->sem_type;
}

void check_expr_id (SemInfo *info, SymTab *tab, AstNode *id) {
  char *id_str;
  Symbol *sym;

  if (id->type != ast_ID) {
    has_semantic_errors = 1;
    UNDEFINE(info)
    UNEXPECTED_NODE(id)
    return;
  }

  id_str = (char*) id->value;
  sym = sym_get(tab, id_str);
  // This symbol was never declared.
  if (sym == NULL) {
    has_semantic_errors = 1;
    UNDEFINE(info)
    UNKNOWN_SYMBOL(id->line, id->column, id_str)
    return;
  }

  info->type = sym->sem_type;
}

void check_pointlit (SemInfo *info, AstNode *pointlit) {
  AstNode *comps;
  SemInfo comp_info;

  if (pointlit->type != ast_POINTLIT) {
    has_semantic_errors = 1;
    UNDEFINE(info)
    UNEXPECTED_NODE(pointlit)
    return;
  }

  // We start by assuming this POINT is semantically correct...
  info->type = sem_POINT;

  //.. then we check the components, one by one.
  comps = pointlit->child;
  while (comps != NULL) {
    check_intlit(&comp_info, comps);
    // A single invalid component invalidates the whole POINT.
    if (comp_info.type == sem_UNDEF) UNDEFINE(info)
    comps = comps->sibling;
  }
}

void check_matrixlit (SemInfo *info, AstNode *matrixlit) {
  AstNode *comps;
  SemInfo comp_info;

  if (matrixlit->type != ast_MATRIXLIT) {
    has_semantic_errors = 1;
    UNDEFINE(info)
    UNEXPECTED_NODE(matrixlit)
    return;
  }

  // We start by assuming this MATRIX is semantically correct...
  info->type = sem_MATRIX;

  //.. then we check the components, one by one.
  comps = matrixlit->child;
  while (comps != NULL) {
    check_intlit(&comp_info, comps);
    // A single invalid component invalidates the whole MATRIX.
    if (comp_info.type == sem_UNDEF) UNDEFINE(info)
    comps = comps->sibling;
  }
}

void check_intlit (SemInfo *info, AstNode *intlit) {
  int ivalue;
  char *svalue;

  if (intlit->type != ast_INTLIT) {
    has_semantic_errors = 1;
    UNDEFINE(info)
    UNEXPECTED_NODE(intlit)
    return;
  }

  svalue = (char*) intlit->value;

  // We reject integers with leading zeros.
  if (strlen(svalue) > 1 && svalue[0] == '0') {
    has_semantic_errors = 1;
    UNDEFINE(info)
    INVALID_INTLIT(intlit->line, intlit->column, svalue)
  }
  // This is not an integer at all.
  if (!parse_int(svalue, &ivalue)) {
    has_semantic_errors = 1;
    UNDEFINE(info)
    INVALID_INTLIT(intlit->line, intlit->column, svalue)
  }

  info->type = sem_INT; // OK
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
