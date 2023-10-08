find_package(BISON)
find_package(FLEX)

if(NOT (BISON_FOUND AND FLEX_FOUND))
    message(WARNING "Bison and flex required to generate the assembler lexer and parser; using pregenerated sources")
    set(grammarSources
        generated/asm.lex.cpp
        generated/asm.yacc.hpp
        generated/asm.yacc.cpp
    )
else()
    flex_target(asm asm.l ${CMAKE_CURRENT_BINARY_DIR}/asm.lex.cpp)
    bison_target(asm asm.y ${CMAKE_CURRENT_BINARY_DIR}/asm.yacc.cpp)
    set(grammarSources
        ${CMAKE_CURRENT_BINARY_DIR}/asm.lex.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/asm.yacc.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/asm.yacc.cpp
    )
endif()

add_executable(asm
    asm.l
    asm.y
    ${grammarSources}
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
set_target_properties(asm PROPERTIES FOLDER "Tools")
target_compile_definitions(asm PUBLIC $<$<CONFIG:DEBUG>:YYDEBUG>)
target_include_directories(asm PUBLIC .)
