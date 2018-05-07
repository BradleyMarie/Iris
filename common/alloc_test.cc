/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    alloc_test.cc

Abstract:

    Unit tests for alloc.h

--*/

extern "C" {
#include "common/alloc.h"
}

#include "googletest/include/gtest/gtest.h"

TEST(AllocTest, LayoutNewAllocationOneElement)
{
    size_t alignments[] = { 1 };
    size_t sizes[] = { 32 };
    size_t placements[] = { 100 };
    size_t size, alignment;

    EXPECT_TRUE(LayoutNewAllocation(1, 
                                    sizes,
                                    alignments,
                                    placements,
                                    &size,
                                    &alignment));
    EXPECT_EQ(0u, placements[0]);
    EXPECT_EQ(32u, size);
    EXPECT_EQ(1u, alignment);
}

TEST(AllocTest, LayoutNewAllocationSucceedsHeaderMoreAlignedNoPaddingNeeded)
{
    size_t alignments[] = { 8, 16 };
    size_t sizes[] = { 64, 32 };
    size_t placements[] = { 0, 0 };
    size_t size, alignment;

    EXPECT_TRUE(LayoutNewAllocation(2, 
                                    sizes,
                                    alignments,
                                    placements,
                                    &size,
                                    &alignment));
    EXPECT_EQ(0u, placements[0]);
    EXPECT_EQ(64u, placements[1]);
    EXPECT_EQ(96u, size);
    EXPECT_EQ(16u, alignment);
}

TEST(AllocTest, LayoutNewAllocationSucceedsHeaderLessAlignedNoPaddingNeeded)
{
    size_t alignments[] = { 8, 16 };
    size_t sizes[] = { 16, 128 };
    size_t placements[] = { 0, 0 };
    size_t size, alignment;

    EXPECT_TRUE(LayoutNewAllocation(2, 
                                    sizes,
                                    alignments,
                                    placements,
                                    &size,
                                    &alignment));
    EXPECT_EQ(0u, placements[0]);
    EXPECT_EQ(16u, placements[1]);
    EXPECT_EQ(144u, size);
    EXPECT_EQ(16u, alignment);
}

TEST(AllocTest, LayoutNewAllocationSucceedsHeaderMoreAlignedPaddingNeeded)
{
    size_t alignments[] = { 8, 32 };
    size_t sizes[] = { 48, 32 };
    size_t placements[] = { 0, 0 };
    size_t size, alignment;

    EXPECT_TRUE(LayoutNewAllocation(2, 
                                    sizes,
                                    alignments,
                                    placements,
                                    &size,
                                    &alignment));
    EXPECT_EQ(0u, placements[0]);
    EXPECT_EQ(64u, placements[1]);
    EXPECT_EQ(96u, size);
    EXPECT_EQ(32u, alignment);
}

TEST(AllocTest, LayoutNewAllocationSucceedsThreeElementsPaddingNeeded)
{
    size_t alignments[] = { 16, 8, 64 };
    size_t sizes[] = { 48, 32, 128 };
    size_t placements[] = { 0, 0, 0 };
    size_t size, alignment;

    EXPECT_TRUE(LayoutNewAllocation(3, 
                                    sizes,
                                    alignments,
                                    placements,
                                    &size,
                                    &alignment));
    EXPECT_EQ(0u, placements[0]);
    EXPECT_EQ(48u, placements[1]);
    EXPECT_EQ(128u, placements[2]);
    EXPECT_EQ(256u, size);
    EXPECT_EQ(64u, alignment);
}

TEST(AllocTest, LayoutNewAllocationFailsTooBig)
{
    size_t alignments[] = { 1, 2 };
    size_t sizes[] = { SIZE_MAX, 2 };
    size_t placements[] = { 0, 0 };
    size_t size, alignment;

    EXPECT_FALSE(LayoutNewAllocation(2, 
                                     sizes,
                                     alignments,
                                     placements,
                                     &size,
                                     &alignment));
}

TEST(AllocTest, LayoutAllocationOneElementSucceeds)
{
    uintptr_t original_allocation = 4096;
    uintptr_t original_allocation_size = 4096;
    size_t alignments[] = { 1 };
    size_t sizes[] = { 32 };
    size_t placements[] = { 0 };

    EXPECT_TRUE(LayoutAllocation(original_allocation,
                                 original_allocation_size,
                                 1, 
                                 sizes,
                                 alignments,
                                 placements));
    EXPECT_EQ(4096u, placements[0]);
}

