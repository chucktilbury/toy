project(runtime)

add_library(${PROJECT_NAME} STATIC
    runtime.c
    rt_memory.c
    rt_strings.c
    rt_object.c
    rt_errors.c
    rt_bool_obj.c
    rt_float_obj.c
    rt_func_obj.c
    rt_int_obj.c
    rt_str_obj.c
    rt_user_obj.c
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/../common
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
