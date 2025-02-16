cmake_minimum_required(VERSION 3.25)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(DZ)

set(DZ_LIB_NAME "DZ")

# Sources
# MY_SOURCES is defined to be a list of all the source files in src that aren't main.cpp
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_library("${DZ_LIB_NAME}" SHARED ${MY_SOURCES})
target_include_directories(${DZ_LIB_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

set_property(TARGET "${DZ_LIB_NAME}" PROPERTY C_STANDARD 99)

# Configure resource files macro
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions("${DZ_LIB_NAME}" PUBLIC
        DZ_DEBUG
    )
    target_link_libraries(${DZ_LIB_NAME}
      -fsanitize=address
    )
endif()

# Add Tests

enable_testing()
add_subdirectory(tests)
