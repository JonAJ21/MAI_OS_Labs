cmake_minimum_required(VERSION 3.10)
project(lab5 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lib1 STATIC lib1.cpp lib.h)
add_library(lib2 STATIC lib2.cpp lib.h)
add_executable(prog1 prog1.cpp)
target_link_libraries(prog1 lib1 lib2 m)

add_library(lib1d SHARED lib1.cpp lib.h)
add_library(lib2d SHARED lib2.cpp lib.h)

add_executable(prog2 prog2.cpp)
target_link_libraries(lib1d m)
target_link_libraries(lib2d m)
target_link_libraries(prog2 ${CMAKE_DL_LIBS})