cmake_minimum_required(VERSION 3.13)

project(Aether VERSION 1.0)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

add_executable(
    aether 
    src/uci.c

    src/constants.c
    src/engine.c
    src/hashing.c
    src/move.c
    src/move_gen.c
    src/perft.c
    src/position.c
    src/string_funcs.c
    src/utils.c
)

add_executable(
    tests
    src/tests.c

    src/constants.c
    src/hashing.c
    src/move.c
    src/move_gen.c
    src/perft.c
    src/position.c
    src/string_funcs.c
    src/utils.c
)

if (WIN32)
    target_link_libraries(aether PRIVATE regex)
    target_link_libraries(tests PRIVATE regex)
endif (WIN32)