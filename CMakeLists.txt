cmake_minimum_required(VERSION 3.16)
project(c_black_hole C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(c_black_hole
  src/main.c
  src/engine/engine.c
  glad/glad.c
)

target_include_directories(c_black_hole PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
)

find_package(glfw3 REQUIRED)
target_link_libraries(c_black_hole PRIVATE glfw)

add_custom_target(build
  DEPENDS c_black_hole
)

add_custom_target(run
  COMMAND $<TARGET_FILE:c_black_hole>
  DEPENDS c_black_hole
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
