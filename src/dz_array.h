#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

extern const size_t DZ_ARR_RESIZE_UP;
extern const size_t DZ_ARR_INIT_CAPACITY;

typedef struct DZArrayHeader {
  size_t length;
  size_t capacity;
} DZArrayHeader;

extern void dz_assert(bool exp);

#define DZArray(T) T *
#define dz_arrlen(a) ((!a) ? 0 : dz_array_header(a)->length)
#define dz_arrfree(a) \
  ((!a) ? (void)0 : dz_arr_free_impl(dz_array_header(a)))

#define dz_arrpop(a)                                                 \
  (dz_assert(a), dz_assert(dz_array_header(a)->length),              \
   dz_arr_maybe_shrink(dz_array_header(a), sizeof(*a), (void **)&a), \
   dz_array_header(a)->length--, a[dz_array_header(a)->length])

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

#define dz_arrprint(arr, format)                                 \
  do {                                                           \
    dz_assert(arr);                                              \
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

#define dz_arrremove(a, index)                                      \
  ((!a) ? (void)0                                                   \
        : dz_arr_remove_impl(dz_array_header(a), index, sizeof(*a), \
                             (void **)&a))

#define dz_arrinsert(a, index, item)                      \
  do {                                                    \
    dz_assert(a);                                         \
    dz_arr_shift_at_index_impl(dz_array_header(a), index, \
                               sizeof(*a), (void **)&a);  \
    a[index] = item;                                      \
  } while (0);

// Implementation Details:

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
