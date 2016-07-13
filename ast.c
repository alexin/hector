#include "ast.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hectorc.h"

#define MALLOC(TYPE,SIZE) ((TYPE*)malloc((SIZE)*sizeof(TYPE)))

#define VALUE(N,V) (N)->value = (void*)(V);
#define SIBLING(A,B) (A)->sibling = (B);

/*----------------------------------------------------------------------------*/

static const char *ast_type_str[] = {
  "ASSIGN", "ID", "INTLIT", "MATRIXLIT", "POINT", "POINTLIT", "PRINT", "PROGRAM", "VARDECL"
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
    case ast_ASSIGN: tprintf(depth, "Assign\n"); break;
    case ast_ID: tprintf(depth, "Id(%s)\n", ((char*)node->value)); break;
    case ast_INTLIT:
      tprintf(depth, "IntLit(%s)\n", ((char*)node->value));
      break;
    case ast_MATRIXLIT: tprintf(depth, "MatrixLit\n"); break;
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

AstNode* ast_create_matrixlit (
  char *m11, char *m12, char *m13, char *m14,
  char *m21, char *m22, char *m23, char *m24,
  char *m31, char *m32, char *m33, char *m34,
  char *m41, char *m42, char *m43, char *m44
) {
  AstNode *node, *n11, *n12, *n13, *n14,
                 *n21, *n22, *n23, *n24,
                 *n31, *n32, *n33, *n34,
                 *n41, *n42, *n43, *n44;

  node = ast_create_node(ast_MATRIXLIT);
  if (node == NULL) return NULL;

  // ROW 1
  n11 = ast_create_node(ast_INTLIT);
  if (n11 == NULL) {
    free(node);
    return NULL;
  }
  n12 = ast_create_node(ast_INTLIT);
  if (n12 == NULL) {
    free(n11);
    free(node);
    return NULL;
  }
  n13 = ast_create_node(ast_INTLIT);
  if (n13 == NULL) {
    free(n12); free(n11);
    free(node);
    return NULL;
  }
  n14 = ast_create_node(ast_INTLIT);
  if (n14 == NULL) {
    free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }

  // ROW 2
  n21 = ast_create_node(ast_INTLIT);
  if (n21 == NULL) {
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n22 = ast_create_node(ast_INTLIT);
  if (n22 == NULL) {
                                     free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n23 = ast_create_node(ast_INTLIT);
  if (n23 == NULL) {
                          free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n24 = ast_create_node(ast_INTLIT);
  if (n24 == NULL) {
               free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }

  // ROW 3
  n31 = ast_create_node(ast_INTLIT);
  if (n31 == NULL) {
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n32 = ast_create_node(ast_INTLIT);
  if (n32 == NULL) {
                                     free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n33 = ast_create_node(ast_INTLIT);
  if (n33 == NULL) {
                          free(n32); free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n34 = ast_create_node(ast_INTLIT);
  if (n34 == NULL) {
               free(n33); free(n32); free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }

  // ROW 4
  n41 = ast_create_node(ast_INTLIT);
  if (n41 == NULL) {
    free(n34); free(n33); free(n32); free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n42 = ast_create_node(ast_INTLIT);
  if (n42 == NULL) {
                                     free(n41);
    free(n34); free(n33); free(n32); free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n43 = ast_create_node(ast_INTLIT);
  if (n43 == NULL) {
                          free(n42); free(n41);
    free(n34); free(n33); free(n32); free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n44 = ast_create_node(ast_INTLIT);
  if (n44 == NULL) {
               free(n43); free(n42); free(n41);
    free(n34); free(n33); free(n32); free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }

  VALUE(n11,m11) VALUE(n12,m12) VALUE(n13,m13) VALUE(n14,m14)
  VALUE(n21,m21) VALUE(n22,m22) VALUE(n23,m23) VALUE(n24,m24)
  VALUE(n31,m31) VALUE(n32,m32) VALUE(n33,m33) VALUE(n34,m34)
  VALUE(n41,m41) VALUE(n42,m42) VALUE(n43,m43) VALUE(n44,m44)

  SIBLING(n11,n12) SIBLING(n12,n13) SIBLING(n13,n14) SIBLING(n14,n21)
  SIBLING(n21,n22) SIBLING(n22,n23) SIBLING(n23,n24) SIBLING(n24,n31)
  SIBLING(n31,n32) SIBLING(n32,n33) SIBLING(n33,n34) SIBLING(n34,n41)
  SIBLING(n41,n42) SIBLING(n42,n43) SIBLING(n43,n44)

  node->child = n11;

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

AstNode* ast_create_assign (char *id, AstNode *expr) {
  AstNode *node, *nid;

  if (expr == NULL) return NULL;

  node = ast_create_node(ast_ASSIGN);
  if (node == NULL) return NULL;

  nid = ast_create_id(id);
  if (nid == NULL) {
    free(node);
    return NULL;
  }

  nid->sibling = expr;
  node->child = nid;

  return node;
}
