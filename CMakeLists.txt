cmake_minimum_required(VERSION 3.19)
project(Aether VERSION 1.0)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug)
endif()

option(AETHER_SANITIZE "" OFF)

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

add_executable(aether src/uci.c)
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

set_target_properties(
  aether tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>"
)

target_include_directories(aether PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(aether PRIVATE aether_core)

target_include_directories(
  tests PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/lib/unity/src
)
target_link_libraries(tests PRIVATE aether_core)

if(AETHER_SANITIZE)
  target_compile_options(
    aether_core PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -g
  )
  target_compile_options(
    aether PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -g
  )
  target_compile_options(
    tests PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -g
  )
  target_link_options(aether PRIVATE -fsanitize=address,undefined)
  target_link_options(tests PRIVATE -fsanitize=address,undefined)
endif()

enable_testing()
add_test(NAME aether_tests COMMAND $<TARGET_FILE:tests>)
