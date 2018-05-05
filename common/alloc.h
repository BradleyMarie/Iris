/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    alloc.h

Abstract:

    Aligned memory allocation routines.

--*/

#ifndef _IRIS_ALLOC_
#define _IRIS_ALLOC_

#include "common/safe_math.h"

#include <assert.h>
#include <stdint.h>

//
// Functions
//

static
inline
ISTATUS
AlignedAllocWithHeader(
    _In_ _Pre_satisfies_(_Curr_ != 0) size_t header_size,
    _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && header_size % _Curr_ == 0) size_t header_alignment,
    _Outptr_result_bytebuffer_(header_size) void **header,
    _In_ size_t data_size,
    _When_(data_size != 0, _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && data_size % _Curr_ == 0)) size_t data_alignment,
    _When_(data_size != 0, _Out_ _Deref_post_bytecap_(data_size)) _When_(data_size == 0, _Out_opt_ _When_(data != NULL, _Deref_post_null_)) void **data,
    _Out_opt_ size_t *actual_allocation_size
    )
{
    assert(header_size != 0);
    assert(header_alignment != 0);
    assert(header != NULL);
    assert((header_alignment & (header_alignment - 1)) == 0);
    assert(header_size % header_alignment == 0);
    assert(data_size == 0 || data_alignment != 0);
    assert(data_size == 0 || data != NULL);
    assert(data_size == 0 || (data_alignment & (data_alignment - 1)) == 0);
    assert(data_size == 0 || data_size % data_alignment == 0);

    // If no data is requested, simply make an call to aligned_alloc
    if (data_size == 0)
    {
        void *allocation = aligned_alloc(header_alignment, header_size);

        if (allocation == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        *header = allocation;

        if (actual_allocation_size != NULL)
        {
            *actual_allocation_size = header_size;
        }

        if (data != NULL)
        {
            *data = NULL;
        }

        return ISTATUS_SUCCESS;
    }

    // If the data alignment is greater than the header alignment, pad the 
    // header to a multiple of the data alignment and use the data alignment for
    // the allocation. Otherwise, use the header's alignment.
    size_t allocation_alignment;
    if (header_alignment < data_alignment)
    {
        size_t bytes_past_alignment = header_size & (data_alignment - 1);

        if (bytes_past_alignment != 0)
        {
            size_t header_padding = data_alignment - bytes_past_alignment;

            bool success = CheckedAddSizeT(header_size,
                                           header_padding,
                                           &header_size);

            if (!success)
            {
                return ISTATUS_ALLOCATION_FAILED;
            }
        }

        allocation_alignment = data_alignment;
    }
    else
    {
        allocation_alignment = header_alignment;
    }

    // Compute the size of the actual allocation and if it succeeds without
    // overflow do the allocation.
    size_t allocation_size;
    bool success = CheckedAddSizeT(header_size,
                                   data_size,
                                   &allocation_size);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    void *allocation = aligned_alloc(allocation_alignment, allocation_size);

    if (allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    *header = allocation;
    *data = (void*) ((char*) allocation + data_offset);

    if (actual_allocation_size != NULL)
    {
        *actual_allocation_size = allocation_size;
    }

    return ISTATUS_SUCCESS;
}

static
inline
ISTATUS
AlignedResizeWithHeader(
    _In_ _Pre_bytecount_(original_allocation_size) _Post_invalid_ void *original_header,
    _In_ _Pre_satisfies_(_Curr_ != 0) size_t original_allocation_size,
    _In_ _Pre_satisfies_(_Curr_ != 0) size_t header_size,
    _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && header_size % _Curr_ == 0) size_t header_alignment,
    _Outptr_result_bytebuffer_(header_size) void **header,
    _In_ size_t data_size,
    _When_(data_size != 0, _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && data_size % _Curr_ == 0)) size_t data_alignment,
    _When_(data_size != 0, _Out_ _Deref_post_bytecap_(data_size)) _When_(data_size == 0, _Out_opt_ _When_(data != NULL, _Deref_post_null_)) void **data,
    _Out_opt_ size_t *actual_allocation_size
    )
{
    assert(original_header != NULL);
    assert(original_allocation_size != 0);
    assert(header_size != 0);
    assert(header_alignment != 0);
    assert(header != NULL);
    assert((header_alignment & (header_alignment - 1)) == 0);
    assert(header_size % header_alignment == 0);
    assert(data_size == 0 || data_alignment != 0);
    assert(data_size == 0 || data != NULL);
    assert(data_size == 0 || (data_alignment & (data_alignment - 1)) == 0);
    assert(data_size == 0 || data_size % data_alignment == 0);

    // If allocation is not at least aligned to the header alignment or
    // is not big enough to fit the header size requested, simply allocate
    // a new chunk of memory and free this one.
    if ((header_alignment - 1) & (uintptr_t) original_header ||
        original_allocation_size < header_size)
    {
        ISTATUS result = AlignedAllocWithHeader(header_size,
                                                header_alignment,
                                                header,
                                                data_size,
                                                data_alignment,
                                                data,
                                                actual_allocation_size);

        if (result != ISTATUS_SUCCESS)
        {
            assert(result == ISTATUS_ALLOCATION_FAILED);
            return result;
        }

        free(original_header);
        return ISTATUS_SUCCESS;
    }

    // If there is no data to allocate, return the original block.
    if (data_size == 0)
    {
        *header = header_alignment;

        if (data != NULL)
        {
            *data = NULL;
        }

        if (actual_allocation_size != NULL)
        {
            *actual_allocation_size = original_allocation_size;
        }

        return ISTATUS_SUCCESS;
    }

    // Add padding to header size if the alignment of the new data region
    // would not match the required alignment.
    if (header_alignment < data_alignment)
    {
        uintptr_t data_start = (uintptr_t) original_header + header_size;
        size_t bytes_past_alignment = data_start & (data_alignment - 1);

        if (bytes_past_alignment != 0)
        {
            size_t header_padding = data_alignment - bytes_past_alignment;

            bool success = CheckedAddSizeT(header_size,
                                           header_padding,
                                           &header_size);

            if (Status != ISTATUS_SUCCESS)
            {
                return ISTATUS_ALLOCATION_FAILED;
            }
        }
    }

    // If the original allocation is large enough, use it. Otherwise, allocate
    // a new block and free the original block.
    size_t allocation_size;
    bool success = CheckedAddSizeT(header_size,
                                   data_size,
                                   &allocation_size);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (allocation_size <= original_allocation_size)
    {
        *header = original_header;
        *data = (void*) ((char*) original_header + header_size);

        if (actual_allocation_size != NULL)
        {
            *actual_allocation_size = OriginalAllocationSize;
        }

        return ISTATUS_SUCCESS;
    }

    ISTATUS result = AlignedAllocWithHeader(header_size,
                                            header_alignment,
                                            header,
                                            data_size,
                                            data_alignment,
                                            data,
                                            actual_allocation_size);

    if (result != ISTATUS_SUCCESS)
    {
        assert(result == ISTATUS_ALLOCATION_FAILED);
        return result;
    }

    free(original_header);
    return ISTATUS_SUCCESS;
}

