#include "semantics.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hectorc.h"
#include "symbols.h"

#define UNKNOWN_SYMBOL(L,C,S) printf(\
  "Line %d, column %d: Unknown symbol: %s\n", (L), (C), (S));

#define SYMBOL_ALREADY_DEFINED(L,C,S) printf(\
  "Line %d, column %d: Symbol already defined: %s\n", (L), (C), (S));

#define INVALID_INTLIT(L,C,S) printf(\
      "Line %d, column %d: Invalid integer literal: %s\n", (L), (C), (S));

#define UNEXPECTED_NODE(N) printf("Unexpected AST node type: %s\n",\
      get_ast_type_str((N)->type));

static void check_stat (SymTab *tab, AstNode *stat);
static void check_print (SymTab *tab, AstNode *stat);
static void check_vardecl (SymTab *tab, AstNode *stat);
static void check_pointlit (AstNode *pointlit);
static void check_intlit (AstNode *intlit);

/*----------------------------------------------------------------------------*/

void check_stat (SymTab *tab, AstNode *stat) {
  switch(stat->type) {
    case ast_PRINT: check_print(tab, stat); break;
    case ast_VARDECL: check_vardecl(tab, stat); break;

    default:
      has_semantic_errors = 1;
      UNEXPECTED_NODE(stat)
  }
}

void check_print (SymTab *tab, AstNode *stat) {
  char *id;
  AstNode *id_node;
  Symbol *sym;

  if (stat->type != ast_PRINT) {
    has_semantic_errors = 1;
    UNEXPECTED_NODE(stat)
    return;
  }

  id_node = stat->child;
  id = (char*) id_node->value;
  sym = sym_get(tab, id);

  // This symbol was never declared.
  if (sym == NULL) {
    has_semantic_errors = 1;
    UNKNOWN_SYMBOL(id_node->line, id_node->column, id)
    return;
  }
}

void check_vardecl (SymTab *tab, AstNode *stat) {
  char *id;
  AstNode *id_node, *pointlit;
  Symbol *sym;

  if (stat->type != ast_VARDECL) {
    has_semantic_errors = 1;
    UNEXPECTED_NODE(stat)
    return;
  }

  id_node = stat->child;
  pointlit = stat->child->sibling;
  id = (char*) id_node->value;
  sym = sym_get(tab, id);

  // The symbol has already been used elsewhere.
  if (sym != NULL) {
    has_semantic_errors = 1;
    SYMBOL_ALREADY_DEFINED(id_node->line, id_node->column, id)

  // it is OK to use this symbol.
  } else {
    sym_put(tab, sym_VAR, id);
  }

  // We check the initializer.
  check_pointlit(pointlit);
}

void check_pointlit (AstNode *pointlit) {
  AstNode *comps;

  if (pointlit->type != ast_POINTLIT) {
    has_semantic_errors = 1;
    UNEXPECTED_NODE(pointlit)
    return;
  }

  // We check the components of this point.
  comps = pointlit->child;
  while (comps != NULL) {
    check_intlit(comps);
    comps = comps->sibling;
  }
}

void check_intlit (AstNode *intlit) {
  int ivalue;
  char *svalue;

  if (intlit->type != ast_INTLIT) {
    has_semantic_errors = 1;
    UNEXPECTED_NODE(intlit)
    return;
  }

  svalue = (char*) intlit->value;

  // We reject integers with leading zeros.
  if (strlen(svalue) > 1 && svalue[0] == '0') {
    has_semantic_errors = 1;
    INVALID_INTLIT(intlit->line, intlit->column, svalue)
  }
  // This is not an integer at all.
  if (!parse_int(svalue, &ivalue)) {
    has_semantic_errors = 1;
    INVALID_INTLIT(intlit->line, intlit->column, svalue)
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
