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

SemInfo* sem_create_info (SemType type, int lvalue);

/*----------------------------------------------------------------------------*/

SemType can_negate (SemType type);

SemType can_add (SemType lhs, SemType rhs);
SemType can_dot (SemType lhs, SemType rhs);
SemType can_assign (SemType lhs, SemType rhs);
SemType can_mult (SemType lhs, SemType rhs);

/*----------------------------------------------------------------------------*/

void check_stat (SymTab *tab, AstNode *stat);
void check_stat_vardecl (SymTab *tab, AstNode *decl);
void check_stat_print (SymTab *tab, AstNode *print);

void check_expr (SemInfo *info, SymTab *tab, AstNode *expr);
void check_expr_id (SemInfo *info, SymTab *tab, AstNode *id);
void check_expr_at (SemInfo *info, SymTab *tab, AstNode *at);

void check_matrixlit (SemInfo *info, AstNode *matrixlit);
void check_pointlit (SemInfo *info, SymTab *tab, AstNode *pointlit);
void check_intlit (SemInfo *info, AstNode *intlit);

void check_expr_neg (SemInfo *info, SymTab *tab, AstNode *neg);

void check_expr_add (SemInfo *info, SymTab *tab, AstNode *add);
void check_expr_dot (SemInfo *info, SymTab *tab, AstNode *dot);
void check_expr_assign (SemInfo *info, SymTab *tab, AstNode *assign);
void check_expr_mult (SemInfo *info, SymTab *tab, AstNode *mult);
void check_expr_sub (SemInfo *info, SymTab *tab, AstNode *sub);

/*----------------------------------------------------------------------------*/

int check_program (AstNode *program);

#endif//H_SEMANTICS