TEST(AllocTest, LayoutAllocationOneElementFailsTooSmall)
{
    uintptr_t original_allocation = 4096;
    uintptr_t original_allocation_size = 4096;
    size_t alignments[] = { 1 };
    size_t sizes[] = { 4097 };
    size_t placements[] = { 0 };

    EXPECT_FALSE(LayoutAllocation(original_allocation,
                                  original_allocation_size,
                                  1, 
                                  sizes,
                                  alignments,
                                  placements));
}

TEST(AllocTest, LayoutAllocationOneElementFailsMisaligned)
{
    uintptr_t original_allocation = 17;
    uintptr_t original_allocation_size = 4096;
    size_t alignments[] = { 16 };
    size_t sizes[] = { 4096 };
    size_t placements[] = { 0 };

    EXPECT_FALSE(LayoutAllocation(original_allocation,
                                  original_allocation_size,
                                  1, 
                                  sizes,
                                  alignments,
                                  placements));
}

TEST(AllocTest, LayoutAllocationTwoElementsSucceeds)
{
    uintptr_t original_allocation = 4096;
    uintptr_t original_allocation_size = 4096;
    size_t alignments[] = { 16, 32 };
    size_t sizes[] = { 48, 128 };
    size_t placements[] = { 0, 0 };

    EXPECT_TRUE(LayoutAllocation(original_allocation,
                                 original_allocation_size,
                                 2, 
                                 sizes,
                                 alignments,
                                 placements));
    EXPECT_EQ(4096u, placements[0]);
    EXPECT_EQ(4160u, placements[1]);
}

TEST(AllocTest, LayoutAllocationTwoElementsSucceedsWithPadding)
{
    uintptr_t original_allocation = 8;
    uintptr_t original_allocation_size = 4096;
    size_t alignments[] = { 8, 32 };
    size_t sizes[] = { 40, 128 };
    size_t placements[] = { 0, 0 };

    EXPECT_TRUE(LayoutAllocation(original_allocation,
                                 original_allocation_size,
                                 2, 
                                 sizes,
                                 alignments,
                                 placements));
    EXPECT_EQ(8u, placements[0]);
    EXPECT_EQ(64u, placements[1]);
}

TEST(AllocTest, LayoutAllocationTwoElementsFailsTooSmall)
{
    uintptr_t original_allocation = 4096;
    uintptr_t original_allocation_size = 128;
    size_t alignments[] = { 16, 32 };
    size_t sizes[] = { 48, 128 };
    size_t placements[] = { 0, 0 };

    EXPECT_FALSE(LayoutAllocation(original_allocation,
                                  original_allocation_size,
                                  2, 
                                  sizes,
                                  alignments,
                                  placements));
}

TEST(AllocTest, LayoutAllocationThreeElementsSucceeds)
{
    uintptr_t original_allocation = 16;
    uintptr_t original_allocation_size = 4096;
    size_t alignments[] = { 16, 32, 8 };
    size_t sizes[] = { 48, 128, 8 };
    size_t placements[] = { 0, 0, 0 };

    EXPECT_TRUE(LayoutAllocation(original_allocation,
                                 original_allocation_size,
                                 3, 
                                 sizes,
                                 alignments,
                                 placements));
    EXPECT_EQ(16u, placements[0]);
    EXPECT_EQ(64u, placements[1]);
    EXPECT_EQ(192u, placements[2]);
}

TEST(AllocTest, LayoutAllocationThreeElementsSucceedsWithPadding)
{
    uintptr_t original_allocation = 8;
    uintptr_t original_allocation_size = 4096;
    size_t alignments[] = { 8, 32, 128 };
    size_t sizes[] = { 40, 128, 256 };
    size_t placements[] = { 0, 0, 0 };

    EXPECT_TRUE(LayoutAllocation(original_allocation,
                                 original_allocation_size,
                                 3, 
                                 sizes,
                                 alignments,
                                 placements));
    EXPECT_EQ(8u, placements[0]);
    EXPECT_EQ(64u, placements[1]);
    EXPECT_EQ(256u, placements[2]);
}