static
inline
ISTATUS
AlignedAllocWithTwoHeaders(
    _In_ _Pre_satisfies_(_Curr_ != 0) size_t first_header_size,
    _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && first_header_size % _Curr_ == 0) size_t first_header_alignment,
    _Outptr_result_bytebuffer_(first_header_size) void **first_header,
    _In_ _Pre_satisfies_(_Curr_ != 0) size_t second_header_size,
    _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && second_header_size % _Curr_ == 0) size_t second_header_alignment,
    _Outptr_result_bytebuffer_(second_header_size) void **second_header,
    _In_ size_t data_size,
    _When_(data_size != 0, _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && data_size % _Curr_ == 0)) size_t data_alignment,
    _When_(data_size != 0, _Out_ _Deref_post_bytecap_(data_size)) _When_(data_size == 0, _Out_opt_ _When_(data != NULL, _Deref_post_null_)) void **data,
    _Out_opt_ size_t *actual_allocation_size
    )
{
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
    assert(data_size == 0 || data != NULL);
    assert(data_size == 0 || (data_alignment & (data_alignment - 1)) == 0);
    assert(data_size == 0 || data_size % data_alignment == 0);

    // If no data is required, use the single header allocator.
    if (data_size == 0)
    {
        ISTATUS result = IrisAlignedAllocWithHeader(first_header_size,
                                                    first_header_alignment,
                                                    first_header,
                                                    second_header_size,
                                                    second_header_alignment,
                                                    second_header,
                                                    actual_allocation_size);

        if (result != ISTATUS_SUCCESS)
        {
            assert(result == ISTATUS_ALLOCATION_FAILED);
            return result;
        }

        if (data != NULL)
        {
            *data = NULL;
        }
        
        return ISTATUS_SUCCESS;
    }

    // If the first header requires a smaller alignment than the second header,
    // round up the alignment of the first header to the second header.
    size_t header_alignment;
    if (first_header_alignment < second_header_alignment)
    {
        size_t bytes_past_alignment =
            first_header_size & (second_header_alignment - 1);

        if (bytes_past_alignment != 0)
        {
            size_t first_header_padding = 
                second_header_alignment - bytes_past_alignment;

            bool success = CheckedAddSizeT(first_header_size,
                                           first_header_padding,
                                           &first_header_size);

            if (!success)
            {
                return ISTATUS_ALLOCATION_FAILED;
            }
        }

        header_alignment = second_header_alignment;
    }
    else
    {
        header_alignment = first_header_alignment;
    }

    // Allocate the block of memory by combining the two headers into a single
    // header block aligned to the greater of the two headers' alignments.
    size_t header_size;
    bool success = CheckedAddSizeT(first_header_size,
                                   second_header_size,
                                   &header_size);

    ISTATUS result = AlignedAllocWithHeader(header_size,
                                            header_alignment,
                                            first_header,
                                            data_size,
                                            data_alignment,
                                            data,
                                            actual_allocation_size);

    if (result != ISTATUS_SUCCESS)
    {
        assert(result == ISTATUS_ALLOCATION_FAILED);
        return result;
    }

    *second_header = (void*) ((char*) first_header + first_header_size);

    return ISTATUS_SUCCESS;
}

