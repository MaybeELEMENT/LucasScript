cmake_minimum_required(VERSION 3.15)

# Project info
project(Lexter
    VERSION 1.0
    DESCRIPTION "Lexter C++17 executable project"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add source files (.cc files)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cc"
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Force output binary name to lowercase
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "lexter")

# Include headers
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Optional: Set warnings and useful compile options
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()