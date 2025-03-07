project(runtime)

add_library(${PROJECT_NAME} STATIC
    runtime.c
    memory.c
    strings.c
    values.c
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
    $<$<CONFIG:DEBUG>:-DENABLE_AST_DUMP>
    #$<$<CONFIG:DEBUG>:-DENABLE_PARSER_TRACE>
    $<$<CONFIG:DEBUG>:-g>
    $<$<CONFIG:RELEASE>:-Ofast>
    $<$<CONFIG:PROFILE>:-pg -O0>
)