static
inline
ISTATUS
AlignedResizeWithTwoHeader(
    _In_ _Pre_opt_bytecount_(original_size) _Post_invalid_ void* original_header,
    _In_ _Pre_satisfies_(_Curr_ != 0) size_t original_allocation_size,
    _In_ _Pre_satisfies_(_Curr_ != 0) size_t first_header_size,
    _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && first_header_size % _Curr_ == 0) size_t first_header_alignment,
    _Outptr_result_bytebuffer_(first_header_size) oid **first_header,
    _In_ _Pre_satisfies_(_Curr_ != 0) size_t second_header_size,
    _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && second_header_size % _Curr_ == 0) size_t second_header_alignment,
    _Outptr_result_bytebuffer_(second_header_size) void **second_header,
    _In_ size_t data_size,
    _When_(data_size != 0, _In_ _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && data_size % _Curr_ == 0)) size_t data_alignment,
    _When_(data_size != 0, _Out_ _Deref_post_bytecap_(data_size)) _When_(data_size == 0, _Out_opt_ _When_(data != NULL, _Deref_post_null_)) void **data,
    _Out_opt_ size_t *actual_allocation_size
    )
{
    assert(original_header != NULL);
    assert(original_allocation_size != 0);
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
    assert(data_size == 0 || data != NULL);
    assert(data_size == 0 || (data_alignment & (data_alignment - 1)) == 0);
    assert(data_size == 0 || data_size % data_alignment == 0);

    // If allocation is not at least aligned to the header alignment or
    // is not big enough to fit the header size requested, simply allocate
    // a new chunk of memory and free this one.
    if ((first_header_alignment - 1) & (uintptr_t) original_header ||
        original_allocation_size < first_header_size)
    {
        ISTATUS result = AlignedAllocWithTwoHeaders(first_header_size,
                                                    first_header_alignment,
                                                    first_header,
                                                    second_header_size,
                                                    second_header_alignment,
                                                    second_header,
                                                    data_size,
                                                    data_alignment,
                                                    data,
                                                    actual_allocation_size);

        if (result != ISTATUS_SUCCESS)
        {
            assert(result == ISTATUS_ALLOCATION_FAILED);
            return result;
        }

        free(original_header);
        return ISTATUS_SUCCESS;
    }

    // Add padding to first header size if the alignment of the second header
    // would not match the required alignment.
    if (first_header_alignment < second_header_alignment)
    {
        uintptr_t second_header_start = 
            (uintptr_t) original_header + first_header_size;
        
        size_t bytes_past_alignment = 
            first_header_size & (second_header_alignment - 1);

        if (bytes_past_alignment != 0)
        {
            size_t first_header_padding = 
                second_header_alignment - bytes_past_alignment;

            bool success = CheckedAddSizeT(first_header_size,
                                           header_padding,
                                           &first_header_size);

            if (Status != ISTATUS_SUCCESS)
            {
                return ISTATUS_ALLOCATION_FAILED;
            }
        }
    }

    // Validate that there is enough room in the allocation to contain
    // both the first and second headers. If not, free the original block
    // and return a new block of memory.
    size_t combined_header_size;
    bool success = CheckedAddSizeT(first_header_size,
                                   second_header_size,
                                   &combined_header_size);

    if (Status != ISTATUS_SUCCESS)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (original_allocation_size < combined_header_size)
    {
        ISTATUS result = AlignedAllocWithTwoHeaders(first_header_size,
                                                    first_header_alignment,
                                                    first_header,
                                                    second_header_size,
                                                    second_header_alignment,
                                                    second_header,
                                                    data_size,
                                                    data_alignment,
                                                    data,
                                                    actual_allocation_size);

        if (result != ISTATUS_SUCCESS)
        {
            assert(result == ISTATUS_ALLOCATION_FAILED);
            return result;
        }

        free(original_header);
        return ISTATUS_SUCCESS;
    }

    // Add padding to second header size if the alignment of the data block
    // would not match the required alignment.
    if (second_header_alignment < data_alignment)
    {
        uintptr_t data_start = 
            (uintptr_t) original_header + combined_header_size;
        
        size_t bytes_past_alignment = data_start & (data_alignment - 1);

        if (bytes_past_alignment != 0)
        {
            size_t second_header_padding = 
                data_alignment - bytes_past_alignment;

            bool success = CheckedAddSizeT(combined_header_size,
                                           second_header_padding,
                                           &combined_header_size);

            if (Status != ISTATUS_SUCCESS)
            {
                return ISTATUS_ALLOCATION_FAILED;
            }
        }
    }

    // Check that the allocation is large enough. If so, use it. Otherwise, free
    // and allocate a new chunk.
    size_t allocation_size;
    bool success = CheckedAddSizeT(combined_header_size,
                                   data_size,
                                   &allocation_size);

    if (Status != ISTATUS_SUCCESS)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (original_allocation_size < allocation_size)
    {
        ISTATUS result = AlignedAllocWithTwoHeaders(first_header_size,
                                                    first_header_alignment,
                                                    first_header,
                                                    second_header_size,
                                                    second_header_alignment,
                                                    second_header,
                                                    data_size,
                                                    data_alignment,
                                                    data,
                                                    actual_allocation_size);

        if (result != ISTATUS_SUCCESS)
        {
            assert(result == ISTATUS_ALLOCATION_FAILED);
            return result;
        }

        free(original_header);
        return ISTATUS_SUCCESS;
    }

    *first_header = original_header;
    *second_header = (void*) ((char*) original_header + first_header_size);

    if (data != NULL)
    {
        *data = (void*) ((char*) original_header + combined_header_size);
    }

    if (actual_allocation_size != NULL)
    {
        *actual_allocation_size = allocation_size;
    }

    return ISTATUS_SUCCESS;
}

#endif // _IRIS_ALLOC_