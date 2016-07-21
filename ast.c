#include "ast.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MALLOC(TYPE,SIZE) ((TYPE*)malloc((SIZE)*sizeof(TYPE)))

#define VALUE(N,V) (N)->value = (void*)(V);
#define SIBLING(A,B) (A)->sibling = (B);

#define IFNULL(E) if ((E) == NULL) return NULL;

/*----------------------------------------------------------------------------*/

static const char *ast_type_str[] = {
  "ADD", "ASSIGN", "AT", "ID", "INT", "INTLIT", "MATRIX", "MATRIXLIT", "MULT",
  "NEG", "POINT", "POINTLIT", "PRINT", "PROGRAM", "VARDECL", "VECTOR"
};

const char* ast_type_to_str (AstType type) {
  return ast_type_str[type];
}

static AstNode* ast_create_node (const AstType type) {
  AstNode *node;
  node = MALLOC(struct ast_node, 1);
  if (node == NULL) return NULL;
  node->type = type;
  node->sibling = NULL;
  node->child = NULL;
  node->value = NULL;
  node->info = NULL;
  return node;
}

static void ast_destroy_node (AstNode *node) {
  if(node == NULL) return;
  ast_destroy_node(node->sibling); node->sibling = NULL;
  ast_destroy_node(node->child); node->child = NULL;
  sem_free(node->info); node->info = NULL;
  free((void*)(node->value)); node->value = NULL;
  free((void*)node);
}

static AstNode* ast_get_last_sibling (AstNode *node) {
  AstNode *last;
  if (node == NULL) return NULL;
  last = node;
  while (last->sibling != NULL) last = last->sibling;
  return last;
}

static void ast_print_annotations (const AstNode *node) {
  if (node->info != NULL) {
    fprintf(stdout, " - %s", sem_type_to_str(node->info->type));
    if (node->info->type != sem_UNDEF) {
      if (node->info->is_lvalue) fprintf(stdout, " - Lvalue");
      else fprintf(stdout, " - Rvalue");
    }
  }
  fprintf(stdout, "\n");
}