TEST(AllocTest, LayoutAllocationThreeElementsFailsTooSmall)
{
    uintptr_t original_allocation = 4096;
    uintptr_t original_allocation_size = 256;
    size_t alignments[] = { 8, 32, 128 };
    size_t sizes[] = { 40, 128, 256 };
    size_t placements[] = { 0, 0, 0 };

    EXPECT_FALSE(LayoutAllocation(original_allocation,
                                  original_allocation_size,
                                  3, 
                                  sizes,
                                  alignments,
                                  placements));
}

TEST(AllocTest, AlignedAllocWithHeaderNoData)
{
    size_t header_size = 16;
    size_t header_alignment = 8;
    void *header;
    size_t data_size = 0;
    size_t data_alignment = 0;
    void *data;
    size_t actual_allocation_size;

    ASSERT_TRUE(AlignedAllocWithHeader(header_size,
                                       header_alignment,
                                       &header,
                                       data_size,
                                       data_alignment,
                                       &data,
                                       &actual_allocation_size));
    EXPECT_TRUE(header != NULL);
    EXPECT_TRUE((uintptr_t)header % header_alignment == 0);
    EXPECT_TRUE(data == NULL);
    EXPECT_LE(16u, actual_allocation_size);

    free(header);
    
    ASSERT_TRUE(AlignedAllocWithHeader(header_size,
                                       header_alignment,
                                       &header,
                                       data_size,
                                       data_alignment,
                                       NULL,
                                       NULL));
    EXPECT_TRUE(header != NULL);
    EXPECT_TRUE((uintptr_t)header % header_alignment == 0);

    free(header);

    ASSERT_TRUE(AlignedAllocWithHeader(header_size,
                                       header_alignment,
                                       &header,
                                       data_size,
                                       data_alignment,
                                       NULL,
                                       &actual_allocation_size));
    EXPECT_TRUE(header != NULL);
    EXPECT_TRUE((uintptr_t)header % header_alignment == 0);
    EXPECT_LE(16u, actual_allocation_size);

    free(header);

    ASSERT_TRUE(AlignedAllocWithHeader(header_size,
                                       header_alignment,
                                       &header,
                                       data_size,
                                       data_alignment,
                                       &data,
                                       NULL));
    EXPECT_TRUE(header != NULL);
    EXPECT_TRUE((uintptr_t)header % header_alignment == 0);
    EXPECT_TRUE(data == NULL);

    free(header);
}

TEST(AllocTest, AlignedAllocWithHeaderWithData)
{
    size_t header_size = 16;
    size_t header_alignment = 8;
    void *header;
    size_t data_size = 128;
    size_t data_alignment = 16;
    void *data;
    size_t actual_allocation_size;

    ASSERT_TRUE(AlignedAllocWithHeader(header_size,
                                       header_alignment,
                                       &header,
                                       data_size,
                                       data_alignment,
                                       &data,
                                       &actual_allocation_size));
    EXPECT_TRUE(header != NULL);
    EXPECT_TRUE((uintptr_t)header % header_alignment == 0);
    EXPECT_TRUE(data != NULL);
    EXPECT_TRUE((uintptr_t)data % data_alignment == 0);
    EXPECT_LE(header_size, (uintptr_t)data - (uintptr_t)header);
    EXPECT_LE(144u, actual_allocation_size);

    free(header);

    ASSERT_TRUE(AlignedAllocWithHeader(header_size,
                                       header_alignment,
                                       &header,
                                       data_size,
                                       data_alignment,
                                       &data,
                                       NULL));
    EXPECT_TRUE(header != NULL);
    EXPECT_TRUE((uintptr_t)header % header_alignment == 0);
    EXPECT_TRUE(data != NULL);
    EXPECT_TRUE((uintptr_t)data % data_alignment == 0);
    EXPECT_LE(header_size, (uintptr_t)data - (uintptr_t)header);

    free(header);
}

TEST(AllocTest, AlignedAllocWithHeaderWithDataFails)
{
    size_t header_size = 16;
    size_t header_alignment = 8;
    void *header;
    size_t data_size = SIZE_MAX;
    size_t data_alignment = 1;
    void *data;
    size_t actual_allocation_size;

    ASSERT_FALSE(AlignedAllocWithHeader(header_size,
                                        header_alignment,
                                        &header,
                                        data_size,
                                        data_alignment,
                                        &data,
                                        &actual_allocation_size));

    ASSERT_FALSE(AlignedAllocWithHeader(data_size,
                                        data_alignment,
                                        &data,
                                        header_size,
                                        header_alignment,
                                        &header,
                                        &actual_allocation_size));
}

