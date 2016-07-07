#include "symbols.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hectorc.h"

#define MALLOC(TYPE,SIZE) ((TYPE*)malloc((SIZE)*sizeof(TYPE)))

/*-- SYMBOL ------------------------------------------------------------------*/

static Symbol* sym_create_symbol (
  const SymType type,
  const char *name
) {
  Symbol *symbol;

  if (name == NULL) return NULL;

  symbol = MALLOC(Symbol, 1);
  if (symbol == NULL) return NULL;

  symbol->type = type;
  symbol->name = strdup(name);
  if (symbol->name == NULL) {
    free(symbol);
    return NULL;
  }
  symbol->next = NULL;

  return symbol;
}

static Symbol* sym_last_symbol (Symbol *symbol) {
  Symbol *last;
  if (symbol == NULL) return NULL;
  last = symbol;
  while (last->next != NULL) last = last->next;
  return last;
}

void sym_free_symbol (Symbol *symbol) {
  if (symbol == NULL) return;
  sym_free_symbol(symbol->next); symbol->next = NULL;
  free(symbol->name); symbol->name = NULL;
  free(symbol);
}

/*-- TABLE -------------------------------------------------------------------*/

static SymTab* sym_last_tab (SymTab *tab) {
  SymTab *last;
  if (tab == NULL) return NULL;
  last = tab;
  while (last->sibling != NULL) last = last->sibling;
  return last;
}

SymTab* sym_create_tab (const char *name, SymTab *parent) {
  SymTab *tab;

  tab = MALLOC(SymTab, 1);
  if (tab == NULL) return NULL;

  tab->name = strdup(name);
  if (name == NULL) tab->name = "undefined";
  tab->symbols = NULL;
  tab->parent = parent;
  tab->child = NULL;
  tab->sibling = NULL;

  sym_add_tab(parent, tab);

  return tab;
}

void sym_free_tab (SymTab *tab) {
  if(tab == NULL) return;
  sym_free_tab(tab->sibling); tab->sibling = NULL;
  sym_free_tab(tab->child); tab->child = NULL;
  sym_free_symbol(tab->symbols); tab->symbols = NULL;
  tab->parent = NULL; /* Not my responsibility. */
  free(tab->name); tab->name = NULL;
  free(tab);
}

void sym_add_tab (SymTab *parent, SymTab *child) {
  SymTab *last_child;
  if (parent == NULL || child == NULL) return;
  last_child = sym_last_tab(parent->child);
  if (last_child == NULL) {
    parent->child = child;
  } else {
    last_child->sibling = child;
  }
}

/*----------------------------------------------------------------------------*/

Symbol* sym_put (
  SymTab *tab,
  const SymType type,
  const char *name
) {
  Symbol *last_symbol;
  if (tab->symbols == NULL) {
    tab->symbols = sym_create_symbol(type, name);
    return tab->symbols;
  } else {
    last_symbol = sym_last_symbol(tab->symbols);
    last_symbol->next = sym_create_symbol(type, name);
    return tab->symbols;
  }
}

Symbol* sym_get (const SymTab *tab, const char *name) {
  Symbol *it;
  if (tab == NULL) return NULL;
  if (name == NULL) return NULL;
  it = tab->symbols;
  while (it != NULL) {
    if (strcmp(it->name, name) == 0) return it;
    it = it->next;
  }
  return NULL;
}

void sym_print_global (const SymTab *global) {
  const Symbol *symbol;

  printf("===== Global Symbol Table =====\n");
  symbol = global->symbols;
  while (symbol != NULL) {
    printf("%s\t", symbol->name);
    printf("\n");
    symbol = symbol->next;
  }
}
