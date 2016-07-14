#include "hectorc.h"

#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "hectorc.tab.h"
#include "ast.h"
#include "semantics.h"
#include "translation.h"
#include "args.h"

#define GENERATED_FILENAME "program.c"

/*----------------------------------------------------------------------------*/

extern int yylex ();
extern int yyparse ();
extern FILE *yyin;

/*----------------------------------------------------------------------------*/

int hc_debug;
unsigned long hc_line, hc_column;
struct ast_node *program;
struct sym_tab *tab;
char *hc_input_file;
int has_lexical_errors;
int has_syntax_errors;
int has_semantic_errors;
int has_translation_errors;
int has_build_errors;

static FILE *hc_in, *hc_out;
static char *in_filename, *out_filename;

static void hc_lexical_analysis_only (void);
static void hc_syntatic_analysis (void);
static void hc_semantic_analysis (void);
static void hc_translate_program (void);
static void hc_build_executable (void);

static void vtab_printf (const char *fmt, va_list argp) {
  vfprintf(stdout, fmt, argp);
}

static void vtab_fprintf(FILE *out, const char *fmt, va_list argp) {
  vfprintf(out, fmt, argp);
}

static char* get_filename (const char* path) {
  int i, j, len, from, to;
  char *s;

  if (path == NULL) return NULL;
  len = strlen(path);
  if (len < 1) return NULL;

  for (i = len-1; i >= 0; i--) {
    if (path[i] == '/') {
      i++;
      break;
    }
  }
  from = i >= 0 ? i : 0;

  for (i = from; i < len; i++) {
    if (path[i] == '.') {
      i--;
      break;
    }
  }
  to = i >= 0 ? i : 0;

  s = (char*) malloc((to-from+1) * sizeof(char));
  if (s == NULL) {
    fprintf(stderr, "Failed to allocate memory!\n");
    return NULL;
  }

  for (j=from; j <= to; j++) {
    s[j-from] = path[j];
  }
  s[j] = '\0';

  return s;
}

static char* append_str (const char *s1, const char *s2) {
  int len1, len2, i;
  char *s;

  len1 = s1 == NULL ? 0 : strlen(s1);
  len2 = s2 == NULL ? 0 : strlen(s2);

  s = (char*) malloc((len1+len2) * sizeof(char));
  if (s == NULL) {
    fprintf(stderr, "Failed to allocate memory!\n");
    return NULL;
  }

  // clang's analyzer tool reports a warning for i=0..2. Don't ask me why.
  for (i=0; i < len1; i++) s[i] = s1[i];
  for (; i < len1+len2; i++) s[i] = s2[i-len1];

  return s;
}

/*----------------------------------------------------------------------------*/

void test (void) {
  exit(EXIT_SUCCESS);
}

