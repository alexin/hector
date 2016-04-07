#ifndef H_TRANSLATION
#define H_TRANSLATION

#include "ast.h"

#include <stdio.h>

void
tr_program(
  FILE *out,
  struct ast_node *program
);

#endif//H_TRANSLATION
