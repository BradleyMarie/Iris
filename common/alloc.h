/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    alloc.h

Abstract:

    Aligned memory allocation routines.
    
    Note, unlike C's realloc function, AlignedResizeWithTwoHeaders does not 
    copy the old contents if a reallocation is required. It is up to the caller
    to preserve any contents that must survive the resize operation.

--*/

#ifndef _COMMON_ALLOC_
#define _COMMON_ALLOC_

#include <stdlib.h>

#include "common/safe_math.h"

//
// Functions
//

_Check_return_
_Success_(return != 0)
static
inline
bool
LayoutNewAllocation(
    _In_ size_t number_of_elements,
    _In_reads_(number_of_members) size_t element_sizes[],
    _In_reads_(number_of_elements) size_t element_alignments[],
    _Out_writes_(number_of_elements) size_t element_placements[],
    _Out_ size_t *size,
    _Out_ size_t *alignment)
{
    assert(number_of_elements != 0);
    assert(element_sizes != NULL);
    assert(element_alignments != NULL);
    assert(element_placements != NULL);
    assert(size != NULL);
    assert(alignment != NULL);

    assert(element_alignments[0] != 0);
    assert(element_sizes[0] % element_alignments[0] == 0);
    assert((element_alignments[0] & (element_alignments[0] - 1)) == 0);

    element_placements[0] = 0;
    *size = element_sizes[0];
    *alignment = element_alignments[0];

    for (size_t i = 1; i < number_of_elements; i++)
    {
        if (element_sizes[i] == 0)
        {
            continue;
        }

        assert(element_alignments[i] != 0);
        assert(element_sizes[i] % element_alignments[i] == 0);
        assert((element_alignments[i] & (element_alignments[i] - 1)) == 0);

        bool success = CheckedAddSizeT(*size,
                                       element_sizes[i],
                                       size);

        if (!success)
        {
            return false;
        }

        size_t next_placement =
            element_placements[i - 1] + element_sizes[i - 1];

        size_t bytes_past_alignment =
            next_placement & (element_alignments[i] - 1);

        if (bytes_past_alignment != 0)
        {
            size_t last_entry_padding =
                element_alignments[i] - bytes_past_alignment;

            success = CheckedAddSizeT(*size,
                                      last_entry_padding,
                                      size);

            if (!success)
            {
                return false;
            }

            next_placement += last_entry_padding;
        }

        if (*alignment < element_alignments[i])
        {
            *alignment = element_alignments[i];
        }

        element_placements[i] = next_placement;
    }

    return true;
}

_Check_return_
_Success_(return != 0)
static
inline
bool
LayoutAllocation(
    _In_ uintptr_t base_address,
    _In_ size_t allocation_size,
    _In_ size_t number_of_elements,
    _In_reads_(number_of_members) size_t element_sizes[],
    _In_reads_(number_of_elements) size_t element_alignments[],
    _Out_writes_(number_of_elements) uintptr_t element_placements[])
{
    assert(base_address != (uintptr_t)NULL);
    assert(allocation_size != 0);
    assert(number_of_elements != 0);
    assert(element_sizes != NULL);
    assert(element_alignments != NULL);
    assert(element_placements != NULL);

    assert(element_alignments[0] != 0);
    assert(element_sizes[0] % element_alignments[0] == 0);
    assert((element_alignments[0] & (element_alignments[0] - 1)) == 0);

    if ((base_address & (element_alignments[0] - 1)) != 0 ||
        allocation_size < element_sizes[0])
    {
        return false;
    }

    element_placements[0] = base_address;
    size_t size = element_sizes[0];

    for (size_t i = 1; i < number_of_elements; i++)
    {
        if (element_sizes[i] == 0)
        {
            continue;
        }

        assert(element_alignments[i] != 0);
        assert(element_sizes[i] % element_alignments[i] == 0);
        assert((element_alignments[i] & (element_alignments[i] - 1)) == 0);

        bool success = CheckedAddSizeT(size,
                                       element_sizes[i],
                                       &size);

        if (!success)
        {
            return false;
        }

        if (allocation_size < size)
        {
            return false;
        }

        uintptr_t next_placement =
            element_placements[i - 1] + element_sizes[i - 1];

        size_t bytes_past_alignment =
            next_placement & (element_alignments[i] - 1);

        if (bytes_past_alignment != 0)
        {
            size_t last_entry_padding =
                element_alignments[i] - bytes_past_alignment;

            success = CheckedAddSizeT(size,
                                      last_entry_padding,
                                      &size);

            if (!success)
            {
                return false;
            }

            if (allocation_size < size)
            {
                return false;
            }

            next_placement += last_entry_padding;
        }

        element_placements[i] = next_placement;
    }

    return true;
}

_Check_return_
_Success_(return != 0)
static
inline
bool
AlignedAllocWithHeader(
    _In_range_(>, 0) size_t header_size,
    _In_ size_t header_alignment,
    _Outptr_result_bytebuffer_to_(header_size, 0) void **header,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Outptr_result_bytebuffer_to_(data_size, 0) void **data)
{
    assert(header_size != 0);
    assert(header_alignment != 0);
    assert((header_alignment & (header_alignment - 1)) == 0);
    assert(header_size % header_alignment == 0);
    assert(header != NULL);
    assert(data_size == 0 || data_alignment != 0);
    assert(data_size == 0 || (data_alignment & (data_alignment - 1)) == 0);
    assert(data_size == 0 || data_size % data_alignment == 0);
    assert(data != NULL);

    size_t sizes[] = {header_size, data_size};
    size_t alignments[] = {header_alignment, data_alignment};
    size_t placements[2] = { 0, 0 };
    size_t alignment, size;

    bool success = LayoutNewAllocation(2,
                                       sizes,
                                       alignments,
                                       placements,
                                       &size,
                                       &alignment);

    if (!success)
    {
        return false;
    }

    void *allocation = aligned_alloc(alignment, size);

    if (allocation == NULL)
    {
        return false;
    }

    *header = allocation;

    if (data_size != 0)
    {
        *data = (void *)((char *)allocation + placements[1]);
    }
    else
    {
        *data = NULL;
    }

    return true;
}

