cmake_minimum_required(VERSION 3.25)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(DZ)

set(DZ_LIB_NAME "DZ")

# Sources
# MY_SOURCES is defined to be a list of all the source files in src that aren't main.cpp
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
# file(GLOB_RECURSE MY_INCLUDE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_library("${DZ_LIB_NAME}" STATIC ${MY_SOURCES})

set_property(TARGET "${DZ_LIB_NAME}" PROPERTY C_STANDARD 99)

# Configure resource files macro
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions("${DZ_LIB_NAME}" PUBLIC
        DZ_DEBUG
    )
endif()

# Add Tests

enable_testing()
add_subdirectory(tests)

# Final Link
# External Libraries
target_include_directories("${DZ_LIB_NAME}" PRIVATE ${MY_INCLUDE})
