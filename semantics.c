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

#define BINARY_CONFLICT(L,C,O,LHS,RHS) printf(\
  "Line %d, column %d: Operator %s cannot be applied to types %s and %s\n",\
  (L), (C), (O), (LHS), (RHS));

#define INV_TARGET_TYPE(L,C,T) printf(\
  "Line %d, column %d: Operator @ cannot be applied to type %s\n",\
  (L), (C), sem_type_to_str(T));

#define TARGET_NOT_LVALUE(L,C) printf(\
  "Line %d, column %d: Target is not an Lvalue\n",\
  (L), (C));

#define NOT_ATTR(L,C,A,T) printf(\
  "Line %d, column %d: %s is not an attribute of %s\n",\
  (L), (C), (A), sem_type_to_str(T));

static const char *matrix_attrs[] = {
  "11", "12", "13", "14",
  "21", "22", "23", "24",
  "31", "32", "33", "34",
  "41", "42", "43", "44"
};

static int is_matrix_attribute (const char *attr) {
  int i;
  for (i=0; i < 16; i++)
    if (strcmp(attr, matrix_attrs[i]) == 0) return TRUE;
  return FALSE;
}

static const char *point_attrs[] = {"x", "y", "z"};

static int is_point_attribute (const char *attr) {
  int i;
  for (i=0; i < 3; i++)
    if (strcmp(attr, point_attrs[i]) == 0) return TRUE;
  return FALSE;
}

/*----------------------------------------------------------------------------*/

static const char *sem_type_str[] = {
  "INT", "MATRIX", "POINT", "UNDEF", "VECTOR"
};

const char* sem_type_to_str (SemType type) {
  return sem_type_str[type];
}

SemInfo* sem_create_info (SemType type, int lvalue) {
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
  SemType result_type;

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
    else if (type->type == ast_VECTOR) sym_put(tab, sym_VAR, sem_VECTOR, id);
    else UNEXPECTED_NODE(type)
    sym = sym_get(tab, id);
  }

  // Finally, we check the initializer.
  if (init != NULL) {
    check_expr(&info, tab, init);
    result_type = can_assign(sym->sem_type, info.type);

    if (result_type == sem_UNDEF || result_type != sym->sem_type) {
      has_semantic_errors = 1;
      info.type = sem_UNDEF;
      BINARY_CONFLICT(
        decl->line, decl->column, "=",
        sem_type_to_str(sym->sem_type), sem_type_to_str(info.type)
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
       if (expr->type == ast_ADD) check_expr_add(info, tab, expr);
  else if (expr->type == ast_ASSIGN) check_expr_assign(info, tab, expr);
  else if (expr->type == ast_AT) check_expr_at(info, tab, expr);
  else if (expr->type == ast_CROSS) check_expr_cross(info, tab, expr);
  else if (expr->type == ast_DOT) check_expr_dot(info, tab, expr);
  else if (expr->type == ast_ID) check_expr_id(info, tab, expr);
  else if (expr->type == ast_INTLIT) check_intlit(info, expr);
  else if (expr->type == ast_MATRIXLIT) check_matrixlit(info, expr);
  else if (expr->type == ast_MULT) check_expr_mult(info, tab, expr);
  else if (expr->type == ast_NEG) check_expr_neg(info, tab, expr);
  else if (expr->type == ast_POINTLIT) check_pointlit(info, tab, expr);
  else if (expr->type == ast_SUB) check_expr_sub(info, tab, expr);
  else if (expr->type == ast_TRANSPOSE) check_expr_transpose(info, tab, expr);
  else {
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(expr)
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

void check_expr_at (SemInfo *info, SymTab *tab, AstNode *at) {
  char *attr_id, *target_id;
  AstNode *target;
  SemInfo target_info;

  if (at->type != ast_AT) {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    UNEXPECTED_NODE(at)
    return;
  }

  attr_id = (char*) ast_get_child_at(0, at)->value;
  target = ast_get_child_at(1, at);
  target_id = (char*) target->value;

  check_expr(&target_info, tab, target);

  if (target_info.is_lvalue) {
    switch (target_info.type) {

      case sem_MATRIX:
        if (is_matrix_attribute(attr_id)) {
          info->type = sem_INT;
          info->is_lvalue = TRUE;
        } else {
          has_semantic_errors = 1;
          info->type = sem_UNDEF;
          NOT_ATTR(at->line, at->column, attr_id, target_info.type)
        }
        break;

      case sem_POINT:
        if (is_point_attribute(attr_id)) {
          info->type = sem_INT;
          info->is_lvalue = TRUE;
        } else {
          has_semantic_errors = 1;
          info->type = sem_UNDEF;
          NOT_ATTR(at->line, at->column, attr_id, target_info.type)
        }
        break;

      default:
        has_semantic_errors = 1;
        info->type = sem_UNDEF;
        INV_TARGET_TYPE(at->line, at->column, target_info.type)
    }
  } else {
    has_semantic_errors = 1;
    info->type = sem_UNDEF;
    TARGET_NOT_LVALUE(at->line, at->column)
  }

  at->info = sem_create_info(info->type, info->is_lvalue);
  if (at->info == NULL) {
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

void check_pointlit (SemInfo *info, SymTab *tab, AstNode *pointlit) {
  AstNode *comp;
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
  comp = pointlit->child;
  while (comp != NULL) {
    check_expr(&comp_info, tab, comp);
    // A single invalid component invalidates the whole POINT.
    if (comp_info.type == sem_UNDEF) info->type = sem_UNDEF;
    comp = comp->sibling;
  }

  pointlit->info = sem_create_info(info->type, info->is_lvalue);
  if (pointlit->info == NULL) {
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
