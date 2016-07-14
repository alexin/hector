#ifndef H_AST
#define H_AST

/*----------------------------------------------------------------------------*/

typedef enum ast_type {

  // Edit get_ast_type_str if you make any changes to this enum.
  // Alphabetical order.

  ast_ASSIGN, ast_ID, ast_INTLIT, ast_MATRIX, ast_MATRIXLIT, ast_POINT,
  ast_POINTLIT, ast_PRINT, ast_PROGRAM, ast_VARDECL

} AstType;

static const char *ast_type_str[] = {
  "ASSIGN", "ID", "INTLIT", "MATRIX", "MATRIXLIT", "POINT", "POINTLIT",
  "PRINT", "PROGRAM", "VARDECL"
};

typedef struct ast_node {
  AstType type;
  struct ast_node *sibling;
  struct ast_node *child;
  void *value;
  int line;
  int column;
} AstNode;

/*----------------------------------------------------------------------------*/

const char* get_ast_type_str (const AstType type);

void ast_print (AstNode *node, const unsigned int d);

/* Appends a sibling to end of the list. */
/* Returns the node itself. */
AstNode* ast_add_sibling (AstNode *node, AstNode *sibling);

unsigned int ast_count_siblings (AstNode *node);

void ast_free (AstNode *ast);

void ast_set_location (AstNode *node, int line, int column);

AstNode* ast_get_sibling_by_type (AstType type, AstNode *node);

AstNode* ast_get_child_at (int index, AstNode *parent);

/*----------------------------------------------------------------------------*/

AstNode* ast_create_program (AstNode *nodes);

AstNode* ast_create_vardecl (AstNode *type, char *id, AstNode *init);
AstNode* ast_create_type (AstType type);
AstNode* ast_create_print (char *id);

AstNode* ast_create_id (char *id);
AstNode* ast_create_assign (char *id, AstNode *expr);

AstNode* ast_create_intlit (char *value);
AstNode* ast_create_pointlit (char *x, char *y, char *z);
AstNode* ast_create_matrixlit (
  char *m11, char *m12, char *m13, char *m14,
  char *m21, char *m22, char *m23, char *m24,
  char *m31, char *m32, char *m33, char *m34,
  char *m41, char *m42, char *m43, char *m44
);

#endif//H_AST
