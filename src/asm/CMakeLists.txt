find_package(BISON)
find_package(FLEX)

if(NOT (BISON_FOUND AND FLEX_FOUND))
    message(WARNING "Bison and flex required to build the assember; skipping")
    return()
endif()

flex_target(asm asm.l ${CMAKE_CURRENT_BINARY_DIR}/asm.lex.cpp)
bison_target(asm asm.y ${CMAKE_CURRENT_BINARY_DIR}/asm.yacc.cpp)
add_executable(asm
    asm.l
    asm.y
    ${CMAKE_CURRENT_BINARY_DIR}/asm.lex.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/asm.yacc.cpp
    sym_table.h
    sym_table.cpp
    tables.h
    tables.cpp
    codes.h
    lexer.h
    phase2.h
    stack.h
    y.tab.hpp
)
target_compile_definitions(asm PUBLIC $<$<CONFIG:DEBUG>:YYDEBUG>)
target_include_directories(asm PUBLIC .)
