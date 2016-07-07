#include "ast.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hectorc.h"

#define MALLOC(TYPE,SIZE) ((TYPE*)malloc((SIZE)*sizeof(TYPE)))

/*----------------------------------------------------------------------------*/

static const char *ast_type_str[] = {
  "ID", "INTLIT", "POINT", "POINTLIT", "PRINT", "PROGRAM", "VARDECL"
};

/*----------------------------------------------------------------------------*/

static AstNode* ast_create_node (const AstType type) {
  AstNode *node;
  node = MALLOC(struct ast_node, 1);
  if (node == NULL) return NULL;
  node->type = type;
  node->sibling = NULL;
  node->child = NULL;
  node->value = NULL;
  return node;
}

static void ast_destroy_node (AstNode *node) {
  if(node == NULL) return;
  ast_destroy_node(node->sibling);
  ast_destroy_node(node->child);
  free((void*)(node->value));
  free((void*)node);
}

static AstNode* ast_get_last_sibling (AstNode *node) {
  AstNode *last;
  if (node == NULL) return NULL;
  last = node;
  while (last->sibling != NULL) last = last->sibling;
  return last;
}

/*----------------------------------------------------------------------------*/

const char* get_ast_type_str (const AstType type) {
  return ast_type_str[type];
}

void ast_print (AstNode *node, const unsigned int depth) {
  if (node == NULL) return;

  switch (node->type) {
    case ast_ID: tprintf(depth, "Id(%s)\n", ((char*)node->value)); break;
    case ast_INTLIT:
      tprintf(depth, "IntLit(%s)\n", ((char*)node->value));
      break;
    case ast_POINTLIT: tprintf(depth, "PointLit\n"); break;
    case ast_PRINT: tprintf(depth, "Print\n"); break;
    case ast_PROGRAM: tprintf(depth, "Program\n"); break;
    case ast_VARDECL: tprintf(depth, "VarDecl\n"); break;

    default:
      tprintf(
        depth,
        "unknown AST node type: %s\n",
        get_ast_type_str(node->type)
      );
      return;
  }

  ast_print(node->child, depth+1);
  ast_print(node->sibling, depth);
}

AstNode* ast_add_sibling (AstNode *node, AstNode *sibling) {
  AstNode *iter;
  if (sibling == NULL) return node;
  iter = node;
  while (iter->sibling != NULL) iter = iter->sibling;
  iter->sibling = sibling;
  return node;
}

unsigned int ast_count_siblings (AstNode *node) {
  AstNode *iter;
  unsigned int count;
  iter = node;
  count = 0;
  while (iter != NULL) {
    count++;
    iter = iter->sibling;
  }
  return count;
}

void ast_free (AstNode *ast) {
  ast_destroy_node(ast);
}

void ast_set_location (AstNode *node, int line, int column) {
  if (node == NULL) return;
  node->line = line;
  node->column = column;
}

AstNode* ast_get_sibling_by_type (AstType type, AstNode *node) {
  AstNode *it;
  it = node;
  while (it != NULL) {
    if (it->type == type) return it;
    it = it->sibling;
  }
  return NULL;
}

/*----------------------------------------------------------------------------*/

AstNode* ast_create_program (AstNode *nodes) {
  AstNode *node;
  if (nodes == NULL) return NULL;
  node = ast_create_node(ast_PROGRAM);
  if (node == NULL) return NULL;
  node->child = nodes;
  return node;
}

AstNode* ast_create_vardecl (char *id, AstNode *pointlit) {
  AstNode *node, *nid;

  node = ast_create_node(ast_VARDECL);
  if (node == NULL) return NULL;

  nid = ast_create_id(id);
  if (nid == NULL) {
    free(node);
    return NULL;
  }

  nid->sibling = pointlit;
  node->child = nid;

  return node;
}

AstNode* ast_create_id (char *id) {
  AstNode *node;
  node = ast_create_node(ast_ID);
  if (node == NULL) return NULL;
  node->value = (void*) id;
  return node;
}

AstNode* ast_create_intlit (char *value) {
  AstNode *node;
  node = ast_create_node(ast_INTLIT);
  if (node == NULL) return NULL;
  node->value = (void*) value;
  return node;
}

AstNode* ast_create_pointlit (char *x, char *y, char *z) {
  AstNode *node, *nx, *ny, *nz;

  node = ast_create_node(ast_POINTLIT);
  if (node == NULL) return NULL;

  nx = ast_create_node(ast_INTLIT);
  if (nx == NULL) {
    free(node);
    return NULL;
  }
  nx->value = (void*) x;

  ny = ast_create_node(ast_INTLIT);
  if (ny == NULL) {
    free(nx);
    free(node);
    return NULL;
  }
  ny->value = (void*) y;

  nz = ast_create_node(ast_INTLIT);
  if (nz == NULL) {
    free(ny);
    free(nx);
    free(node);
    return NULL;
  }
  nz->value = (void*) z;

  ny->sibling = nz;
  nx->sibling = ny;
  node->child = nx;

  return node;
}

AstNode* ast_create_print (char *id) {
  AstNode *node, *nid;

  node = ast_create_node(ast_PRINT);
  if (node == NULL) return NULL;

  nid = ast_create_id(id);
  if (nid == NULL) {
    free(node);
    return NULL;
  }

  node->child = nid;

  return node;
}
