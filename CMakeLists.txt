if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
    message(FATAL_ERROR "No in-source builds allowed!")
endif()

cmake_minimum_required(VERSION 3.14)

project(Tetris)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "Configured build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(
        -fno-rtti
        -fno-exceptions
        -Wall
        -Werror
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "MSVC not supported yet.")
else()
    message(FATAL_ERROR "Compiler '${CMAKE_CXX_COMPILER_ID}' not supported!")
endif()

message(STATUS "Configuring vendor libraries")
add_subdirectory(vendor)

message(STATUS "Configuring libraries")
add_subdirectory(lib)
message(STATUS "Configuring game")
add_subdirectory(game)
message(STATUS "Configuring tests")
add_subdirectory(test)
