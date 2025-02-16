#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

extern const size_t HM_INIT_CAPACITY;
typedef struct HM_Inst *HashMap;

typedef enum HM_ERROR {
  HM_ERROR_None,
  HM_ERROR_Memory,
  HM_ERROR_Count
} HM_ERROR;

const char *hm_error_get_failure_str(enum HM_ERROR error_enum);
bool hm_has_error(HM_ERROR *error_ref);

HashMap hm_init(HM_ERROR *error);
void hm_free(HashMap hm);
const char *hm_get(HashMap hm, const char *key);
void hm_add(HashMap hm, const char *key, const char *value,
            HM_ERROR *error);
void hm_delete(HashMap hm, const char *item);
size_t hm_count(HashMap hm);
