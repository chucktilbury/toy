cmake_minimum_required(VERSION 3.10)
project(parser)

file(GLOB_RECURSE ${PROJECT_NAME}_files
    ${PROJECT_SOURCE_DIR}/*.c
)

add_library(${PROJECT_NAME} STATIC
    ${${PROJECT_NAME}_files}
)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../parser
    ${CMAKE_CURRENT_BINARY_DIR}/../ast
    ${CMAKE_CURRENT_BINARY_DIR}/../scanner
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/common
    ${CMAKE_SOURCE_DIR}/src/compiler/ast
    ${CMAKE_SOURCE_DIR}/src/compiler/scanner
    ${CMAKE_SOURCE_DIR}/src/compiler/parser
    "/usr/local/include"
)

link_directories(
    "/usr/local/lib"
)

add_definitions(
    -Wall
    -Wextra
    -Wpedantic
    -pedantic
)

if(CMAKE_BUILD_TYPE STREQUAL "debug")
    add_definitions(
        -g
        -O0
        -DMEMORY_DEBUG
        #-DUSE_TRACE
        -DUSE_ASSERTS
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "release")
    add_definitions(
        -Ofast
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "profile")
    add_definitions(
        -O0
        -pg
    )
endif()


