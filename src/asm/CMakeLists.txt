find_package(BISON)
find_package(FLEX)

if(NOT (BISON_FOUND AND FLEX_FOUND))
    message(WARNING "Bison and flex required to generate the assembler lexer and parser; using pregenerated sources")
    set(grammarSources
        generated/asm.lex.cpp
        generated/asm.yacc.hpp
        generated/asm.yacc.cpp
    )
    set(grammarInclude generated)
else()
    flex_target(asm-lib asm.l ${CMAKE_CURRENT_BINARY_DIR}/asm.lex.cpp)
    bison_target(asm-lib asm.y ${CMAKE_CURRENT_BINARY_DIR}/asm.yacc.cpp)
    set(grammarSources
        ${CMAKE_CURRENT_BINARY_DIR}/asm.lex.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/asm.yacc.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/asm.yacc.cpp
    )
    set(grammarInclude ${CMAKE_CURRENT_BINARY_DIR})
endif()

add_library(asm-lib STATIC
    actions.h
    actions.cpp
    asm.l
    asm.y
    ${grammarSources}
    sym_table.h
    sym_table.cpp
    codes.h
    lexer.h
    lexer.cpp
    parser.h
    parser.cpp
    phase2.h
    y.tab.hpp
)
target_include_directories(asm-lib PUBLIC . ${grammarInclude})
set_target_properties(asm-lib PROPERTIES FOLDER "Libraries")

add_executable(asm
    main.cpp
)
set_target_properties(asm PROPERTIES FOLDER "Tools")
target_compile_definitions(asm PUBLIC $<$<CONFIG:DEBUG>:YYDEBUG>)
target_include_directories(asm PUBLIC .)
target_link_libraries(asm PUBLIC asm-lib)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
