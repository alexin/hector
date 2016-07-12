#ifndef H_SEMANTICS
#define H_SEMANTICS

#include "ast.h"

/*----------------------------------------------------------------------------*/

typedef enum sem_type {
  sem_INT, sem_POINT, sem_UNDEF
} SemType;

static const char *sem_type_str[] = {
  "INT", "POINT", "UNDEF"
};

/*----------------------------------------------------------------------------*/

typedef enum sym_type {
  sym_VAR
} SymType;

static const char *sym_type_str[] = {
  "VAR"
};

/*----------------------------------------------------------------------------*/

typedef struct symbol {
  SymType sym_type;
  SemType sem_type;
  char *name;
  struct symbol *next; /* NULLABLE, LIST */
} Symbol;

typedef struct sym_tab {
  char *name;
  struct symbol *symbols;
  struct sym_tab *parent;
  struct sym_tab *child;
  struct sym_tab *sibling;
} SymTab;

/*----------------------------------------------------------------------------*/

const char* get_sym_type_str (const SymType type);

const char* get_sem_type_str (const SemType type);

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
