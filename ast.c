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
  "ADD", "ASSIGN", "ID", "INT", "INTLIT", "MATRIX", "MATRIXLIT", "MULT", "NEG",
  "POINT", "POINTLIT", "PRINT", "PROGRAM", "VARDECL", "VECTOR"
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

AstNode* ast_create_pointlit (char *x, char *y, char *z) {
  AstNode *node, *nx, *ny, *nz;

  IFNULL(x) IFNULL(y) IFNULL(z)

  node = ast_create_node(ast_POINTLIT);
  if (node == NULL) return NULL;

  nx = ast_create_intlit(x);
  if (nx == NULL) {
    free(node);
    return NULL;
  }

  ny = ast_create_intlit(y);
  if (ny == NULL) {
    free(nx);
    free(node);
    return NULL;
  }

  nz = ast_create_intlit(z);
  if (nz == NULL) {
    free(ny);
    free(nx);
    free(node);
    return NULL;
  }

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

  IFNULL(m11) IFNULL(m12) IFNULL(m13) IFNULL(m14)
  IFNULL(m21) IFNULL(m22) IFNULL(m23) IFNULL(m24)
  IFNULL(m31) IFNULL(m32) IFNULL(m33) IFNULL(m34)
  IFNULL(m41) IFNULL(m42) IFNULL(m43) IFNULL(m44)

  node = ast_create_node(ast_MATRIXLIT);
  if (node == NULL) return NULL;

  // ROW 1
  n11 = ast_create_intlit(m11);
  if (n11 == NULL) {
    free(node);
    return NULL;
  }
  n12 = ast_create_intlit(m12);
  if (n12 == NULL) {
    free(n11);
    free(node);
    return NULL;
  }
  n13 = ast_create_intlit(m13);
  if (n13 == NULL) {
    free(n12); free(n11);
    free(node);
    return NULL;
  }
  n14 = ast_create_intlit(m14);
  if (n14 == NULL) {
    free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }

  // ROW 2
  n21 = ast_create_intlit(m21);
  if (n21 == NULL) {
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n22 = ast_create_intlit(m22);
  if (n22 == NULL) {
                                     free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n23 = ast_create_intlit(m23);
  if (n23 == NULL) {
                          free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n24 = ast_create_intlit(m24);
  if (n24 == NULL) {
               free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }

  // ROW 3
  n31 = ast_create_intlit(m31);
  if (n31 == NULL) {
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n32 = ast_create_intlit(m32);
  if (n32 == NULL) {
                                     free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n33 = ast_create_intlit(m33);
  if (n33 == NULL) {
                          free(n32); free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n34 = ast_create_intlit(m34);
  if (n34 == NULL) {
               free(n33); free(n32); free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }

  // ROW 4
  n41 = ast_create_intlit(m41);
  if (n41 == NULL) {
    free(n34); free(n33); free(n32); free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n42 = ast_create_intlit(m42);
  if (n42 == NULL) {
                                     free(n41);
    free(n34); free(n33); free(n32); free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n43 = ast_create_intlit(m43);
  if (n43 == NULL) {
                          free(n42); free(n41);
    free(n34); free(n33); free(n32); free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }
  n44 = ast_create_intlit(m44);
  if (n44 == NULL) {
               free(n43); free(n42); free(n41);
    free(n34); free(n33); free(n32); free(n31);
    free(n24); free(n23); free(n22); free(n21);
    free(n14); free(n13); free(n12); free(n11);
    free(node);
    return NULL;
  }

  //VALUE(n11,m11) VALUE(n12,m12) VALUE(n13,m13) VALUE(n14,m14)
  //VALUE(n21,m21) VALUE(n22,m22) VALUE(n23,m23) VALUE(n24,m24)
  //VALUE(n31,m31) VALUE(n32,m32) VALUE(n33,m33) VALUE(n34,m34)
  //VALUE(n41,m41) VALUE(n42,m42) VALUE(n43,m43) VALUE(n44,m44)

  SIBLING(n11,n12) SIBLING(n12,n13) SIBLING(n13,n14) SIBLING(n14,n21)
  SIBLING(n21,n22) SIBLING(n22,n23) SIBLING(n23,n24) SIBLING(n24,n31)
  SIBLING(n31,n32) SIBLING(n32,n33) SIBLING(n33,n34) SIBLING(n34,n41)
  SIBLING(n41,n42) SIBLING(n42,n43) SIBLING(n43,n44)

  node->child = n11;

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
