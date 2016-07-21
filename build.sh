
PROGRAM="hectorc"
STATIC="static"
TESTS="tests"
VALGRIND_TEST="valgrind.hc"

source cmdarg.sh
cmdarg_purge
cmdarg 'c' 'clean'
cmdarg 'a' 'analyze'
cmdarg 'v' 'valgrind'
cmdarg 'z' 'zip'
cmdarg_parse "$@"

if [ ${cmdarg_cfg['clean']} ]; then
  # Flex
  rm lex.yy.c
  # Bison
  rm ${PROGRAM}.tab.h
  rm ${PROGRAM}.tab.c
  rm ${PROGRAM}.output
  # Program
  rm ${PROGRAM}
  rm -r ${PROGRAM}.dSYM
  rm ${PROGRAM}.zip
  exit
fi

OK=0

# Flex
lex ${PROGRAM}.l
OK="$?"
if [ ! "$OK" = "0" ]; then
  exit
fi

# Bison
bison -d -v ${PROGRAM}.y
OK="$?"
if [ ! "$OK" = "0" ]; then
  exit
fi

# clang-analyzer
if [ ${cmdarg_cfg['analyze']} ]; then
  hash scan-build 2>/dev/null || { echo >&2 "clang-analyzer not installed!"; exit 1; }
  scan-build -o ${STATIC} -V clang -g -O0 -Wall -Wno-unused-function args.c ast.c hectorc.c hectorc.tab.c lex.yy.c symbols.c semantics.c sem_unary_ops.c sem_binary_ops.c translation.c tr_unary_ops.c tr_binary_ops.c
  OK="$?"
  rm a.out
  rm -r a.out.dSYM
  rm -r ${STATIC}
  exit
fi

# Valgrind
if [ ${cmdarg_cfg['valgrind']} ]; then
  hash valgrind 2>/dev/null || { echo >&2 "Valgrind not installed!"; exit 1; }
  clang -g -O0 -Wall -Wno-unused-function args.c ast.c hectorc.c hectorc.tab.c lex.yy.c symbols.c semantics.c sem_unary_ops.c sem_binary_ops.c translation.c tr_unary_ops.c tr_binary_ops.c -o ${PROGRAM}
  echo "${VALGRIND_TEST}"
  valgrind --leak-check=yes ./${PROGRAM} -d ${VALGRIND_TEST}
  rm ${PROGRAM}
  rm -r ${PROGRAM}.dSYM
  exit
fi

# Program
clang -g -Wall -Wno-unused-function args.c ast.c hectorc.c hectorc.tab.c lex.yy.c symbols.c semantics.c sem_unary_ops.c sem_binary_ops.c translation.c tr_unary_ops.c tr_binary_ops.c -o ${PROGRAM}
OK="$?"
if [ ! "$OK" = "0" ]; then
  exit
fi

# ZIP
if [ ${cmdarg_cfg['zip']} ]; then
  zip -r ${PROGRAM}.zip ${PROGRAM}.l ${PROGRAM}.y ${PROGRAM}.c ${PROGRAM}.h args.h args.c ast.h ast.c semantics.h semantics.c
fi
