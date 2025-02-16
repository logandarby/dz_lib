#pragma once

// A generic, automatically resizing array implementation

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "dz_core.h"

extern const size_t DZ_ARR_RESIZE_UP;
extern const size_t DZ_ARR_INIT_CAPACITY;

// Used to define an array.
// Usage Example:
//  DZArray(int) array;
//  dz_arrpush(array, 2);
//  printf("First item %d", array[0]);
//  dz_arrfree(array)
#define DZArray(T) T *

// Frees the array a
#define dz_arrfree(a) \
  ((!a) ? (void)0 : dz_arr_free_impl(dz_array_header(a)))

// Gets the length of the array a
#define dz_arrlen(a) ((!a) ? 0 : dz_array_header(a)->length)

// Returns the last item in the array, and removes it
#define dz_arrpop(a)                                                 \
  (DZ_ASSERT(a), DZ_ASSERT(dz_array_header(a)->length),              \
   dz_arr_maybe_shrink(dz_array_header(a), sizeof(*a), (void **)&a), \
   dz_array_header(a)->length--, a[dz_array_header(a)->length])

// Pushes an item into a. a can be a NULL pointer
#define dz_arrpush(a, item)                             \
  do {                                                  \
    if (!a) {                                           \
      dz_arr_init((void **)(&a), sizeof(*a));           \
    } else {                                            \
      dz_arr_maybe_grow(dz_array_header(a), sizeof(*a), \
                        (void **)&a);                   \
    }                                                   \
    a[dz_array_header(a)->length++] = item;             \
  } while (0)

// Prints the contents of the array
#define dz_arrprint(arr, format)                                 \
  do {                                                           \
    DZ_ASSERT(arr);                                              \
    if (!dz_array_header(arr)->length) {                         \
      printf("[]\n");                                            \
      break;                                                     \
    }                                                            \
    printf("[");                                                 \
    for (int i = 0; i < dz_array_header(arr)->length - 1; i++) { \
      printf(format, arr[i]);                                    \
      printf(", ");                                              \
    }                                                            \
    printf(format, arr[(dz_array_header(arr)->length - 1)]);     \
    printf("]\n");                                               \
  } while (0);

// Removes the item at index `index`, and shifts the rest of the
// array to fill the space up
#define dz_arrremove(a, index)                                      \
  ((!a) ? (void)0                                                   \
        : dz_arr_remove_impl(dz_array_header(a), index, sizeof(*a), \
                             (void **)&a))

// Inserts an item into the index `index`, and shift the rest
// of the array to accomodate
#define dz_arrinsert(a, index, item)                      \
  do {                                                    \
    DZ_ASSERT(a);                                         \
    dz_arr_shift_at_index_impl(dz_array_header(a), index, \
                               sizeof(*a), (void **)&a);  \
    a[index] = item;                                      \
  } while (0);

// Implementation Details:

// Used to store information about the array
// The user of the API is given a pointer that points to the
// first memory location after the structure for data.
typedef struct DZArrayHeader {
  size_t length;
  size_t capacity;
} DZArrayHeader;

#define dz_array_header(a) (&((DZArrayHeader *)a)[-1])

extern void dz_arr_init(void **arr_ref, size_t item_size);
extern void dz_arr_maybe_grow(DZArrayHeader *header,
                              size_t element_size, void **arr_ptr);
extern void dz_arr_maybe_shrink(DZArrayHeader *header,
                                size_t element_size, void **arr_ptr);
extern void dz_arr_free_impl(DZArrayHeader *arr);
extern void dz_arr_remove_impl(DZArrayHeader *header,
                               size_t index_to_remove,
                               size_t element_size, void **arr_ptr);
extern void dz_arr_shift_at_index_impl(DZArrayHeader *header,
                                       size_t index_to_add,
                                       size_t element_size,
                                       void **arr_ptr);
extern void dz_arrprint_impl(void *arr, const char *format,
                             size_t element_size);
