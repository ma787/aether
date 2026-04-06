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

target_include_directories(aether PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(aether PRIVATE aether_core)

add_executable(
    tests
    tests/test_main.c
    tests/test_position.c
    tests/test_move.c
    tests/test_hash.c
    tests/test_check.c
    tests/test_move_gen.c
    lib/unity/src/unity.c
)
target_include_directories(
    tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/unity/src
)

target_link_libraries(tests PRIVATE aether_core)
