#include "ast.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hectorc.h"

#define MALLOC(TYPE,SIZE) ((TYPE*)malloc((SIZE)*sizeof(TYPE)))

/*----------------------------------------------------------------------------*/

static const char *ast_type_str[] = {
  "PROGRAM",
  "MINUS",
  "ADD", "SUB", "MUL", "DIV", "POW",
  "INTLIT"
};

/*----------------------------------------------------------------------------*/

static
struct ast_node*
ast_create_node(
  const enum ast_type type
) {
  struct ast_node *node;
  node = MALLOC(struct ast_node, 1);
  if(node == NULL) return NULL;
  node->type = type;
  node->sibling = NULL;
  node->child = NULL;
  node->value = NULL;
  return node;
}

static
void
ast_destroy_node(
  struct ast_node *node
) {
  if(node == NULL) return;
  ast_destroy_node(node->sibling);
  ast_destroy_node(node->child);
  free((void*)(node->value));
  free((void*)node);
}

static
struct ast_node*
get_last_sibling(
  struct ast_node *node
) {
  struct ast_node *last;
  if(node == NULL) return NULL;
  last = node;
  while(last->sibling != NULL) last = last->sibling;
  return last;
}

static
int
is_ast_type_unary(
  const enum ast_type type
) {
  return type == ast_MINUS;
}

static
int
is_ast_type_binary(
  const enum ast_type type
) {
  return type == ast_ADD || type == ast_SUB ||
         type == ast_MUL || type == ast_DIV ||
         type == ast_POW;
}

/*----------------------------------------------------------------------------*/

const char*
get_ast_type_str(
  const enum ast_type type
) {
  return ast_type_str[type];
}

int
ast_is_unary_expression(
  const struct ast_node *node
) {
  return is_ast_type_unary(node->type);
}

int
ast_is_binary_expression(
  const struct ast_node *node
) {
  return is_ast_type_binary(node->type);
}

void
ast_print(
  struct ast_node *node,
  const unsigned int depth
) {
  if(node == NULL) return;

  switch(node->type) {
    case ast_PROGRAM: tab_printf(depth, "Program\n"); break;
    case ast_ADD: tab_printf(depth, "Add\n"); break;
    case ast_SUB: tab_printf(depth, "Sub\n"); break;
    case ast_MUL: tab_printf(depth, "Mul\n"); break;
    case ast_DIV: tab_printf(depth, "Div\n"); break;
    case ast_POW: tab_printf(depth, "Pow\n"); break;
    case ast_MINUS: tab_printf(depth, "Minus\n"); break;
    case ast_INTLIT:
      tab_printf(depth, "IntLit(%s)\n", ((char*)node->value));
      break;
    default:
      tab_printf(
        depth,
        "unknown AST node type: %s\n",
        get_ast_type_str(node->type)
      );
      return;
  }

  ast_print(node->child, depth+1);
  ast_print(node->sibling, depth);
}

struct ast_node*
ast_add_sibling(
  struct ast_node *node,
  struct ast_node *sibling /* NULLABLE */
) {
  struct ast_node *iter;
  if(sibling == NULL) return node;
  iter = node;
  while(iter->sibling != NULL) iter = iter->sibling;
  iter->sibling = sibling;
  return node;
}

struct ast_node*
ast_set_flag(
  const int flag,
  struct ast_node *node /* NULLABLE, LIST */
) {
  struct ast_node *iter;
  iter = node;
  while(iter != NULL) {
    iter->flag = flag;
    iter = iter->sibling;
  }
  return node;
}

unsigned int
ast_count_siblings(
  struct ast_node *node /* NULLABLE, LIST */
) {
  struct ast_node *iter;
  unsigned int count;
  iter = node;
  count = 0;
  while(iter != NULL) {
    count++;
    iter = iter->sibling;
  }
  return count;
}

void
ast_free(
  struct ast_node *ast
) {
  ast_destroy_node(ast);
}

/*----------------------------------------------------------------------------*/

struct ast_node*
ast_create_program(
  struct ast_node *expr
) {
  struct ast_node *node;
  if(expr == NULL) return NULL;
  node = ast_create_node(ast_PROGRAM);
  if(node == NULL) return NULL;
  node->child = expr;
  return node;
}

/*----------------------------------------------------------------------------*/

struct ast_node*
ast_create_intlit(
  char *value
) {
  struct ast_node *node;
  node = ast_create_node(ast_INTLIT);
  if(node == NULL) return NULL;
  node->value = (void*) value;
  return node;
}

/*----------------------------------------------------------------------------*/

struct ast_node*
ast_create_unary(
  const enum ast_type type,
  struct ast_node *expr
) {
  struct ast_node *node;

  if(!is_ast_type_unary(type)) return NULL;

  if(expr == NULL) return NULL;

  node = ast_create_node(type);
  if(node == NULL) return NULL;

  node->child = expr;

  return node;
}

struct ast_node*
ast_create_binary(
  const enum ast_type type,
  struct ast_node *lhs,
  struct ast_node *rhs
) {
  struct ast_node *node;
  if(!is_ast_type_binary(type)) return NULL;
  if(lhs == NULL) return NULL;
  if(rhs == NULL) return NULL;
  node = ast_create_node(type);
  if(node == NULL) return NULL;
  lhs->sibling = rhs;
  node->child = lhs;
  return node;
}
