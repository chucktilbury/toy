project(scanner)

find_package(FLEX 2.6 REQUIRED)

FLEX_TARGET(SCANNER
    scanner.l
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/scanner.h
    COMPILE_FLAGS
)

file(GLOB_RECURSE ${PROJECT_NAME}_files
    ${PROJECT_SOURCE_DIR}/*.c
)

add_library(${PROJECT_NAME} STATIC
    #${BISON_PARSER_OUTPUTS}
    ${FLEX_SCANNER_OUTPUTS}
    ${${PROJECT_NAME}_files}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/build/src/scanner
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src/common
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wno-sign-compare
    -Wno-unused-variable
    -Wno-parentheses-equality
    #-DUSE_GC
    $<$<CONFIG:DEBUG>:-DENABLE_AST_DUMP>
    $<$<CONFIG:DEBUG>:-g>
    $<$<CONFIG:RELEASE>:-Ofast>
    $<$<CONFIG:PROFILE>:-pg -O0>
)


