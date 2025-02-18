#pragma once

// Hashmap implementation for strings.
// Open addressed, double hashed

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern const size_t HM_INIT_CAPACITY;
typedef struct DzHashmapInstance *DzHashmap;

typedef enum DzHmError {
  DzHmError_None,    // No error
  DzHmError_Memory,  // Error with memory allocation

  DzHmError_Count
} DzHmError;

const char *hm_error_get_failure_str(enum DzHmError error_enum);
bool hm_has_error(DzHmError *error_ref);

// Initializes a Hashmap
// Caller must free the hashmap using hm_free
DzHashmap hm_init(DzHmError *error);

// Frees a hashmap
void hm_free(DzHashmap hm);

// Get the value of the hashmap stored with key. If nothing is found,
// returns NULL
const void *hm_get(DzHashmap hm, const void *key, const size_t keysize);

// Version of hm_get for strings 
// Assumes that key value pairs are strings
const char *hm_get_str(DzHashmap hm, const char *key) {
  return (char*)hm_get(hm, key, strlen(key) + 1);
}

// Adds a new key-value pair to the hashmap. The key and value are
// copied in, so memory management is not needed
void hm_add(DzHashmap hm, const void *key, size_t keysize, const void *value, size_t valuesize,
            DzHmError *error);

// A version of hm_add specifically for string keys and values
void hm_add_str(DzHashmap hm, const char *key, const char *value, DzHmError *error) {
  hm_add(hm, key, strlen(key) + 1, value, strlen(value) + 1, error);
}

// Deletes a a key-value pair from the hashmap based on a key value.
// If a corresponding value is not stored, then this is a no-op
void hm_delete(DzHashmap hm, const void *key, size_t keysize);

// Version of hm_delete for string key-value pairs
void hm_delete_str(DzHashmap hm, const char *key) {
  hm_delete(hm, key, strlen(key) + 1);
}

// Gets the count of items stored in the hashmap
size_t hm_count(DzHashmap hm);
