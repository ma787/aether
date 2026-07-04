cmake_minimum_required(VERSION 3.25)
project(Aether VERSION 1.0)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(AETHER_SANITIZE "" OFF)

include(FetchContent)
FetchContent_Declare(
  Unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG        v2.6.1
  GIT_SHALLOW    TRUE
  SYSTEM
)
FetchContent_MakeAvailable(Unity)

add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Wvla)

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
)

set_target_properties(
  aether tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>"
)

target_include_directories(aether PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(aether PRIVATE aether_core)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(tests PRIVATE aether_core unity::framework)

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
