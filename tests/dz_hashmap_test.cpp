#include <gtest/gtest.h>

#define _TESTING

extern "C"
{
#include "dz_hashmap.c"
}

TEST(DzHashmap_Item, Initialization)
{
  const char*key1 = "key1";
  const char *value = "value";
    DzHashmapItem *item = hm_item_create(key1, value, strlen(key1), strlen(value));
    ASSERT_EQ(memcmp(item->key, key1, strlen(key1)), 0);
    ASSERT_EQ(memcmp(item->value, value, strlen(value)), 0);
    hm_item_free(item);
}

TEST(DzHashmap, Initialization)
{
    DzHmError error = DzHmError_None;
    DzHashmap hm = hm_init(&error);
    ASSERT_EQ(error, DzHmError_None);
    ASSERT_TRUE(hm->items);
    ASSERT_EQ(hm->capacity, HM_INIT_CAPACITY);
    ASSERT_EQ(hm->count, 0);
    for (size_t i = 0; i < hm->capacity; i++)
    {
        ASSERT_EQ(hm->items[i], (DzHashmapItem *)NULL);
    }
    hm_free(hm);
}

TEST(DzHashmap, InsertAndDelete)
{
    DzHmError error = DzHmError_None;
    DzHashmap hm = hm_init(&error);
    ASSERT_EQ(error, DzHmError_None);
    ASSERT_EQ(hm_count(hm), 0);
    hm_add_str(hm, "key1", "value1", &error);
    ASSERT_EQ(error, DzHmError_None);
    ASSERT_EQ(hm_count(hm), 1);
    const char *value = hm_get_str(hm, "key1");
    ASSERT_TRUE(value);
    ASSERT_EQ(strcmp(value, "value1"), 0);
    hm_delete_str(hm, "key1");
    ASSERT_EQ(hm_count(hm), 0);
    const char *valueDeleted = hm_get_str(hm, "key1");
    ASSERT_EQ(valueDeleted, (char *)NULL);
    hm_free(hm);
}

TEST(DzHashmap, InvalidSearch)
{
    DzHmError error = DzHmError_None;
    DzHashmap hm = hm_init(&error);
    ASSERT_EQ(error, DzHmError_None);
    hm_add_str(hm, "key1", "value1", &error);
    ASSERT_EQ(error, DzHmError_None);
    const char *value = hm_get_str(hm, "INVALIDKEY");
    ASSERT_EQ(value, (char *)NULL);
    hm_free(hm);
}

TEST(DzHashmap, InvalidDelete)
{
    DzHmError error = DzHmError_None;
    DzHashmap hm = hm_init(&error);
    ASSERT_EQ(error, DzHmError_None);
    hm_add_str(hm, "key1", "value1", &error);
    ASSERT_EQ(error, DzHmError_None);
    ASSERT_EQ(hm_count(hm), 1);
    hm_delete_str(hm, "INVALIDKEY");
    ASSERT_EQ(hm_count(hm), 1);
    hm_free(hm);
}

TEST(DzHashmap, InsertAndUpdate)
{
    DzHmError error = DzHmError_None;
    DzHashmap hm = hm_init(&error);
    ASSERT_EQ(error, DzHmError_None);
    hm_add_str(hm, "key1", "value1", &error);
    ASSERT_EQ(error, DzHmError_None);
    hm_add_str(hm, "key1", "value2", &error);
    ASSERT_EQ(error, DzHmError_None);
    const char *value = hm_get_str(hm, "key1");
    ASSERT_EQ(hm_count(hm), 1);
    ASSERT_EQ(strcmp(value, "value2"), 0);
    hm_delete_str(hm, "key1");
    ASSERT_EQ(hm_count(hm), 0);
    hm_free(hm);
}

TEST(DzHashmap, Resize)
{
    DzHmError error = DzHmError_None;
    DzHashmap hm = hm_init(&error);
    ASSERT_EQ(error, DzHmError_None);
    for (size_t i = 0; i < HM_INIT_CAPACITY; i++)
    {
        char str[256];
        snprintf(str, 256, "%zu", i);
        hm_add_str(hm, str, str, NULL);
    }
    printf("%zu", hm->capacity);
    hm_free(hm);
}

/*TEST(Hash, Hash)*/
/*{*/
/*    size_t hash = hm_internal_hash("cat", 3, 151, 53);*/
/*    ASSERT_EQ(hash, 5);*/
/*}*/

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
