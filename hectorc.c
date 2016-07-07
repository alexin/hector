#include "hectorc.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#include "hectorc.tab.h"
#include "ast.h"
#include "semantics.h"
#include "symbols.h"
#include "translation.h"
#include "args.h"

/*----------------------------------------------------------------------------*/

extern int yylex ();
extern int yyparse ();

/*----------------------------------------------------------------------------*/

static void hc_lexical_analysis_only (int debug);
static void hc_syntatic_analysis (int debug);
static void hc_semantic_analysis (int debug);
static void hc_translation_phase (int debug);

static void vtab_printf (const char *fmt, va_list argp) {
  vfprintf(stdout, fmt, argp);
}

static void vtab_fprintf(FILE *out, const char *fmt, va_list argp) {
  vfprintf(out, fmt, argp);
}

/*----------------------------------------------------------------------------*/

int hc_init (int argc, char **argv) {
  int fd, f1, f2, f3;

  fd = contains_arg(argc, argv, "-d");
  f1 = contains_arg(argc, argv, "-1");
  f2 = contains_arg(argc, argv, "-2");
  f3 = contains_arg(argc, argv, "-3");

  has_lexical_errors = 0;
  has_syntax_errors = 0;
  has_semantic_errors = 0;
  has_translation_errors = 0;

  program = NULL;
  tab = NULL;

  if(f1) {
    hc_lexical_analysis_only(fd);

  } else if(f2) {
    hc_syntatic_analysis(fd);

  } else if(f3) {
    hc_syntatic_analysis(fd);
    if(!has_lexical_errors && !has_syntax_errors) {
      hc_semantic_analysis(fd);
    }
  } else {
    hc_syntatic_analysis(fd);
    if(!has_lexical_errors && !has_syntax_errors) {
      hc_semantic_analysis(fd);
      if(!has_semantic_errors) {
        hc_translation_phase(fd);
      }
    }
  }

  sym_free_tab(tab);
  ast_free(program);

  if (has_lexical_errors ||
      has_syntax_errors ||
      has_semantic_errors ||
      has_translation_errors
  ) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void hc_lexical_analysis_only (const int debug) {
  if (debug) printf("Lexical analysis...\n");
  hc_debug = debug;
  hc_line = 1;
  hc_column = 1;
  while (yylex());
  if (debug && has_lexical_errors)
    printf("There are lexical errors.\n");
}

void hc_syntatic_analysis (const int debug) {
  if (debug) printf("Syntatic analysis...\n");
  hc_debug = debug;
  hc_line = 1;
  hc_column = 1;
  yyparse();
  if (debug && !has_lexical_errors && !has_syntax_errors)
    ast_print(program, 0);
  if (debug && has_lexical_errors)
    printf("There are lexical errors.\n");
  if (debug && has_syntax_errors)
    printf("There are syntatic errors.\n");
}

void hc_semantic_analysis (const int debug) {
  if (debug) printf("Semantic analysis...\n");
  hc_debug = debug;
  tab = sym_create_tab("global", NULL);
  check_program(program);
  if (debug)
    sym_print_global(tab);
  if (debug && has_semantic_errors)
    printf("There are semantic errors.\n");
}

void hc_translation_phase (const int debug) {
  if (debug) printf("Translation phase...\n");
  hc_debug = debug;
  tr_program(stdout, program);
  if (debug && has_translation_errors)
    printf("There are translation errors.\n");
}

/*----------------------------------------------------------------------------*/

void tprintf (u8 depth, const char *fmt, ...) {
  va_list argp;
  u8 i;
  for(i=0; i<depth; i++) printf("..");
  va_start(argp, fmt);
  vtab_printf(fmt, argp);
  va_end(argp);
}

void tfprintf (FILE *out, u8 depth, const char *fmt, ...) {
  va_list argp;
  u8 i;
  for(i=0; i<depth; i++) printf("  ");
  va_start(argp, fmt);
  vtab_fprintf(out, fmt, argp);
  va_end(argp);
}

/*----------------------------------------------------------------------------*/

//TODO Test this function.
// Returns 1 if an integer was parsed, 0 otherwise.
int parse_int (const char *str, int *value) {
  char *endptr;
  long v;
  errno = 0;
  v = strtol(str, &endptr, 10);
  if(errno == ERANGE || *endptr != '\0' || str == endptr) return 0;
  if (v < INT_MIN || v > INT_MAX) return 0;
  *value = (int) v;
  return 1;
}

/*int
parse_float(
  const char *str,
  float *value
) {
  char *endptr;
  float v;
  errno = 0;
  v = strtof(str, &endptr);
  if(errno == ERANGE || *endptr != '\0' || str == endptr) return 0;
  if (v == HUGE_VALF || v == -HUGE_VALF) return 0;
  *value = (float) v;
  return 1;
}*/
