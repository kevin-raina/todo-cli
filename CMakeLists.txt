cmake_minimum_required(VERSION 3.16)

project(todo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(fmt REQUIRED)

file(GLOB SOURCES
    src/*.cpp
)

add_executable(todo ${SOURCES})

target_link_libraries(todo PRIVATE fmt::fmt)

