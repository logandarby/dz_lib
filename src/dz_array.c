#include "dz_array.h"

#include <memory.h>
#include <stdlib.h>
#include <string.h>

const size_t DZ_ARR_RESIZE_UP = 2;
const size_t DZ_ARR_RESIZE_DOWN = 2;
const size_t DZ_ARR_INIT_CAPACITY = 50;

static inline size_t imax(size_t a, size_t b) {
  return (a > b) ? a : b;
}

void dz_assert(bool exp) { assert(exp); }

/**
Returns the array in bytes
 */
static inline uint8_t *dz_arr_get_ptr_from_header(
    DZArrayHeader *header) {
  return (uint8_t *)&header[1];
}

void dz_impl_arr_init(void **arr_ref, size_t item_size) {
  dz_assert(arr_ref != NULL);
  dz_assert(*arr_ref == NULL);  // Should initialize a null array
  DZArrayHeader *header = (DZArrayHeader *)malloc(
      sizeof(DZArrayHeader) + item_size * DZ_ARR_INIT_CAPACITY);
  header->capacity = DZ_ARR_INIT_CAPACITY;
  header->length = 0;
  *arr_ref = dz_arr_get_ptr_from_header(header);
}

static void dz_arr_resize(DZArrayHeader *header, size_t element_size,
                          void **arr_ptr, size_t new_capacity) {
  header->capacity = new_capacity;
  DZArrayHeader *new_header = (DZArrayHeader *)realloc(
      header, sizeof(DZArrayHeader) + new_capacity * element_size);
  *arr_ptr = dz_arr_get_ptr_from_header(new_header);
}

void dz_impl_arr_maybe_grow(DZArrayHeader *header,
                            size_t element_size, void **arr_ptr) {
  if (header->length >= header->capacity) {
    const size_t new_capacity = header->capacity * DZ_ARR_RESIZE_UP;
    dz_arr_resize(header, element_size, arr_ptr, new_capacity);
  }
}

void dz_impl_arr_maybe_shrink(DZArrayHeader *header,
                              size_t element_size, void **arr_ptr) {
  size_t shrunk_capacity = imax(header->capacity / DZ_ARR_RESIZE_DOWN,
                                DZ_ARR_INIT_CAPACITY);
  if (header->length < shrunk_capacity &&
      header->capacity > DZ_ARR_INIT_CAPACITY) {
    dz_arr_resize(header, element_size, arr_ptr, shrunk_capacity);
  }
}

void dz_impl_arr_remove(DZArrayHeader *header, size_t index_to_remove,
                        size_t element_size, void **arr_ptr) {
  uint8_t *array_bytes = (uint8_t *)*arr_ptr;
  for (size_t i = index_to_remove; i < header->length; i++) {
    memmove(&array_bytes[i * element_size],
            &array_bytes[(i + 1) * element_size], element_size);
  }
  header->length--;
  dz_impl_arr_maybe_shrink(header, element_size, arr_ptr);
}

void dz_impl_arr_remove_and_replace(DZArrayHeader *header,
                                    size_t index_to_remove,
                                    size_t element_size,
                                    void **arr_ptr) {
  uint8_t *array_bytes = (uint8_t *)*arr_ptr;
  header->length--;
  memcpy(&array_bytes[index_to_remove * element_size],
         &array_bytes[header->length * element_size], element_size);
}

void dz_impl_arr_shift_at_index(DZArrayHeader *header,
                                size_t index_to_add,
                                size_t element_size, void **arr_ptr) {
  dz_assert(*arr_ptr);
  dz_assert(index_to_add <= header->length);
  header->length++;
  dz_impl_arr_maybe_grow(header, element_size, arr_ptr);
  uint8_t *array_bytes = (uint8_t *)*arr_ptr;
  for (size_t i = header->length - 1; i > index_to_add; i--) {
    memmove(&array_bytes[i * element_size],
            &array_bytes[(i - 1) * element_size], element_size);
  }
}

void dz_impl_arr_free(DZArrayHeader *arr) {
  dz_assert(arr);
  free(arr);
}

ssize_t dz_impl_arr_indexof(DZArrayHeader *header, void *item_to_find,
                            size_t array_element_size,
                            size_t item_to_find_size,
                            void **arr_ptr) {
  uint8_t *array_bytes = (uint8_t *)*arr_ptr;
  for (size_t i = 0; i < header->length; i++) {
    const int cmp =
        memcmp(item_to_find, &array_bytes[i * array_element_size],
               array_element_size);
    if (cmp == 0) {
      return i;
    }
  }
  return -1;
}
