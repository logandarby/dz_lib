#pragma once

// Hashmap implementation for strings.

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

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
const char *hm_get(DzHashmap hm, const char *key);

// Adds a new key-value pair to the hashmap. The key and value are
// copied in, so memory management is not needed
void hm_add(DzHashmap hm, const char *key, const char *value,
            DzHmError *error);

// Deletes a a key-value pair from the hashmap based on a key value.
// If a corresponding value is not stored, then this is a no-op
void hm_delete(DzHashmap hm, const char *key);

// Gets the count of items stored in the hashmap
size_t hm_count(DzHashmap hm);
