return()

add_library(asm
    asm.lex.cpp
    asm.yacc.cpp
    sym_table.cpp
    tables.cpp
    codes.h
    lexer.h
    phase2.h
    stack.h
    sym_table.h
    tables.h
    y.tab.h
)
