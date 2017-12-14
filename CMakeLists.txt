cmake_minimum_required(VERSION 3.3)
project(Collection)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES collection.c main.c)
add_executable(collection ${SOURCE_FILES})
