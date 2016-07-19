#ifndef H_HECTORC
#define H_HECTORC

#include <stdio.h>
#include <stdint.h>

#define TRUE 1
#define FALSE 0

typedef uint8_t u8;

#define UNEXPECTED_NODE(N) fprintf(stderr,\
  "(%s:%d) Unexpected AST node type: %s\n",\
  __FILE__, __LINE__, ast_type_to_str((N)->type));

#define UNEXPECTED_SEM_TYPE(T) fprintf(stderr,\
  "(%s:%d) Unexpected semantic type: %s\n",\
  __FILE__, __LINE__, sem_type_to_str((T)));

#define FAILED_MALLOC fprintf(stderr,\
  "(%s:%d) Failed to allocate memory\n", __FILE__, __LINE__);

/*-- SEMANTICS ---------------------------------------------------------------*/

typedef enum sem_type {
  sem_INT, sem_MATRIX, sem_POINT, sem_UNDEF
} SemType;

const char* sem_type_to_str (SemType type);

typedef struct sem_info {
  SemType type;
  int is_lvalue;
} SemInfo;

void sem_free (SemInfo *info);

/*-- AST ---------------------------------------------------------------------*/

typedef enum ast_type {
  ast_ADD, ast_ASSIGN, ast_ID, ast_INT, ast_INTLIT, ast_MATRIX, ast_MATRIXLIT,
  ast_POINT, ast_POINTLIT, ast_PRINT, ast_PROGRAM, ast_VARDECL
} AstType;

const char* ast_type_to_str (AstType type);

typedef struct ast_node {
  AstType type;
  struct ast_node *sibling;
  struct ast_node *child;
  void *value;
  int line;
  int column;
  SemInfo *info;
} AstNode;

void ast_free (AstNode *ast);

/*-- SYMBOLS -----------------------------------------------------------------*/

typedef enum sym_type {
  sym_VAR
} SymType;

const char* sym_type_to_str (SymType type);

typedef struct symbol {
  SymType sym_type;
  SemType sem_type;
  char *name;
  struct symbol *next;
} Symbol;

typedef struct sym_tab {
  char *name;
  struct symbol *symbols;
  struct sym_tab *parent;
  struct sym_tab *child;
  struct sym_tab *sibling;
} SymTab;

/*----------------------------------------------------------------------------*/

/* The include below undefines some macros. What's the point? */
/* #include "hectorc.lex.h" */

/* 0  = errors only. */
/* 0 != errors and tokens. */
extern int hc_debug;

/* The current line and column in the source file being parsed by the lexical */
/* analyzer. */
/* Can't be 'yy_size_t' because 'hectorc.lex.h' can't be included. */
extern unsigned long hc_line, hc_column;

extern AstNode *program;
extern SymTab *tab;

extern char *hc_input_file;

extern int has_lexical_errors;
extern int has_syntax_errors;
extern int has_semantic_errors;
extern int has_translation_errors;
extern int has_build_errors;

/*----------------------------------------------------------------------------*/

int hc_init (int argc, char **argv);

/*----------------------------------------------------------------------------*/

void tprintf (u8 depth, const char *fmt, ...);
void tfprintf (FILE *out, u8 depth, const char *fmt, ...);

int parse_int (const char *str, int *value);

#endif//H_HECTORC
