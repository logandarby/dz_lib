#include "dz_hashmap.h"

#include <math.h>
#include <string.h>
#include <limits.h>

#include "dz_core.h"

typedef struct DzHashmapItem {
  char *key;
  char *value;
} DzHashmapItem;

typedef struct DzHashmapInstance {
  DzHashmapItem **items;
  size_t capacity;
  size_t count;
} DzHashmapInstance;

const size_t HM_INIT_CAPACITY = 53;
static const size_t HM_PRIME_1 = 100008317;
static const size_t HM_PRIME_2 = 100008317 + 2;
static const double HM_RESIZE_UP_THRESHOLD = 0.7;
static const size_t MAX_KEY_SIZE = SIZE_MAX;

static DzHashmapItem DELETED_ITEM = {.key = NULL, .value = NULL};

static const char *DZ_HM_ERROR_STRINGS[DzHmError_Count] = {
    [DzHmError_None] = "No error",
    [DzHmError_Memory] = "Could not allocate memory",
};

static void hm_error_set(DzHmError *error_ref, DzHmError value) {
  if (error_ref) {
    *error_ref = value;
  }
}

static bool is_prime(const size_t x) {
  if (x <= 2) {
    return false;
  }
  if (x % 2 == 0) {
    return false;
  }
  for (size_t i = 3; i < (size_t)floor(sqrt(x)); i++) {
    if (x % i == 0) {
      return false;
    }
  }
  return true;
}

static size_t prime_find_next(size_t x) {
  if (x <= 1) {
    return 2;
  }
  x++;
  while (!is_prime(x)) {
    x++;
  }
  return x;
}

bool hm_has_error(DzHmError *error_ref) {
  return error_ref && *error_ref;
}

const char *hm_error_get_failure_str(DzHmError error_enum) {
  return DZ_HM_ERROR_STRINGS[error_enum];
}

DzHashmap hm_init_with_capacity(size_t capacity, DzHmError *error) {
  DzHashmap hm = (DzHashmap)calloc(1, sizeof(struct DzHashmapInstance));
  DZ_ASSERT(hm, "Malloc failed on hashmap failed");
  if (!hm) {
    hm_error_set(error, DzHmError_Memory);
    return NULL;
  }
  hm->count = 0;
  hm->capacity = capacity;
  hm->items = (DzHashmapItem **)calloc(hm->capacity, (sizeof *hm->items));
  DZ_ASSERT(hm->items, "Malloc failed on hashmap items");
  if (!hm->items) {
    hm_error_set(error, DzHmError_Memory);
    return NULL;
  }
  return hm;
}

DzHashmap hm_init(DzHmError *error) {
  return hm_init_with_capacity(HM_INIT_CAPACITY, error);
}

static void hm_item_free(DzHashmapItem *item) {
  if (!item || item == &DELETED_ITEM) {
    return;
  }
  if (item->key) {
    free(item->key);
  }
  if (item->value) {
    free(item->value);
  }
  free(item);
}

void hm_free(DzHashmap hm) {
  DZ_ASSERT(hm);
  if (!hm) {
    return;
  }
  for (size_t i = 0; i < hm->capacity; i++) {
    DzHashmapItem *item = hm->items[i];
    if (item && item != &DELETED_ITEM) {
      hm_item_free(item);
    }
  }
  if (hm->items) {
    free(hm->items);
  }
  free(hm);
  hm = NULL;
}

static DzHashmapItem *hm_item_create(const char *key, const char *value) {
  DZ_ASSERT(key, "Caller must supply a key");
  DZ_ASSERT(value, "Caller must supply a value");
  if (!key || !value) {
    return NULL;
  }
  DzHashmapItem *item = (DzHashmapItem *)calloc(1, sizeof(DzHashmapItem));
  item->key = strdup(key);
  DZ_ASSERT(item->key, "Strdup failed.");
  item->value = strdup(value);
  DZ_ASSERT(item->value, "Strdup failed.");
  return item;
}

static size_t hm_internal_hash(const char *str, const size_t a,
                               const size_t bucket_count) {
  DZ_ASSERT(str);
  size_t hash = 0;
  const size_t str_len = strlen(str);
  for (size_t i = 0; i < str_len; i++) {
    hash += pow(a, str_len - (i + 1)) * (size_t)str[i];
  }
  return hash % bucket_count;
}

static size_t hm_internal_get_index_hash(const char *str,
                                         const size_t bucket_count,
                                         const size_t attempt) {
  DZ_ASSERT(str);
  const size_t hash_a =
      hm_internal_hash(str, HM_PRIME_1, bucket_count);
  const size_t hash_b =
      hm_internal_hash(str, HM_PRIME_2, bucket_count);
  return (hash_a + (attempt * (hash_b + 1))) % bucket_count;
}

