cmake_minimum_required(VERSION 3.13)

project(Aether VERSION 1.0)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

add_library(
    aether_core
    src/constants.c
    src/engine.c
    src/hashing.c
    src/move.c
    src/move_gen.c
    src/perft.c
    src/position.c
    src/utils.c
)

add_executable(
    aether 
    src/uci.c
)

target_link_libraries(aether PRIVATE aether_core)

add_executable(
    tests
    src/tests.c
)

target_link_libraries(tests PRIVATE aether_core)
