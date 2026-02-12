cmake_minimum_required(VERSION 3.16)
project(c_black_hole C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(c_black_hole
  src/main.c
  src/engine/engine.c
  src/engine/gl_utils.c
  src/physics/constants.c
  src/blackhole/blackhole.c
  src/ray/ray.c
  glad/glad.c
)

target_include_directories(c_black_hole PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
)

find_package(glfw3 REQUIRED)
target_link_libraries(c_black_hole PRIVATE glfw m)

add_custom_target(build
  DEPENDS c_black_hole
)

add_custom_target(run
  COMMAND ${CMAKE_COMMAND} -DRUN_BINARY=$<TARGET_FILE:c_black_hole> -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/run.cmake
  DEPENDS c_black_hole
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