TEST(AllocTest, AlignedAllocWithTwoHeadersNoData)
{
    size_t first_header_size = 16;
    size_t first_header_alignment = 8;
    void *first_header;
    size_t second_header_size = 32;
    size_t second_header_alignment = 16;
    void *second_header;
    size_t data_size = 0;
    size_t data_alignment = 0;
    void *data;
    size_t actual_allocation_size;

    ASSERT_TRUE(AlignedAllocWithTwoHeaders(first_header_size,
                                           first_header_alignment,
                                           &first_header,
                                           second_header_size,
                                           second_header_alignment,
                                           &second_header,
                                           data_size,
                                           data_alignment,
                                           &data,
                                           &actual_allocation_size));
    EXPECT_TRUE(first_header != NULL);
    EXPECT_TRUE((uintptr_t)first_header % first_header_alignment == 0);
    EXPECT_LE(first_header_size,
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);
    EXPECT_TRUE(data == NULL);
    EXPECT_LE(48u, actual_allocation_size);

    free(first_header);
    
    ASSERT_TRUE(AlignedAllocWithTwoHeaders(first_header_size,
                                           first_header_alignment,
                                           &first_header,
                                           second_header_size,
                                           second_header_alignment,
                                           &second_header,
                                           data_size,
                                           data_alignment,
                                           NULL,
                                           NULL));
    EXPECT_TRUE(first_header != NULL);
    EXPECT_TRUE((uintptr_t)first_header % first_header_alignment == 0);
    EXPECT_LE(first_header_size, 
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);

    free(first_header);

    ASSERT_TRUE(AlignedAllocWithTwoHeaders(first_header_size,
                                           first_header_alignment,
                                           &first_header,
                                           second_header_size,
                                           second_header_alignment,
                                           &second_header,
                                           data_size,
                                           data_alignment,
                                           NULL,
                                           &actual_allocation_size));
    EXPECT_TRUE(first_header != NULL);
    EXPECT_TRUE((uintptr_t)first_header % first_header_alignment == 0);
    EXPECT_LE(first_header_size, 
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);
    EXPECT_LE(48u, actual_allocation_size);

    free(first_header);

    ASSERT_TRUE(AlignedAllocWithTwoHeaders(first_header_size,
                                           first_header_alignment,
                                           &first_header,
                                           second_header_size,
                                           second_header_alignment,
                                           &second_header,
                                           data_size,
                                           data_alignment,
                                           &data,
                                           NULL));
    EXPECT_TRUE(first_header != NULL);
    EXPECT_TRUE((uintptr_t)first_header % first_header_alignment == 0);
    EXPECT_LE(first_header_size, 
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);
    EXPECT_TRUE(data == NULL);

    free(first_header);
}

TEST(AllocTest, AlignedAllocWithTwoHeadersWithData)
{
    size_t first_header_size = 16;
    size_t first_header_alignment = 8;
    void *first_header;
    size_t second_header_size = 32;
    size_t second_header_alignment = 16;
    void *second_header;
    size_t data_size = 128;
    size_t data_alignment = 16;
    void *data;
    size_t actual_allocation_size;

    ASSERT_TRUE(AlignedAllocWithTwoHeaders(first_header_size,
                                           first_header_alignment,
                                           &first_header,
                                           second_header_size,
                                           second_header_alignment,
                                           &second_header,
                                           data_size,
                                           data_alignment,
                                           &data,
                                           &actual_allocation_size));
    EXPECT_TRUE(first_header != NULL);
    EXPECT_TRUE((uintptr_t)first_header % first_header_alignment == 0);
    EXPECT_LE(first_header_size, 
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);
    EXPECT_TRUE(data != NULL);
    EXPECT_TRUE((uintptr_t)data % data_alignment == 0);
    EXPECT_LE(second_header_size, (uintptr_t)data - (uintptr_t)second_header);
    EXPECT_LE(176u, actual_allocation_size);

    free(first_header);

    ASSERT_TRUE(AlignedAllocWithTwoHeaders(first_header_size,
                                           first_header_alignment,
                                           &first_header,
                                           second_header_size,
                                           second_header_alignment,
                                           &second_header,
                                           data_size,
                                           data_alignment,
                                           &data,
                                           NULL));
    EXPECT_TRUE(first_header != NULL);
    EXPECT_TRUE((uintptr_t)first_header % first_header_alignment == 0);
    EXPECT_LE(first_header_size, 
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);
    EXPECT_TRUE(data != NULL);
    EXPECT_TRUE((uintptr_t)data % data_alignment == 0);
    EXPECT_LE(second_header_size, (uintptr_t)data - (uintptr_t)second_header);

    free(first_header);
}

