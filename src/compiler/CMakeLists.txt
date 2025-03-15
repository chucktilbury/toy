project(toy)

find_package(FLEX 2.6 REQUIRED)
find_package(BISON 3.8 REQUIRED)

BISON_TARGET(PARSER
    parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.h
    COMPILE_FLAGS ""
)

FLEX_TARGET(SCANNER
    scanner.l
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/scanner.h
    COMPILE_FLAGS "-i"
)

ADD_FLEX_BISON_DEPENDENCY(SCANNER PARSER)

add_executable(${PROJECT_NAME}
    ${BISON_PARSER_OUTPUTS}
    ${FLEX_SCANNER_OUTPUTS}
    main.c
    tokens.c
    fileio.c
    errors.c
    memory.c
    hash.c
    ast.c
    ast_tables.c
    pointer_list.c
    string_buffer.c
    string_list.c
    context.c
    trace.c
    symbol.c
    value.c
    symbol_table.c
    type_table.c
    dump_symbols.c
    emit_code.c
)

target_link_libraries(${PROJECT_NAME}
    gc
    cord
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wno-sign-compare
    -Wno-unused-variable
    -Wno-parentheses-equality
    -DUSE_GC
    $<$<CONFIG:DEBUG>:-DENABLE_AST_DUMP>
    #$<$<CONFIG:DEBUG>:-DENABLE_PARSER_TRACE>
    $<$<CONFIG:DEBUG>:-g>
    $<$<CONFIG:RELEASE>:-Ofast>
    $<$<CONFIG:PROFILE>:-pg -O0>
)
