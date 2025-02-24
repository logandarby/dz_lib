include(FetchContent)

# Set up Google Test

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

include(GoogleTest)

# Add Tests

add_executable(dz_array_test dz_array_test.cpp)
add_executable(dz_hashmap_test dz_hashmap_test.cpp)
add_executable(dz_arena_test dz_arena_test.cpp)
add_executable(dz_core_test dz_core_test.cpp)
# gtest_discover_tests(tests)
target_link_libraries(dz_array_test PRIVATE GTest::GTest DZ)
target_link_libraries(dz_hashmap_test PRIVATE GTest::GTest DZ)
target_include_directories(dz_hashmap_test PRIVATE "../src")
target_link_libraries(dz_arena_test PRIVATE GTest::GTest DZ)
target_link_libraries(dz_core_test PRIVATE GTest::GTest DZ)

add_test(dz_array_test_gtest dz_array_test)
add_test(dz_hashmap_test_gtest dz_hashmap_test)
add_test(dz_arena_test_gtest dz_arena_test)
add_test(dz_core_test_gtest dz_core_test)