_Check_return_
_Success_(return != 0)
static
inline
bool
AlignedAllocWithTwoHeaders(
    _In_range_(>, 0) size_t first_header_size,
    _In_ size_t first_header_alignment,
    _Outptr_result_bytebuffer_to_(first_header_size, 0) void **first_header,
    _In_range_(>, 0) size_t second_header_size,
    _In_ size_t second_header_alignment,
    _Outptr_result_bytebuffer_to_(second_header_size, 0) void **second_header,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Outptr_result_bytebuffer_to_(data_size, 0) void **data,
    _Out_ size_t *actual_allocation_size)
{
    assert(first_header_size != 0);
    assert(first_header_alignment != 0);
    assert((first_header_alignment & (first_header_alignment - 1)) == 0);
    assert(first_header_size % first_header_alignment == 0);
    assert(first_header != NULL);
    assert(second_header_size != 0);
    assert(second_header_alignment != 0);
    assert((second_header_alignment & (second_header_alignment - 1)) == 0);
    assert(second_header_size % second_header_alignment == 0);
    assert(second_header != NULL);
    assert(data_size == 0 || data_alignment != 0);
    assert(data_size == 0 || (data_alignment & (data_alignment - 1)) == 0);
    assert(data_size == 0 || data_size % data_alignment == 0);
    assert(data != NULL);
    assert(actual_allocation_size != NULL);

    size_t sizes[] = {first_header_size, second_header_size, data_size};
    size_t alignments[] =
        {first_header_alignment, second_header_alignment, data_alignment};
    size_t placements[3] = { 0, 0, 0 };
    size_t alignment, size;

    bool success = LayoutNewAllocation(3,
                                       sizes,
                                       alignments,
                                       placements,
                                       &size,
                                       &alignment);

    if (!success)
    {
        return false;
    }

    void *allocation = aligned_alloc(alignment, size);

    if (allocation == NULL)
    {
        return false;
    }

    *first_header = allocation;
    *second_header = (void *)((char *)allocation + placements[1]);

    if (data_size != 0)
    {
        *data = (void *)((char *)allocation + placements[2]);
    }
    else
    {
        *data = NULL;
    }

    *actual_allocation_size = size;

    return true;
}

_Check_return_
_Success_(return != 0)
static
inline
bool
AlignedResizeWithTwoHeaders(
    _Pre_writable_byte_size_(original_size) _Post_invalid_ void *original,
    _In_range_(>, 0) size_t original_size,
    _In_range_(>, 0) size_t first_header_size,
    _In_ size_t first_header_alignment,
    _Outptr_result_bytebuffer_(first_header_size) void **first_header,
    _In_range_(>, 0) size_t second_header_size,
    _In_ size_t second_header_alignment,
    _Outptr_result_bytebuffer_(second_header_size) void **second_header,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Outptr_result_bytebuffer_to_(data_size, 0) void **data,
    _Out_ size_t *actual_allocation_size)
{
    assert(original != NULL);
    assert(original_size != 0);
    assert(first_header_size != 0);
    assert(first_header_alignment != 0);
    assert(first_header != NULL);
    assert((first_header_alignment & (first_header_alignment - 1)) == 0);
    assert(first_header_size % first_header_alignment == 0);
    assert(second_header_size != 0);
    assert(second_header_alignment != 0);
    assert(second_header != NULL);
    assert((second_header_alignment & (second_header_alignment - 1)) == 0);
    assert(second_header_size % second_header_alignment == 0);
    assert(data_size == 0 || data_alignment != 0);
    assert(data != NULL);
    assert(data_size == 0 || (data_alignment & (data_alignment - 1)) == 0);
    assert(data_size == 0 || data_size % data_alignment == 0);
    assert(actual_allocation_size != NULL);

    size_t sizes[] = {first_header_size, second_header_size, data_size};
    size_t alignments[] =
        {first_header_alignment, second_header_alignment, data_alignment};
    uintptr_t placements[3] = { 0, 0, 0 };

    bool success = LayoutAllocation((uintptr_t)original,
                                    original_size,
                                    3,
                                    sizes,
                                    alignments,
                                    placements);

    if (success)
    {
        *first_header = (void *)placements[0];
        *second_header = (void *)placements[1];

        if (data_size != 0)
        {
            *data = (void *)placements[2];
        }
        else
        {
            *data = NULL;
        }

        *actual_allocation_size = original_size;

        return true;
    }

    bool result = AlignedAllocWithTwoHeaders(first_header_size,
                                             first_header_alignment,
                                             first_header,
                                             second_header_size,
                                             second_header_alignment,
                                             second_header,
                                             data_size,
                                             data_alignment,
                                             data,
                                             actual_allocation_size);

    if (!result)
    {
        return false;
    }

    free(original);
    return true;
}

#endif // _COMMON_ALLOC_