TEST(AllocTest, AlignedAllocWithTwoHeadersWithoutDataFails)
{
    size_t first_header_size = SIZE_MAX;
    size_t first_header_alignment = 1;
    void *first_header;
    size_t second_header_size = 32;
    size_t second_header_alignment = 16;
    void *second_header;
    size_t data_size = 0;
    size_t data_alignment = 0;
    void *data;
    size_t actual_allocation_size;

    ASSERT_FALSE(AlignedAllocWithTwoHeaders(first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            data_size,
                                            data_alignment,
                                            &data,
                                            &actual_allocation_size));
                                            
    ASSERT_FALSE(AlignedAllocWithTwoHeaders(second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            data_size,
                                            data_alignment,
                                            &data,
                                            &actual_allocation_size));
}

TEST(AllocTest, AlignedAllocWithTwoHeadersWithDataFails)
{
    size_t first_header_size = SIZE_MAX;
    size_t first_header_alignment = 1;
    void *first_header;
    size_t second_header_size = 32;
    size_t second_header_alignment = 16;
    void *second_header;
    size_t data_size = 128;
    size_t data_alignment = 16;
    void *data;
    size_t actual_allocation_size;

    ASSERT_FALSE(AlignedAllocWithTwoHeaders(first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            data_size,
                                            data_alignment,
                                            &data,
                                            &actual_allocation_size));

    ASSERT_FALSE(AlignedAllocWithTwoHeaders(second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            data_size,
                                            data_alignment,
                                            &data,
                                            &actual_allocation_size));

    ASSERT_FALSE(AlignedAllocWithTwoHeaders(second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            data_size,
                                            data_alignment,
                                            &data,
                                            first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            &actual_allocation_size));
}

TEST(AllocTest, AlignedResizeWithTwoHeadersNoData)
{
    size_t first_header_size = 16;
    size_t first_header_alignment = 8;
    void *first_header;
    size_t second_header_size = 32;
    size_t second_header_alignment = 16;
    void *second_header;
    size_t data_size = 0;
    size_t data_alignment = 0;
    void *data;
    size_t actual_allocation_size;

    ASSERT_TRUE(AlignedAllocWithTwoHeaders(first_header_size,
                                           first_header_alignment,
                                           &first_header,
                                           second_header_size,
                                           second_header_alignment,
                                           &second_header,
                                           data_size,
                                           data_alignment,
                                           &data,
                                           &actual_allocation_size));
    void *original_first_header = first_header;
    size_t original_allocation_size = actual_allocation_size;

    ASSERT_TRUE(AlignedResizeWithTwoHeaders(original_first_header,
                                            original_allocation_size,
                                            first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            data_size,
                                            data_alignment,
                                            &data,
                                            &actual_allocation_size));
    EXPECT_EQ(original_first_header, first_header);
    EXPECT_LE(first_header_size,
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);
    EXPECT_TRUE(data == NULL);
    EXPECT_EQ(original_allocation_size, actual_allocation_size);
    
    ASSERT_TRUE(AlignedResizeWithTwoHeaders(original_first_header,
                                            original_allocation_size,
                                            first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            data_size,
                                            data_alignment,
                                            NULL,
                                            NULL));
    EXPECT_EQ(original_first_header, first_header);
    EXPECT_LE(first_header_size,
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);

    ASSERT_TRUE(AlignedResizeWithTwoHeaders(original_first_header,
                                            original_allocation_size,
                                            first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            data_size,
                                            data_alignment,
                                            NULL,
                                            &actual_allocation_size));
    EXPECT_EQ(original_first_header, first_header);
    EXPECT_LE(first_header_size,
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);
    EXPECT_EQ(original_allocation_size, actual_allocation_size);

    ASSERT_TRUE(AlignedResizeWithTwoHeaders(original_first_header,
                                            original_allocation_size,
                                            first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            data_size,
                                            data_alignment,
                                            &data,
                                            NULL));
    EXPECT_EQ(original_first_header, first_header);
    EXPECT_LE(first_header_size,
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);
    EXPECT_TRUE(data == NULL);

    free(original_first_header);
}

