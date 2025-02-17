#pragma once

// A generic, automatically resizing array implementation

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "dz_debug.h"

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
  ((!a) ? (void)0 : dz_impl_arr_free(dz_array_header(a)))

// Gets the length of the array a
#define dz_arrlen(a) ((!a) ? 0 : dz_array_header(a)->length)

// Returns the last item in the array, and removes it
#define dz_arrpop(a)                                        \
  (DZ_ASSERT(a), DZ_ASSERT(dz_array_header(a)->length),     \
   dz_impl_arr_maybe_shrink(dz_array_header(a), sizeof(*a), \
                            (void **)&a),                   \
   dz_array_header(a)->length--, a[dz_array_header(a)->length])

// Pushes an item into a. a can be a NULL pointer
#define dz_arrpush(a, item)                                  \
  do {                                                       \
    if (!a) {                                                \
      dz_impl_arr_init((void **)(&a), sizeof(*a));           \
    } else {                                                 \
      dz_impl_arr_maybe_grow(dz_array_header(a), sizeof(*a), \
                             (void **)&a);                   \
    }                                                        \
    a[dz_array_header(a)->length++] = item;                  \
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
// O(n) time
#define dz_arrremove(a, index)                                      \
  ((!a) ? (void)0                                                   \
        : dz_impl_arr_remove(dz_array_header(a), index, sizeof(*a), \
                             (void **)&a))

// Removes the item at index `index` by replacing it with the last
// element in the array Works in O(1) time, but does not preserve
// ordering of the array
#define dz_arrremove_and_replace(a, index)                          \
  ((!a) ? (void)0                                                   \
        : dz_impl_arr_remove_and_replace(dz_array_header(a), index, \
                                         sizeof(*a), (void **)&a))

// Inserts an item into the index `index`, and shift the rest
// of the array to accomodate
#define dz_arrinsert(a, index, item)                      \
  do {                                                    \
    DZ_ASSERT(a);                                         \
    dz_impl_arr_shift_at_index(dz_array_header(a), index, \
                               sizeof(*a), (void **)&a);  \
    a[index] = item;                                      \
  } while (0);

// Returns the index of the item stored at item_addr.
// NOTE: item_addr must be a valid address
// Uses memcmp under the hood to compare the value stored at
// item_addr, and the items in the array
// If the item is not found, returns -1.
#define dz_arrindexof(a, item_addr)                                 \
  (DZ_STATIC_ASSERT(                                                \
       sizeof(*a) == sizeof(*item_addr),                            \
       "In dz_addrindexof: The size of the item to find and the "   \
       "size of the items in the DzArray must match."),             \
   (!a || !item_addr)                                               \
       ? -1                                                         \
       : dz_impl_arr_indexof(dz_array_header(a), (void *)item_addr, \
                             sizeof(*a), sizeof(*item_addr),        \
                             (void **)&a))

// Implementation Details:

// Used to store information about the array
// The user of the API is given a pointer that points to the
// first memory location after the structure for data.
typedef struct DZArrayHeader {
  size_t length;
  size_t capacity;
} DZArrayHeader;

#define dz_array_header(a) (&((DZArrayHeader *)a)[-1])

extern void dz_impl_arr_init(void **arr_ref, size_t item_size);
extern void dz_impl_arr_maybe_grow(DZArrayHeader *header,
                                   size_t element_size,
                                   void **arr_ptr);
extern void dz_impl_arr_maybe_shrink(DZArrayHeader *header,
                                     size_t element_size,
                                     void **arr_ptr);
extern void dz_impl_arr_free(DZArrayHeader *arr);
extern void dz_impl_arr_remove(DZArrayHeader *header,
                               size_t index_to_remove,
                               size_t element_size, void **arr_ptr);
extern void dz_impl_arr_remove_and_replace(DZArrayHeader *header,
                                           size_t index_to_remove,
                                           size_t element_size,
                                           void **arr_ptr);
extern void dz_impl_arr_shift_at_index(DZArrayHeader *header,
                                       size_t index_to_add,
                                       size_t element_size,
                                       void **arr_ptr);
extern void dz_impl_arrprint(void *arr, const char *format,
                             size_t element_size);
extern ssize_t dz_impl_arr_indexof(DZArrayHeader *header,
                                   void *item_to_find_addr,
                                   size_t array_element_size,
                                   size_t item_to_find_size,
                                   void **arr_ptr);
