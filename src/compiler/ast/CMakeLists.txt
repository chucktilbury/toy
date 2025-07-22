cmake_minimum_required(VERSION 3.10)
project(ast)

include(${PROJECT_SOURCE_DIR}/../CompilerBuildOpts.txt)

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_files} )