int hc_init (int argc, char **argv) {
  int fd, f1, f2, f3, f4;

  //test();

  fd = contains_arg(argc, argv, "-d");
  f1 = contains_arg(argc, argv, "-1");
  f2 = contains_arg(argc, argv, "-2");
  f3 = contains_arg(argc, argv, "-3");
  f4 = contains_arg(argc, argv, "-4");

  hc_debug = fd;
  hc_in = NULL;
  hc_out = NULL;
  hc_line = 1;
  hc_column = 1;

  hc_input_file = get_file(argc, argv);
  if (hc_input_file != NULL) {
    hc_in = fopen(hc_input_file, "r");
    if (hc_in == NULL) {
      fprintf(stderr, "No such file: %s\n", hc_input_file);
      return EXIT_FAILURE;
    }
    if (fd) printf("Reading from file: %s\n", hc_input_file);
    yyin = hc_in;
  }

  has_lexical_errors = 0;
  has_syntax_errors = 0;
  has_semantic_errors = 0;
  has_translation_errors = 0;
  has_build_errors = 0;

  program = NULL;
  tab = NULL;

  if (f1) {
    hc_lexical_analysis_only();

  } else if (f2) {
    hc_syntatic_analysis();

  } else if (f3) {
    hc_syntatic_analysis();
    if (!has_lexical_errors && !has_syntax_errors) {
      hc_semantic_analysis();
    }
  } else if (f4) {
    hc_syntatic_analysis();
    if (!has_lexical_errors && !has_syntax_errors) {
      hc_semantic_analysis();
      if (!has_semantic_errors) {
        hc_translate_program();
      }
    }
  } else {
    hc_syntatic_analysis();
    if (!has_lexical_errors && !has_syntax_errors) {
      hc_semantic_analysis();
      if (!has_semantic_errors) {
        hc_translate_program();
        if (!has_translation_errors) {
          hc_build_executable();
        }
      }
    }
  }

  sym_free_tab(tab);
  ast_free(program);

  if (in_filename != NULL) free(in_filename);
  if (out_filename != NULL) free(out_filename);

  if (hc_in != NULL) {
    fclose(hc_in);
    hc_in = NULL;
  }

  if (has_lexical_errors ||
      has_syntax_errors ||
      has_semantic_errors ||
      has_translation_errors ||
      has_build_errors
  ) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void hc_lexical_analysis_only (void) {
  if (hc_debug) printf("Lexical analysis...\n");
  while (yylex());
  if (hc_debug && has_lexical_errors)
    printf("There are lexical errors.\n");
}

void hc_syntatic_analysis (void) {
  if (hc_debug) printf("Syntatic analysis...\n");
  yyparse();
  if (hc_debug && !has_lexical_errors && !has_syntax_errors)
    ast_print(program, 0);
  if (hc_debug && has_lexical_errors)
    printf("There are lexical errors.\n");
  if (hc_debug && has_syntax_errors)
    printf("There are syntatic errors.\n");
}

void hc_semantic_analysis (void) {
  if (hc_debug) printf("Semantic analysis...\n");
  tab = sym_create_tab("global", NULL);
  check_program(program);
  if (hc_debug)
    sym_print_global(tab);
  if (hc_debug && has_semantic_errors)
    printf("There are semantic errors.\n");
}

void hc_translate_program (void) {
  if (hc_debug) printf("Translating program to C...\n");

  // If no input file was specified, then we use a default name to the
  // output file.
  if (hc_input_file == NULL) {
    //TODO in_filenmae and out_filenmae are needed for building.
    hc_out = fopen(out_filename, "w");
    if (hc_out == NULL) {
      fprintf(stderr, "No such file: %s\n", GENERATED_FILENAME);
      return;
    }

  // The output file is named after the input file.
  } else {
    // in_filename is guaranteed to be not null.
    in_filename = get_filename(hc_input_file);
    out_filename = append_str(in_filename, ".c");
    hc_out = fopen(out_filename, "w");
    if (hc_out == NULL) {
      fprintf(stderr, "No such file: %s\n", out_filename);
      return;
    }
  }

  tr_program(hc_out, program);

  if (hc_out != NULL) {
    fclose(hc_out);
    hc_out = NULL;
  }

  if (hc_debug && has_translation_errors)
    printf("There are translation errors.\n");
}

void hc_build_executable (void) {
  pid_t pid;
  int status;

  if (hc_debug) printf("Building executable...\n");

  pid = fork();

  // Child process.
  if (pid == 0) {
    //TODO Do I have to clean something up?
    if (execlp("clang", "clang", "-Wall", "-o", in_filename, "lib.c", out_filename, (char*)0) == -1) {
      has_build_errors = 1;
      fprintf(stderr, "Failed to call the C compiler!\n");
      _exit(EXIT_FAILURE);
    }

  // Error.
  } else if (pid == -1) {
    has_build_errors = 1;
    fprintf(stderr, "Failed to fork!\n");

  // Parent process.
  } else {
    waitpid(pid, &status, 0);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
      has_build_errors = 1;
    }
  }

  if (hc_debug && has_build_errors)
    printf("There are build errors.\n");
}

/*----------------------------------------------------------------------------*/

void tprintf (u8 depth, const char *fmt, ...) {
  va_list argp;
  u8 i;
  for (i=0; i<depth; i++) printf("..");
  va_start(argp, fmt);
  vtab_printf(fmt, argp);
  va_end(argp);
}

void tfprintf (FILE *out, u8 depth, const char *fmt, ...) {
  va_list argp;
  u8 i;
  for (i=0; i<depth; i++) fprintf(out, "  ");
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
  if (errno == ERANGE || *endptr != '\0' || str == endptr) return 0;
  if (v < INT_MIN || v > INT_MAX) return 0;
  *value = (int) v;
  return 1;
}