TEST(AllocTest, AlignedResizeWithTwoHeadersWithData)
{
    size_t first_header_size = 16;
    size_t first_header_alignment = 8;
    void *first_header;
    size_t second_header_size = 32;
    size_t second_header_alignment = 16;
    void *second_header;
    size_t data_size = 128;
    size_t data_alignment = 4;
    void *data;
    size_t actual_allocation_size;

    ASSERT_TRUE(AlignedAllocWithTwoHeaders(first_header_size,
                                           first_header_alignment,
                                           &first_header,
                                           second_header_size,
                                           second_header_alignment,
                                           &second_header,
                                           data_size,
                                           data_alignment,
                                           &data,
                                           &actual_allocation_size));
    void *original_first_header = first_header;
    size_t original_allocation_size = actual_allocation_size;

    ASSERT_TRUE(AlignedResizeWithTwoHeaders(original_first_header,
                                            original_allocation_size,
                                            first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            data_size,
                                            data_alignment,
                                            &data,
                                            &actual_allocation_size));
    EXPECT_EQ(original_first_header, first_header);
    EXPECT_LE(first_header_size,
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);
    EXPECT_TRUE(data != NULL);
    EXPECT_TRUE((uintptr_t)data % data_alignment == 0);
    EXPECT_LE(second_header_size, (uintptr_t)data - (uintptr_t)second_header);
    EXPECT_EQ(original_allocation_size, actual_allocation_size);

    ASSERT_TRUE(AlignedResizeWithTwoHeaders(original_first_header,
                                            original_allocation_size,
                                            first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            data_size,
                                            data_alignment,
                                            &data,
                                            NULL));
    EXPECT_EQ(original_first_header, first_header);
    EXPECT_LE(first_header_size,
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);
    EXPECT_TRUE(data != NULL);
    EXPECT_TRUE((uintptr_t)data % data_alignment == 0);
    EXPECT_LE(second_header_size, (uintptr_t)data - (uintptr_t)second_header);

    free(original_first_header);
}

TEST(AllocTest, AlignedResizeWithTwoHeadersResizeNeeded)
{
    size_t first_header_size = 16;
    size_t first_header_alignment = 8;
    void *first_header;
    size_t second_header_size = 32;
    size_t second_header_alignment = 16;
    void *second_header;
    size_t data_size = 128;
    size_t data_alignment = 4;
    void *data;
    size_t actual_allocation_size;

    ASSERT_TRUE(AlignedAllocWithTwoHeaders(first_header_size,
                                           first_header_alignment,
                                           &first_header,
                                           second_header_size,
                                           second_header_alignment,
                                           &second_header,
                                           data_size,
                                           data_alignment,
                                           &data,
                                           &actual_allocation_size));

    data_size = 512;
    data_alignment = 16;

    ASSERT_TRUE(AlignedResizeWithTwoHeaders(first_header,
                                            actual_allocation_size,
                                            first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            data_size,
                                            data_alignment,
                                            &data,
                                            &actual_allocation_size));
    EXPECT_TRUE(first_header != NULL);
    EXPECT_TRUE((uintptr_t)first_header % first_header_alignment == 0);
    EXPECT_LE(first_header_size,
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);
    EXPECT_TRUE(data != NULL);
    EXPECT_TRUE((uintptr_t)data % data_alignment == 0);
    EXPECT_LE(second_header_size, (uintptr_t)data - (uintptr_t)second_header);
    EXPECT_LE(560u, actual_allocation_size);

    data_size = 0;
    first_header_size = 1024;

    ASSERT_TRUE(AlignedResizeWithTwoHeaders(first_header,
                                            actual_allocation_size,
                                            first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            data_size,
                                            data_alignment,
                                            &data,
                                            &actual_allocation_size));
    EXPECT_TRUE(first_header != NULL);
    EXPECT_TRUE((uintptr_t)first_header % first_header_alignment == 0);
    EXPECT_LE(first_header_size,
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);
    EXPECT_TRUE(data == NULL);
    EXPECT_LE(1056u, actual_allocation_size);

    first_header_size = 32;
    second_header_size = 2048;

    ASSERT_TRUE(AlignedResizeWithTwoHeaders(first_header,
                                            actual_allocation_size,
                                            first_header_size,
                                            first_header_alignment,
                                            &first_header,
                                            second_header_size,
                                            second_header_alignment,
                                            &second_header,
                                            data_size,
                                            data_alignment,
                                            &data,
                                            &actual_allocation_size));
    EXPECT_TRUE(first_header != NULL);
    EXPECT_TRUE((uintptr_t)first_header % first_header_alignment == 0);
    EXPECT_LE(first_header_size,
              (uintptr_t)second_header - (uintptr_t)first_header);
    EXPECT_TRUE(second_header != NULL);
    EXPECT_TRUE((uintptr_t)second_header % second_header_alignment == 0);
    EXPECT_TRUE(data == NULL);
    EXPECT_LE(2080u, actual_allocation_size);

    free(first_header);
}

