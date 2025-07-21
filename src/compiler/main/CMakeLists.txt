project(toy)

include(${CMAKE_SOURCE_DIR}/CMakeBuildOpts.txt)

add_executable(${PROJECT_NAME}
    main.c
)

target_link_libraries(${PROJECT_NAME}
    #gc
    #cord
    scanner
    parser
    ast
    common
)
