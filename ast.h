#ifndef H_AST
#define H_AST

/*----------------------------------------------------------------------------*/

enum ast_type {
  ast_PROGRAM,
  ast_MINUS, ast_PLUS,
  ast_ADD, ast_SUB, ast_MUL, ast_DIV, ast_POW,
  ast_INTLIT, ast_FLOATLIT
};

struct ast_node {
  enum ast_type type;
  struct ast_node *sibling;
  struct ast_node *child;
  void *value;
  int flag; /* Multipurpose flag. */
};

/*----------------------------------------------------------------------------*/

const char*
get_ast_type_str(
  const enum ast_type type
);

int
ast_is_unary_expression(
  const struct ast_node *node
);

int
ast_is_binary_expression(
  const struct ast_node *node
);

void
ast_print(
  struct ast_node *node,
  const unsigned int depth
);

/* Appends a sibling to end of the list. */
/* Returns the node itself. */
struct ast_node*
ast_add_sibling(
  struct ast_node *node,
  struct ast_node *sibling /* NULLABLE, LIST */
);

struct ast_node*
ast_set_flag(
  const int flag,
  struct ast_node *node /* NULLABLE, LIST */
);

unsigned int
ast_count_siblings(
  struct ast_node *node /* NULLABLE, LIST */
);

void
ast_free(
  struct ast_node *ast
);

/*----------------------------------------------------------------------------*/

struct ast_node*
ast_create_program(
  struct ast_node *expr
);

/*----------------------------------------------------------------------------*/

struct ast_node* ast_create_intlit(char *value);
struct ast_node* ast_create_floatlit(char *value);

/*----------------------------------------------------------------------------*/

struct ast_node*
ast_create_unary(
  const enum ast_type type,
  struct ast_node *expr
);

struct ast_node*
ast_create_binary(
  const enum ast_type type,
  struct ast_node *lhs,
  struct ast_node *rhs
);

#endif//H_AST
