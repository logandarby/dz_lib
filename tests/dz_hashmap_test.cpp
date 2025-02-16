#include <gtest/gtest.h>

#define _TESTING

extern "C"
{
#include "dz_hashmap.c"
#include "prime.c"
}

TEST(HashMap_Item, Initialization)
{
    HM_Item *item = hm_item_create("key1", "value");
    ASSERT_EQ(strcmp(item->key, "key1"), 0);
    ASSERT_EQ(strcmp(item->value, "value"), 0);
    hm_item_free(item);
}

TEST(HashMap, Initialization)
{
    HM_ERROR error = HM_ERROR_None;
    HashMap hm = hm_init(&error);
    ASSERT_EQ(error, HM_ERROR_None);
    ASSERT_TRUE(hm->items);
    ASSERT_EQ(hm->capacity, HM_INIT_CAPACITY);
    ASSERT_EQ(hm->count, 0);
    for (size_t i = 0; i < hm->capacity; i++)
    {
        ASSERT_EQ(hm->items[i], (HM_Item *)NULL);
    }
    hm_free(hm);
}

TEST(HashMap, InsertAndDelete)
{
    HM_ERROR error = HM_ERROR_None;
    HashMap hm = hm_init(&error);
    ASSERT_EQ(error, HM_ERROR_None);
    ASSERT_EQ(hm_count(hm), 0);
    hm_add(hm, "key1", "value1", &error);
    ASSERT_EQ(error, HM_ERROR_None);
    ASSERT_EQ(hm_count(hm), 1);
    const char *value = hm_get(hm, "key1");
    ASSERT_EQ(strcmp(value, "value1"), 0);
    hm_delete(hm, "key1");
    ASSERT_EQ(hm_count(hm), 0);
    const char *valueDeleted = hm_get(hm, "key1");
    ASSERT_EQ(valueDeleted, (char *)NULL);
    hm_free(hm);
}

TEST(HashMap, InvalidSearch)
{
    HM_ERROR error = HM_ERROR_None;
    HashMap hm = hm_init(&error);
    ASSERT_EQ(error, HM_ERROR_None);
    hm_add(hm, "key1", "value1", &error);
    ASSERT_EQ(error, HM_ERROR_None);
    const char *value = hm_get(hm, "INVALIDKEY");
    ASSERT_EQ(value, (char *)NULL);
    hm_free(hm);
}

TEST(HashMap, InvalidDelete)
{
    HM_ERROR error = HM_ERROR_None;
    HashMap hm = hm_init(&error);
    ASSERT_EQ(error, HM_ERROR_None);
    hm_add(hm, "key1", "value1", &error);
    ASSERT_EQ(error, HM_ERROR_None);
    ASSERT_EQ(hm_count(hm), 1);
    hm_delete(hm, "INVALIDKEY");
    ASSERT_EQ(hm_count(hm), 1);
    hm_free(hm);
}

TEST(HashMap, InsertAndUpdate)
{
    HM_ERROR error = HM_ERROR_None;
    HashMap hm = hm_init(&error);
    ASSERT_EQ(error, HM_ERROR_None);
    hm_add(hm, "key1", "value1", &error);
    ASSERT_EQ(error, HM_ERROR_None);
    hm_add(hm, "key1", "value2", &error);
    ASSERT_EQ(error, HM_ERROR_None);
    const char *value = hm_get(hm, "key1");
    ASSERT_EQ(hm_count(hm), 1);
    ASSERT_EQ(strcmp(value, "value2"), 0);
    hm_delete(hm, "key1");
    ASSERT_EQ(hm_count(hm), 0);
    hm_free(hm);
}

TEST(HashMap, Resize)
{
    HM_ERROR error = HM_ERROR_None;
    HashMap hm = hm_init(&error);
    ASSERT_EQ(error, HM_ERROR_None);
    for (size_t i = 0; i < HM_INIT_CAPACITY; i++)
    {
        char str[256];
        snprintf(str, 256, "%zu", i);
        hm_add(hm, str, str, NULL);
    }
    printf("%zu", hm->capacity);
    hm_free(hm);
}

TEST(Hash, Hash)
{
    size_t hash = hm_internal_hash("cat", 151, 53);
    ASSERT_EQ(hash, 5);
}

TEST(Prime, findNextPrime)
{
    ASSERT_TRUE(is_prime(100008317));
    ASSERT_FALSE(is_prime(100008317 + 1));
    ASSERT_TRUE(is_prime(100008317 + 2));
    ASSERT_EQ(prime_find_next(100008317), 100008319);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
