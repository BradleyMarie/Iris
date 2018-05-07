/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    hit_allocator.c

Abstract:

    The hit allocation routines for Iris.

--*/

#include "iris/full_hit_context.h"
#include "iris/hit_allocator.h"
#include "iris/hit_allocator_internal.h"

#include <string.h>

//
// Static Functions
//

static
ISTATUS
HitAllocatorAllocateInternal(
    _Inout_ PHIT_ALLOCATOR allocator,
    _In_opt_ PHIT next,
    _In_ float_t distance,
    _In_ uint32_t front_face,
    _In_ uint32_t back_face,
    _When_(additional_data_size != 0, _In_reads_bytes_opt_(additional_data_size)) const void *additional_data,
    _In_ size_t additional_data_size,
    _When_(additional_data_size != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && additional_data_size % _Curr_ == 0)) size_t additional_data_alignment,
    _In_opt_ PPOINT3 hit_point,
    _Out_ PHIT *hit
    )
{
    assert(hit != NULL);
    assert(hit_point == NULL || PointValidate(*hit_point));

    if (allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(distance))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (additional_data_size != 0)
    {
        if (additional_data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
        
        if (additional_data_alignment == 0 ||
            (additional_data_alignment & (additional_data_alignment - 1)) != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;    
        }
        
        if (additional_data_size % additional_data_alignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    PFULL_HIT_CONTEXT hit_context;
    void *additional_data_dest;
    bool success = DynamicMemoryAllocatorAllocate(&allocator->allocator,
                                                  sizeof(FULL_HIT_CONTEXT),
                                                  alignof(FULL_HIT_CONTEXT),
                                                  (void **)&hit_context,
                                                  additional_data_size,
                                                  additional_data_alignment,
                                                  &additional_data_dest);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    hit_context->hit.next = next;
    hit_context->hit.distance = distance;
    hit_context->context.data = NULL;
    hit_context->context.distance = distance;
    hit_context->context.front_face = front_face;
    hit_context->context.back_face = back_face;
    hit_context->context.additional_data_size = additional_data_size;

    if (additional_data_size != 0)
    {
        memcpy(additional_data_dest, additional_data, additional_data_size);
        hit_context->context.additional_data = additional_data_dest;
    }
    else
    {
        hit_context->context.additional_data = NULL;
    }

    hit_context->shared_context = NULL;

    if (hit_point != NULL)
    {
        hit_context->model_hit_point = *hit_point;
        hit_context->model_hit_point_valid = true;
    }
    else
    {
        hit_context->model_hit_point_valid = false;
    }

    *hit = &hit_context->hit;

    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
HitAllocatorAllocate(
    _Inout_ PHIT_ALLOCATOR allocator,
    _In_opt_ PHIT next,
    _In_ float_t distance,
    _In_ uint32_t front_face,
    _In_ uint32_t back_face,
    _When_(additional_data_size != 0, _In_reads_bytes_opt_(additional_data_size)) const void *additional_data,
    _In_ size_t additional_data_size,
    _When_(additional_data_size != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && additional_data_size % _Curr_ == 0)) size_t additional_data_alignment,
    _Out_ PHIT *hit
    )
{
    if (hit == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    ISTATUS status = HitAllocatorAllocateInternal(allocator,
                                                  next,
                                                  distance,
                                                  front_face,
                                                  back_face,
                                                  additional_data,
                                                  additional_data_size,
                                                  additional_data_alignment,
                                                  NULL,
                                                  hit);

    return status;
}

ISTATUS
HitAllocatorAllocateWithHitPoint(
    _Inout_ PHIT_ALLOCATOR allocator,
    _In_opt_ PHIT next,
    _In_ float_t distance,
    _In_ uint32_t front_face,
    _In_ uint32_t back_face,
    _When_(additional_data_size != 0, _In_reads_bytes_opt_(additional_data_size)) const void *additional_data,
    _In_ size_t additional_data_size,
    _When_(additional_data_size != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && additional_data_size % _Curr_ == 0)) size_t additional_data_alignment,
    _In_ POINT3 hit_point,
    _Out_ PHIT *hit
    )
{
    if (!PointValidate(hit_point))
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    if (hit == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_09;
    }

    ISTATUS status = HitAllocatorAllocateInternal(allocator,
                                                  next,
                                                  distance,
                                                  front_face,
                                                  back_face,
                                                  additional_data,
                                                  additional_data_size,
                                                  additional_data_alignment,
                                                  &hit_point,
                                                  hit);

    return status;
}