void ast_print (AstNode *node, unsigned int depth) {
  if (node == NULL) return;

  switch (node->type) {
    case ast_ADD:
      tprintf(depth, "Add");
      ast_print_annotations(node);
      break;
    case ast_ASSIGN:
      tprintf(depth, "Assign");
      ast_print_annotations(node);
      break;
    case ast_AT:
      tprintf(depth, "At");
      ast_print_annotations(node);
      break;
    case ast_ID:
      tprintf(depth, "Id(%s)", ((char*)node->value));
      ast_print_annotations(node);
      break;
    case ast_INT:
      tprintf(depth, "Int");
      ast_print_annotations(node);
      break;
    case ast_INTLIT:
      tprintf(depth, "IntLit(%s)", ((char*)node->value));
      ast_print_annotations(node);
      break;
    case ast_MATRIX:
      tprintf(depth, "Matrix");
      ast_print_annotations(node);
      break;
    case ast_MATRIXLIT:
      tprintf(depth, "MatrixLit");
      ast_print_annotations(node);
      break;
    case ast_MULT:
      tprintf(depth, "Mult");
      ast_print_annotations(node);
      break;
    case ast_NEG:
      tprintf(depth, "Neg");
      ast_print_annotations(node);
      break;
    case ast_POINT:
      tprintf(depth, "Point");
      ast_print_annotations(node);
      break;
    case ast_POINTLIT:
      tprintf(depth, "PointLit");
      ast_print_annotations(node);
      break;
    case ast_PRINT:
      tprintf(depth, "Print");
      ast_print_annotations(node);
      break;
    case ast_PROGRAM:
      tprintf(depth, "Program");
      ast_print_annotations(node);
      break;
    case ast_VARDECL:
      tprintf(depth, "VarDecl");
      ast_print_annotations(node);
      break;
    case ast_VECTOR:
      tprintf(depth, "Vector");
      ast_print_annotations(node);
      break;

    default:
      tprintf(
        depth,
        "unknown AST node type (%d): %s\n",
        __LINE__, ast_type_to_str(node->type)
      );
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

AstNode* ast_get_child_at (int index, AstNode *parent) {
  AstNode *it;
  int i;
  for (i=0, it=parent->child; i < index; i++) {
    if (it == NULL) return NULL;
    it = it->sibling;
  }
  return it;
}

/*----------------------------------------------------------------------------*/

AstNode* ast_create_program (AstNode *nodes) {
  AstNode *node;
  IFNULL(nodes)
  node = ast_create_node(ast_PROGRAM);
  if (node == NULL) return NULL;
  node->child = nodes;
  return node;
}

AstNode* ast_create_vardecl (AstNode *type, char *id, AstNode *init) {
  AstNode *node, *nid;

  IFNULL(id)

  node = ast_create_node(ast_VARDECL);
  if (node == NULL) return NULL;

  nid = ast_create_id(id);
  if (nid == NULL) {
    free(node);
    return NULL;
  }

  SIBLING(nid, init)
  SIBLING(type, nid)
  node->child = type;

  return node;
}

AstNode* ast_create_type (AstType type) {
  AstNode *node;
  if (
    type != ast_INT &&
    type != ast_POINT &&
    type != ast_MATRIX &&
    type != ast_VECTOR
  ) return NULL;
  node = ast_create_node(type);
  if (node == NULL) return NULL;
  return node;
}

AstNode* ast_create_id (char *id) {
  AstNode *node;
  IFNULL(id)
  node = ast_create_node(ast_ID);
  if (node == NULL) return NULL;
  node->value = (void*) id;
  return node;
}

AstNode* ast_create_intlit (char *value) {
  AstNode *node;
  IFNULL(value)
  node = ast_create_node(ast_INTLIT);
  if (node == NULL) return NULL;
  node->value = (void*) value;
  return node;
}

AstNode* ast_create_pointlit (AstNode *comps) {
  AstNode *node;

  IFNULL(comps)
  if (ast_count_siblings(comps) != 3) return NULL;

  node = ast_create_node(ast_POINTLIT);
  if (node == NULL) return NULL;

  node->child = comps;
  return node;
}

AstNode* ast_create_matrixlit (AstNode *comps) {
  AstNode *node;

  IFNULL(comps)
  if (ast_count_siblings(comps) != 16) return NULL;

  node = ast_create_node(ast_MATRIXLIT);
  if (node == NULL) return NULL;

  node->child = comps;

  return node;
}

AstNode* ast_create_print (AstNode *expr) {
  AstNode *node;
  if (expr == NULL) return NULL;
  node = ast_create_node(ast_PRINT);
  if (node == NULL) return NULL;
  node->child = expr;
  return node;
}

AstNode* ast_create_assign (char *id, AstNode *expr) {
  AstNode *node, *nid;

  IFNULL(id)
  IFNULL(expr)

  node = ast_create_node(ast_ASSIGN); IFNULL(node)

  nid = ast_create_id(id);
  if (nid == NULL) {
    free(node);
    return NULL;
  }

  nid->sibling = expr;
  node->child = nid;

  return node;
}

AstNode* ast_create_binary (AstType op, AstNode *lhs, AstNode *rhs) {
  AstNode *node;

  if (
    op != ast_ADD &&
    op != ast_MULT
  ) return NULL;

  IFNULL(lhs)
  IFNULL(rhs)

  node = ast_create_node(op); IFNULL(node)

  lhs->sibling = rhs;
  node->child = lhs;

  return node;
}

AstNode* ast_create_unary (AstType op, AstNode *expr) {
  AstNode *node;
  if (
    op != ast_NEG
  ) return NULL;
  IFNULL(expr)
  node = ast_create_node(op); IFNULL(node)
  node->child = expr;
  return node;
}

AstNode* ast_create_at (char *id, AstNode *target) {
  AstNode *node, *nid;

  IFNULL(id)
  IFNULL(target)

  node = ast_create_node(ast_AT); IFNULL(node)

  nid = ast_create_id(id);
  if (nid == NULL) {
    free(node);
    return NULL;
  }

  nid->sibling = target;
  node->child = nid;

  return node;
}
