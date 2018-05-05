/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    safe_math_test.cc

Abstract:

    Unit tests for safe_math.h

--*/

extern "C" {
#include "common/safe_math.h"
}

#include "googletest/include/gtest/gtest.h"

TEST(SafeMathTest, CheckedAddSizeTSucceeds)
{
    size_t result;
    ASSERT_TRUE(CheckedAddSizeT(0, 0, &result));
    EXPECT_EQ(0u, result);

    ASSERT_TRUE(CheckedAddSizeT(1234, 2, &result));
    EXPECT_EQ(1236u, result);

    ASSERT_TRUE(CheckedAddSizeT(SIZE_MAX - 1, 1, &result));
    EXPECT_EQ(SIZE_MAX, result);
}

TEST(SafeMathTest, CheckedAddSizeTAddZeroSucceeds)
{
    size_t result;
    ASSERT_TRUE(CheckedAddSizeT(0, 0, &result));
    EXPECT_EQ(0u, result);

    ASSERT_TRUE(CheckedAddSizeT(SIZE_MAX, 0, &result));
    EXPECT_EQ(SIZE_MAX, result);

    ASSERT_TRUE(CheckedAddSizeT(0, SIZE_MAX, &result));
    EXPECT_EQ(SIZE_MAX, result);
}

TEST(SafeMathTest, CheckedAddSizeTFails)
{
    size_t result;
    ASSERT_FALSE(CheckedAddSizeT(SIZE_MAX, 1, &result));
    ASSERT_FALSE(CheckedAddSizeT(1, SIZE_MAX, &result));
    ASSERT_FALSE(CheckedAddSizeT(SIZE_MAX, SIZE_MAX, &result));
}

TEST(SafeMathTest, CheckedMultiplySizeTSucceeds)
{
    size_t result;
    ASSERT_TRUE(CheckedMultiplySizeT(1234, 2, &result));
    EXPECT_EQ(2468u, result);
}

TEST(SafeMathTest, CheckedMultiplySizeTMultiplyZeroSucceeds)
{
    size_t result;
    ASSERT_TRUE(CheckedMultiplySizeT(0, 0, &result));
    EXPECT_EQ(0u, result);

    ASSERT_TRUE(CheckedMultiplySizeT(1, 0, &result));
    EXPECT_EQ(0u, result);

    ASSERT_TRUE(CheckedMultiplySizeT(SIZE_MAX, 0, &result));
    EXPECT_EQ(0u, result);

    ASSERT_TRUE(CheckedMultiplySizeT(0, SIZE_MAX, &result));
    EXPECT_EQ(0u, result);
}

TEST(SafeMathTest, CheckedMultiplySizeTMultiplyOneSucceeds)
{
    size_t result;
    ASSERT_TRUE(CheckedMultiplySizeT(1, 1, &result));
    EXPECT_EQ(1u, result);

    ASSERT_TRUE(CheckedMultiplySizeT(SIZE_MAX, 1, &result));
    EXPECT_EQ(SIZE_MAX, result);

    ASSERT_TRUE(CheckedMultiplySizeT(1, SIZE_MAX, &result));
    EXPECT_EQ(SIZE_MAX, result);
}

TEST(SafeMathTest, CheckedMultiplySizeTFails)
{
    size_t result;
    ASSERT_FALSE(CheckedMultiplySizeT(SIZE_MAX, 2, &result));
    ASSERT_FALSE(CheckedMultiplySizeT(2, SIZE_MAX, &result));
    ASSERT_FALSE(CheckedMultiplySizeT(SIZE_MAX, SIZE_MAX, &result));
}