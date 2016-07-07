#ifndef H_SYMBOLS
#define H_SYMBOLS

/*----------------------------------------------------------------------------*/

typedef enum sym_type {
  sym_VAR
} SymType;

typedef struct symbol Symbol;
typedef struct sym_tab SymTab;

struct symbol {
  SymType type;
  char *name;
  Symbol *next; /* NULLABLE, LIST */
};

struct sym_tab {
  char *name;
  Symbol *symbols; /* NULLABLE, LIST */
  SymTab *parent; /* NULLABLE */
  SymTab *child; /* NULLABLE */
  SymTab *sibling; /* NULLABLE, LIST */
};

/*-- SYMBOL ------------------------------------------------------------------*/

void sym_free_symbol (Symbol *sym);

/*-- TABLE -------------------------------------------------------------------*/

SymTab* sym_create_tab (const char *name, SymTab *parent);

void sym_free_tab (SymTab *tab);

void sym_add_tab (SymTab *parent, SymTab *child);

/*----------------------------------------------------------------------------*/

Symbol* sym_put (
  SymTab *tab,
  const SymType type,
  const char *name
);

Symbol* sym_get (const SymTab *tab, const char *name);

void sym_print_global (const SymTab *global);

#endif//H_SYMBOLS
