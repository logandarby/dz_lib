#include "dz_hashmap.h"
#include "prime.h"

#include <math.h>
#include <string.h>

typedef struct HM_Item {
  char *key;
  char *value;
} HM_Item;

typedef struct HM_Inst {
  HM_Item **items;
  size_t capacity;
  size_t count;
} HM_Inst;

const size_t HM_INIT_CAPACITY = 53;
static const size_t HM_PRIME_1 = 100008317;
static const size_t HM_PRIME_2 = 100008317 + 2;
static const double HM_RESIZE_UP_THRESHOLD = 0.7;

static HM_Item DELETED_ITEM = {.key = NULL, .value = NULL};

static const char *HM_ERROR_STRINGS[HM_ERROR_Count] = {
    [HM_ERROR_None] = "No error",
    [HM_ERROR_Memory] = "Could not allocate memory",
};

const char *hm_error_get_failure_str(HM_ERROR error_enum) {
  return HM_ERROR_STRINGS[error_enum];
}

static void hm_error_set(HM_ERROR *error_ref, HM_ERROR value) {
  if (error_ref) {
    *error_ref = value;
  }
}

bool hm_has_error(HM_ERROR *error_ref) { return error_ref && *error_ref; }

HashMap hm_init_with_capacity(size_t capacity, HM_ERROR *error) {
  HashMap hm = (HashMap)calloc(1, sizeof(struct HM_Inst));
  if (!hm) {
    hm_error_set(error, HM_ERROR_Memory);
    return NULL;
  }
  hm->count = 0;
  hm->capacity = capacity;
  hm->items = (HM_Item **)calloc(hm->capacity, (sizeof *hm->items));
  if (!hm->items) {
    hm_error_set(error, HM_ERROR_Memory);
    return NULL;
  }
  return hm;
}

HashMap hm_init(HM_ERROR *error) {
  return hm_init_with_capacity(HM_INIT_CAPACITY, error);
}

static void hm_item_free(HM_Item *item) {
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

void hm_free(HashMap hm) {
  if (!hm) {
    return;
  }
  for (size_t i = 0; i < hm->capacity; i++) {
    HM_Item *item = hm->items[i];
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

static HM_Item *hm_item_create(const char *key, const char *value) {
  if (!key || !value) {
    return NULL;
  }
  HM_Item *item = (HM_Item *)calloc(1, sizeof(HM_Item));
  item->key = strdup(key);
  item->value = strdup(value);
  return item;
}

static size_t hm_internal_hash(const char *str, const size_t a,
                               const size_t bucket_count) {
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
  const size_t hash_a = hm_internal_hash(str, HM_PRIME_1, bucket_count);
  const size_t hash_b = hm_internal_hash(str, HM_PRIME_2, bucket_count);
  return (hash_a + (attempt * (hash_b + 1))) % bucket_count;
}

const char *hm_get(HashMap hm, const char *key) {
  if (!hm || !key) {
    return NULL;
  }
  size_t index = hm_internal_get_index_hash(key, hm->capacity, 0);
  HM_Item *current_item = hm->items[index];
  size_t i = 1;
  while (current_item != NULL) {
    if (current_item != &DELETED_ITEM && strcmp(current_item->key, key) == 0) {
      return current_item->value;
    }
    index = hm_internal_get_index_hash(key, hm->capacity, i);
    current_item = hm->items[index];
    i++;
  }
  return NULL;
}

void hm_resize(HashMap hm, const size_t new_size, HM_ERROR *error) {
  if (new_size < HM_INIT_CAPACITY) {
    return;
  }
  hm_error_set(error, HM_ERROR_None);
  HashMap new_hm_buffer = hm_init_with_capacity(new_size, error);
  if (hm_has_error(error)) {
    return;
  }
  for (size_t i = 0; i < hm->count; i++) {
    HM_Item *old_item = hm->items[i];
    if (old_item && old_item != &DELETED_ITEM) {
      hm_add(new_hm_buffer, old_item->key, old_item->value, error);
      if (hm_has_error(error)) {
        hm_free(new_hm_buffer);
        return;
      }
    }
  }
  HM_Item **old_items = hm->items;
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

void hm_add(HashMap hm, const char *key, const char *value, HM_ERROR *error) {
  if (!hm || !key || !value) {
  hm_error_set(error, HM_ERROR_Memory);
    return;
  }
  if (hm->capacity * HM_RESIZE_UP_THRESHOLD <= hm->count) {
    hm_resize(hm, prime_find_next(hm->capacity), error);
    if (hm_has_error(error)) {
      return;
    }
  }
  HM_Item *item = hm_item_create(key, value);
  size_t index = hm_internal_get_index_hash(key, hm->capacity, 0);
  HM_Item *current_item = hm->items[index];
  size_t i = 1;
  while (current_item != NULL && current_item != &DELETED_ITEM) {
    if (strcmp(current_item->key, key) == 0) {
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

void hm_delete(HashMap hm, const char *key) {
  if (!hm || !key) {
    return;
  }
  size_t index = hm_internal_get_index_hash(key, hm->capacity, 0);
  HM_Item *current_item = hm->items[index];
  size_t i = 1;
  while (current_item != NULL) {
    if (strcmp(current_item->key, key) == 0) {
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

size_t hm_count(HashMap hm) {
  if (!hm) {
    return 0;
  }
  return hm->count;
}