TEST(AllocTest, AlignedResizeWithTwoHeadersWithoutDataFails)
{
    size_t first_header_size = 16;
    size_t first_header_alignment = 1;
    void *first_header;
    size_t second_header_size = 32;
    size_t second_header_alignment = 16;
    void *second_header;
    size_t data_size = 0;
    size_t data_alignment = 0;
    void *data;
    size_t actual_allocation_size;

    ASSERT_TRUE(AlignedAllocWithTwoHeaders(first_header_size,
                                           first_header_alignment,
                                           &first_header,
                                           second_header_size,
                                           second_header_alignment,
                                           &second_header,
                                           data_size,
                                           data_alignment,
                                           &data,
                                           &actual_allocation_size));
    void* original_header = first_header;

    first_header_size = SIZE_MAX;
    ASSERT_FALSE(AlignedResizeWithTwoHeaders(first_header,
                                             actual_allocation_size,
                                             second_header_size,
                                             second_header_alignment,
                                             &second_header,
                                             first_header_size,
                                             first_header_alignment,
                                             &first_header,
                                             data_size,
                                             data_alignment,
                                             &data,
                                             &actual_allocation_size));
    ASSERT_EQ(original_header, first_header);

    ASSERT_FALSE(AlignedResizeWithTwoHeaders(first_header,
                                             actual_allocation_size,
                                             second_header_size,
                                             second_header_alignment,
                                             &second_header,
                                             first_header_size,
                                             first_header_alignment,
                                             &first_header,
                                             data_size,
                                             data_alignment,
                                             &data,
                                             &actual_allocation_size));
    ASSERT_EQ(original_header, first_header);

    free(original_header);
}

TEST(AllocTest, AlignedResizeWithTwoHeadersWithDataFails)
{
    size_t first_header_size = 16;
    size_t first_header_alignment = 1;
    void *first_header;
    size_t second_header_size = 32;
    size_t second_header_alignment = 16;
    void *second_header;
    size_t data_size = 128;
    size_t data_alignment = 16;
    void *data;
    size_t actual_allocation_size;

    ASSERT_TRUE(AlignedAllocWithTwoHeaders(first_header_size,
                                           first_header_alignment,
                                           &first_header,
                                           second_header_size,
                                           second_header_alignment,
                                           &second_header,
                                           data_size,
                                           data_alignment,
                                           &data,
                                           &actual_allocation_size));
    void* original_header = first_header;

    first_header_size = SIZE_MAX;
    ASSERT_FALSE(AlignedResizeWithTwoHeaders(first_header,
                                             actual_allocation_size,
                                             first_header_size,
                                             first_header_alignment,
                                             &first_header,
                                             second_header_size,
                                             second_header_alignment,
                                             &second_header,
                                             data_size,
                                             data_alignment,
                                             &data,
                                             &actual_allocation_size));
    ASSERT_EQ(original_header, first_header);

    ASSERT_FALSE(AlignedResizeWithTwoHeaders(first_header,
                                             actual_allocation_size,
                                             second_header_size,
                                             second_header_alignment,
                                             &second_header,
                                             first_header_size,
                                             first_header_alignment,
                                             &first_header,
                                             data_size,
                                             data_alignment,
                                             &data,
                                             &actual_allocation_size));
    ASSERT_EQ(original_header, first_header);

    ASSERT_FALSE(AlignedResizeWithTwoHeaders(first_header,
                                             actual_allocation_size,
                                             second_header_size,
                                             second_header_alignment,
                                             &second_header,
                                             data_size,
                                             data_alignment,
                                             &data,
                                             first_header_size,
                                             first_header_alignment,
                                             &first_header,
                                             &actual_allocation_size));
    ASSERT_EQ(original_header, first_header);

    free(original_header);
}