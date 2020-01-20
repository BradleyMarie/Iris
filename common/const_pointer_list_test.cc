/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    const_pointer_list_test.cc

Abstract:

    Unit tests for const_pointer_list.h

--*/

extern "C" {
#include "common/const_pointer_list.h"
}

#include "googletest/include/gtest/gtest.h"

TEST(ConstPointerListTest, ConstantPointerListInitializeAndDestroy)
{
    CONSTANT_POINTER_LIST list;
    list.list = NULL;
    list.capacity = 0;
    list.size = 0;

    ASSERT_TRUE(ConstantPointerListInitialize(&list));
    EXPECT_TRUE(list.list != NULL);
    EXPECT_NE(0u, list.capacity);
    EXPECT_EQ(0u, list.size);

    ConstantPointerListDestroy(&list);
    EXPECT_TRUE(list.list == NULL);
    EXPECT_EQ(0u, list.capacity);
    EXPECT_EQ(0u, list.size);
}

TEST(ConstPointerListTest, ConstantPointerListAddAndRetrieve)
{
    CONSTANT_POINTER_LIST list;
    ASSERT_TRUE(ConstantPointerListInitialize(&list));

    for (size_t i = 0; i < 1000; i++) {
        ASSERT_TRUE(ConstantPointerListAddPointer(&list, (void *)(uintptr_t)i));
    }

    ASSERT_EQ(1000u, ConstantPointerListGetSize(&list));

    for (size_t i = 0; i < 1000; i++) {
        const void *ptr = ConstantPointerListRetrieveAtIndex(&list, i);
        EXPECT_EQ(i, (size_t)(uintptr_t)ptr);
    }

    ConstantPointerListDestroy(&list);
}

TEST(ConstPointerListTest, ConstantPointerListClear)
{
    CONSTANT_POINTER_LIST list;
    ASSERT_TRUE(ConstantPointerListInitialize(&list));

    for (size_t i = 0; i < 1000; i++) {
        ASSERT_TRUE(ConstantPointerListAddPointer(&list, (void *)(uintptr_t)i));
    }

    ASSERT_EQ(1000u, ConstantPointerListGetSize(&list));

    ConstantPointerListClear(&list);
    ASSERT_EQ(0u, ConstantPointerListGetSize(&list));

    ConstantPointerListDestroy(&list);
}

TEST(ConstPointerListTest, ConstantPointerListGetData)
{
    CONSTANT_POINTER_LIST list;
    ASSERT_TRUE(ConstantPointerListInitialize(&list));

    for (size_t i = 0; i < 1000; i++) {
        ASSERT_TRUE(ConstantPointerListAddPointer(&list, (void *)(uintptr_t)i));
    }

    ASSERT_EQ(1000u, ConstantPointerListGetSize(&list));

    const void** data;
    size_t size;
    ConstantPointerListGetData(&list, &data, &size);
    ASSERT_EQ(1000u, size);
    ASSERT_EQ(list.list, data);

    ConstantPointerListDestroy(&list);
}

int Reverse(const void* left, const void* right) {
    const void **left_entry = (const void **)left;
    const void **right_entry = (const void **)right;

    if ((uintptr_t)*left_entry > (uintptr_t)*right_entry) {
        return -1;
    }

    return 1;
}

TEST(ConstPointerListTest, ConstantPointerListSort)
{
    CONSTANT_POINTER_LIST list;
    ASSERT_TRUE(ConstantPointerListInitialize(&list));

    for (size_t i = 0; i < 1000; i++) {
        ASSERT_TRUE(ConstantPointerListAddPointer(&list, (void *)(uintptr_t)i));
    }

    ConstantPointerListSort(&list, Reverse);

    for (size_t i = 0; i < 1000; i++) {
        const void *ptr = ConstantPointerListRetrieveAtIndex(&list, 999 - i);
        EXPECT_EQ(i, (size_t)(uintptr_t)ptr);
    }

    ConstantPointerListDestroy(&list);
}