const char *hm_get(DzHashmap hm, const char *key) {
  DZ_ASSERT(hm, "Caller must supply a hashmap");
  DZ_ASSERT(key, "Caller must supply a key");
  if (!hm || !key) {
    return NULL;
  }
  size_t index = hm_internal_get_index_hash(key, hm->capacity, 0);
  DzHashmapItem *current_item = hm->items[index];
  size_t i = 1;
  while (current_item != NULL) {
    if (current_item != &DELETED_ITEM &&
        str_eq(current_item->key, key, strlen(current_item->key))) {
      return current_item->value;
    }
    index = hm_internal_get_index_hash(key, hm->capacity, i);
    current_item = hm->items[index];
    i++;
  }
  return NULL;
}

void hm_resize(DzHashmap hm, const size_t new_size, DzHmError *error) {
  DZ_ASSERT(hm, "Caller must supply a hashmap");
  if (new_size < HM_INIT_CAPACITY) {
    return;
  }
  hm_error_set(error, DzHmError_None);
  DzHashmap new_hm_buffer = hm_init_with_capacity(new_size, error);
  if (hm_has_error(error)) {
    return;
  }
  for (size_t i = 0; i < hm->count; i++) {
    DzHashmapItem *old_item = hm->items[i];
    if (old_item && old_item != &DELETED_ITEM) {
      hm_add(new_hm_buffer, old_item->key, old_item->value, error);
      if (hm_has_error(error)) {
        DZ_ASSERT(!hm_has_error(error), "There was a problem adding an item");
        hm_free(new_hm_buffer);
        return;
      }
    }
  }
  DzHashmapItem **old_items = hm->items;
  hm->items = new_hm_buffer->items;
  new_hm_buffer->items = old_items;

  size_t old_count = hm->count;
  hm->count = new_hm_buffer->count;
  new_hm_buffer->count = old_count;

  size_t old_capacity = hm->capacity;
  hm->capacity = new_hm_buffer->capacity;
  new_hm_buffer->capacity = old_capacity;

  hm_free(new_hm_buffer);
}

void hm_add(DzHashmap hm, const char *key, const char *value,
            DzHmError *error) {
  DZ_ASSERT(hm, "Caller must supply a hashmap");
  DZ_ASSERT(key, "Caller must supply a key");
  DZ_ASSERT(value, "Caller must supply a value");
  DZ_ASSERT(strlen(key) < MAX_KEY_SIZE, "Key is too large");
  if (!hm || !key || !value) {
    hm_error_set(error, DzHmError_Memory);
    return;
  }
  if (hm->capacity * HM_RESIZE_UP_THRESHOLD <= hm->count) {
    hm_resize(hm, prime_find_next(hm->capacity), error);
    if (hm_has_error(error)) {
      return;
    }
  }
  DzHashmapItem *item = hm_item_create(key, value);
  size_t index = hm_internal_get_index_hash(key, hm->capacity, 0);
  DzHashmapItem *current_item = hm->items[index];
  size_t i = 1;
  while (current_item != NULL && current_item != &DELETED_ITEM) {
    if (str_eq(current_item->key, key, strlen(current_item->key))) {
      hm_item_free(current_item);
      hm->items[index] = item;
      return;
    }
    index = hm_internal_get_index_hash(key, hm->capacity, i);
    current_item = hm->items[index];
    i++;
  }
  hm->items[index] = item;
  hm->count++;
}

void hm_delete(DzHashmap hm, const char *key) {
  DZ_ASSERT(hm, "Caller must supply a hashmap");
  DZ_ASSERT(key, "Caller must supply a key");
  if (!hm || !key) {
    return;
  }
  size_t index = hm_internal_get_index_hash(key, hm->capacity, 0);
  DzHashmapItem *current_item = hm->items[index];
  size_t i = 1;
  while (current_item != NULL) {
    if (str_eq(current_item->key, key, strlen(current_item->key))) {
      hm_item_free(current_item);
      hm->items[index] = &DELETED_ITEM;
      hm->count--;
      break;
    }
    index = hm_internal_get_index_hash(key, hm->capacity, i);
    current_item = hm->items[index];
    i++;
  }
}

size_t hm_count(DzHashmap hm) {
  DZ_ASSERT(hm, "Caller must supply a hashmap");
  if (!hm) {
    return 0;
  }
  return hm->count;
}
