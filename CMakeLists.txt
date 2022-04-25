cmake_minimum_required(VERSION 3.17)
project(lib C)
set(CMAKE_C_STANDARD 99)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/")
find_package(Lit REQUIRED)

include_directories(include/ ${LIT_INCLUDE_DIRS})

add_library(lib src/lib.c include/lib.h)
target_link_libraries(lib ${LIT_LIBRARIES} m)

set_target_properties(lib PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dist")

add_executable(test src/test.c)
set_target_properties(test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(test LINK_PUBLIC lib)