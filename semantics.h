#ifndef H_SEMANTICS
#define H_SEMANTICS

#include "hectorc.h"
#include "ast.h"

void sym_free_symbol (Symbol *sym);
SymTab* sym_create_tab (const char *name, SymTab *parent);
void sym_free_tab (SymTab *tab);
void sym_add_tab (SymTab *parent, SymTab *child);
Symbol* sym_put (
  SymTab *tab, const SymType sym_type, const SemType sem_type, const char *name
);
Symbol* sym_get (const SymTab *tab, const char *name);
void sym_print_global (const SymTab *global);

/*----------------------------------------------------------------------------*/

int check_program (AstNode *program);

#endif//H_SEMANTICS
