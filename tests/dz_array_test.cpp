#include <gtest/gtest.h>
#include <stdio.h>

#include <cstddef>

extern "C" {
#include "dz_array.h"
}

TEST(Array, NullFree) {
  DZArray(int) dynamic_array = NULL;
  dz_arrfree(dynamic_array);
  ASSERT_EQ((void *)dynamic_array, (void *)NULL);
}

TEST(Array, Initialization) {
  DZArray(int) dynamic_array = NULL;
  size_t len = dz_arrlen(dynamic_array);
  ASSERT_EQ(len, 0);
  dz_arrfree(dynamic_array);
  ASSERT_EQ((void *)dynamic_array, (void *)NULL);
}

TEST(Array, Pushing) {
  DZArray(int) arr = NULL;
  dz_arrpush(arr, 8);
  dz_arrpush(arr, 10);
  ASSERT_EQ(arr[0], 8);
  ASSERT_EQ(arr[1], 10);
  dz_arrfree(arr);
}

TEST(Array, PushingToCapacity) {
  DZArray(int) arr = NULL;
  for (size_t i = 0; i < DZ_ARR_INIT_CAPACITY; i++) {
    dz_arrpush(arr, i);
  }
  size_t last_index = DZ_ARR_INIT_CAPACITY - 1;
  ASSERT_EQ(arr[last_index], last_index);
  ASSERT_EQ(dz_array_header(arr)->capacity, DZ_ARR_INIT_CAPACITY);
  dz_arrfree(arr);
}

TEST(Array, PushingAboveCapacity) {
  DZArray(int) arr = NULL;
  for (size_t i = 0; i < DZ_ARR_INIT_CAPACITY; i++) {
    dz_arrpush(arr, i);
  }
  ASSERT_EQ(dz_array_header(arr)->capacity, DZ_ARR_INIT_CAPACITY);
  dz_arrpush(arr, 1);
  ASSERT_EQ(dz_array_header(arr)->capacity,
            DZ_ARR_INIT_CAPACITY * DZ_ARR_RESIZE_UP);
  ASSERT_EQ(arr[DZ_ARR_INIT_CAPACITY], 1);
  dz_arrfree(arr);
}

TEST(Array, PushingAboveCapacityThenShrink) {
  DZArray(int) arr = NULL;
  for (size_t i = 0; i < DZ_ARR_INIT_CAPACITY * DZ_ARR_RESIZE_UP;
       i++) {
    dz_arrpush(arr, i);
  }
  ASSERT_EQ(dz_array_header(arr)->capacity,
            DZ_ARR_INIT_CAPACITY * DZ_ARR_RESIZE_UP);
  for (size_t i = 0; i < DZ_ARR_INIT_CAPACITY * DZ_ARR_RESIZE_UP - 1;
       i++) {
    dz_arrpop(arr);
  }
  ASSERT_EQ(dz_array_header(arr)->capacity, DZ_ARR_INIT_CAPACITY);
  dz_arrfree(arr);
}

TEST(Array, Print) {
  DZArray(int) arr = NULL;
  for (size_t i = 0; i < 10; i++) {
    dz_arrpush(arr, i);
  }
  dz_arrprint(arr, "%d");
  dz_arrfree(arr);
}

TEST(Array, Pop) {
  DZArray(double) arr = NULL;
  for (size_t i = 0; i < 10; i++) {
    dz_arrpush(arr, (double)i);
  }
  ASSERT_EQ(arr[0], 0.0);
  ASSERT_EQ(arr[1], 1.0);
  dz_arrprint(arr, "%lf");
  double item = dz_arrpop(arr);
  ASSERT_EQ(item, 9.0);
  ASSERT_EQ(dz_arrlen(arr), 9);
  item = dz_arrpop(arr);
  ASSERT_EQ(item, 8.0);
  ASSERT_EQ(dz_arrlen(arr), 8);
  item = dz_arrpop(arr);
  ASSERT_EQ(item, 7.0);
  ASSERT_EQ(dz_arrlen(arr), 7);
  dz_arrfree(arr);
}

TEST(Array, Remove) {
  DZArray(double) arr = NULL;
  for (size_t i = 0; i < 10; i++) {
    dz_arrpush(arr, (double)i);
  }
  ASSERT_EQ(arr[3], 3);
  dz_arrremove(arr, 3);
  ASSERT_EQ(arr[3], 4);
  dz_arrremove(arr, 3);
  dz_arrremove(arr, 3);
  dz_arrremove(arr, 3);
  ASSERT_EQ(arr[3], 7);
  dz_arrfree(arr);
}

