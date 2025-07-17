project(toy)

include(${PROJECT_SOURCE_DIR}/../../CMakeBuildOpts.txt)

find_package(FLEX 2.6 REQUIRED)

FLEX_TARGET(SCANNER
    scanner.l
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/scanner.h
    COMPILE_FLAGS
)

add_executable(${PROJECT_NAME}
    main.c
    parser.c
    tokens.c
    file_io.c
    ${FLEX_SCANNER_OUTPUTS}
)

target_link_libraries(${PROJECT_NAME}
    #gc
    #cord
    common
)
