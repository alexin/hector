#ifndef H_TRANSLATION
#define H_TRANSLATION

#include "ast.h"

#include <stdio.h>

int tr_program (FILE *out, AstNode *program);

void tr_expr (FILE *out, AstNode *expr);

void tr_expr_neg (FILE *out, AstNode *neg);

void tr_expr_add (FILE *out, AstNode *add);
void tr_expr_mult (FILE *out, AstNode *mult);


#endif//H_TRANSLATION