TEST(Array, Insert) {
  DZArray(double) arr = NULL;
  for (size_t i = 0; i < 10; i++) {
    dz_arrpush(arr, (double)i);
  }
  ASSERT_EQ(arr[3], 3);

  dz_arrinsert(arr, 3, 2.5);
  ASSERT_EQ(arr[3], 2.5);
  ASSERT_EQ(dz_arrlen(arr), 11);

  dz_arrinsert(arr, 3, 2.1);
  ASSERT_EQ(arr[3], 2.1);
  ASSERT_EQ(arr[4], 2.5);
  ASSERT_EQ(arr[5], 3);
  ASSERT_EQ(dz_arrlen(arr), 12);

  ASSERT_EQ(arr[11], 9);

  dz_arrfree(arr);
}

TEST(Array, InsertAtEnd) {
  DZArray(double) arr = NULL;
  for (size_t i = 0; i < 10; i++) {
    dz_arrpush(arr, (double)i);
  }
  ASSERT_EQ(arr[3], 3);

  dz_arrinsert(arr, 9, 8.5);
  ASSERT_EQ(arr[9], 8.5);
  ASSERT_EQ(arr[10], 9);
  ASSERT_EQ(dz_arrlen(arr), 11);

  dz_arrinsert(arr, 11, 12);
  ASSERT_EQ(arr[11], 12);
  ASSERT_EQ(dz_arrlen(arr), 12);

  dz_arrprint(arr, "%lf");

  dz_arrfree(arr);
}

TEST(Array, Insert_at_beginning) {
  DZArray(double) arr = NULL;
  for (size_t i = 0; i < 10; i++) {
    dz_arrpush(arr, (double)i);
  }
  dz_arrinsert(arr, 0, -1);
  ASSERT_EQ(dz_arrlen(arr), 11);
  ASSERT_EQ(arr[0], -1);
  ASSERT_EQ(arr[1], 0);

  dz_arrprint(arr, "%lf");

  dz_arrfree(arr);
}

TEST(Array, Remove_and_replace) {
  printf("Size of double %zu\n", sizeof(double));
  DZArray(double) arr = NULL;
  for (size_t i = 0; i < 10; i++) {
    dz_arrpush(arr, (double)i);
  }
  ASSERT_EQ(dz_arrlen(arr), 10);
  ASSERT_EQ(arr[9], 9);
  ASSERT_EQ(arr[3], 3);
  dz_arrremove_and_replace(arr, 3);
  ASSERT_EQ(dz_arrlen(arr), 9);  // length should be one less
  ASSERT_EQ(arr[8], 8);
  ASSERT_EQ(arr[3], 9);  // Index should be replaced w last element
  // Surrounding bytes should be kept intact
  ASSERT_EQ(arr[2], 2);
  ASSERT_EQ(arr[4], 4);

  dz_arrfree(arr);
}

TEST(Array, Find_Index) {
  DZArray(double) arr = NULL;

  for (size_t i = 0; i < 10; i++) {
    dz_arrpush(arr, (double)i);
  }
  double find_expr = 6;
  ASSERT_EQ(dz_arrindexof(arr, &find_expr), 6);
  find_expr = 0;
  ASSERT_EQ(dz_arrindexof(arr, &find_expr), 0);
  find_expr = 9;
  ASSERT_EQ(dz_arrindexof(arr, &find_expr), 9);
  find_expr = 10;
  ASSERT_EQ(dz_arrindexof(arr, &find_expr), -1);

  dz_arrfree(arr);
}

TEST(Array, Clear) {
  DZArray(double) arr = NULL;

  for (size_t i = 0; i < 10; i++) {
    dz_arrpush(arr, (double)i);
  }

  dz_arrclear(arr);
  
  ASSERT_EQ(dz_arrlen(arr), 0);

  dz_arrpush(arr, 1);
  dz_arrpush(arr, 2);
  dz_arrpush(arr, 3);

  ASSERT_EQ(dz_arrlen(arr), 3);
  ASSERT_EQ(arr[0], 1);
  ASSERT_EQ(arr[1], 2);
  ASSERT_EQ(arr[2], 3);

  dz_arrfree(arr);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
