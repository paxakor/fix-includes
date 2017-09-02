cmake_minimum_required(VERSION 3.0)
project(fix-includes)

set(SOURCES
    category.cpp
    fix_includes.cpp
    main.cpp
    source_file.cpp
    util.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra -Wpedantic")

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(GIT_BRANCH STREQUAL "master")
    message(STATUS "Release version")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Werror -DNDEBUG")
else()
    message(STATUS "Developer version")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -ggdb -DDEBUG")
endif()

add_executable(fix-includes ${SOURCES})
target_link_libraries(fix-includes stdc++fs)
