cmake_minimum_required(VERSION 3.17)
project(archive C)
set(CMAKE_C_STANDARD 99)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/")
find_package(Lit REQUIRED)

find_package(LibArchive REQUIRED)
include_directories(include/ ${LIT_INCLUDE_DIRS} ${LibArchive_INCLUDE_DIRS})

add_library(archive SHARED src/lit_archive.c include/lit_archive.h)
set_target_properties(archive PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dist")

target_link_libraries(archive ${LIT_LIBRARIES} ${LibArchive_LIBRARIES} m)

add_executable(test src/test.c)
set_target_properties(test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(test LINK_PUBLIC archive)

install(TARGETS archive DESTINATION lib)