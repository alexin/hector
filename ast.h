#ifndef H_AST
#define H_AST

#include "hectorc.h"

void ast_print (AstNode *node, unsigned int d);
/* Appends a sibling to end of the list. */
/* Returns the node itself. */
AstNode* ast_add_sibling (AstNode *node, AstNode *sibling);
unsigned int ast_count_siblings (AstNode *node);
void ast_set_location (AstNode *node, int line, int column);
AstNode* ast_get_sibling_by_type (AstType type, AstNode *node);
AstNode* ast_get_child_at (int index, AstNode *parent);

/*----------------------------------------------------------------------------*/

AstNode* ast_create_program (AstNode *nodes);

AstNode* ast_create_vardecl (AstNode *type, char *id, AstNode *init);
AstNode* ast_create_type (AstType type);
AstNode* ast_create_print (AstNode *expr);

AstNode* ast_create_id (char *id);
AstNode* ast_create_assign (char *id, AstNode *expr);
AstNode* ast_create_unary (AstType op, AstNode *expr);
AstNode* ast_create_binary (AstType op, AstNode *lhs, AstNode *rhs);
AstNode* ast_create_at (char *id, AstNode *target);

AstNode* ast_create_intlit (char *value);
AstNode* ast_create_matrixlit (AstNode *comps);
AstNode* ast_create_pointlit (AstNode *comps);

#endif//H_AST
