/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    hit_allocator.h

Abstract:

    Allocator for hits during Iris Physx intersection tests. See documentation
    in "iris/hit_allocator.h" for further detail.

--*/

#ifndef _IRIS_PHYSX_HIT_ALLOCATOR_
#define _IRIS_PHYSX_HIT_ALLOCATOR_

#include "iris/iris.h"

//
// Types
//

typedef struct _HIT_ALLOCATOR SHAPE_HIT_ALLOCATOR, *PSHAPE_HIT_ALLOCATOR;
typedef const SHAPE_HIT_ALLOCATOR *PCSHAPE_HIT_ALLOCATOR;

//
// Functions
//

static
inline
ISTATUS
ShapeHitAllocatorAllocate(
    _Inout_ PSHAPE_HIT_ALLOCATOR allocator,
    _In_opt_ PHIT next,
    _In_ float_t distance,
    _In_ uint32_t front_face,
    _In_ uint32_t back_face,
    _In_reads_bytes_opt_(additional_data_size) const void *additional_data,
    _In_ size_t additional_data_size,
    _In_ size_t additional_data_alignment,
    _Out_ PHIT *hit
    )
{
    ISTATUS status = HitAllocatorAllocate(allocator,
                                          next,
                                          distance,
                                          front_face,
                                          back_face,
                                          additional_data,
                                          additional_data_size,
                                          additional_data_alignment,
                                          hit);

    return status;
}

static
inline
ISTATUS
ShapeHitAllocatorAllocateWithHitPoint(
    _Inout_ PSHAPE_HIT_ALLOCATOR allocator,
    _In_opt_ PHIT next,
    _In_ float_t distance,
    _In_ uint32_t front_face,
    _In_ uint32_t back_face,
    _In_reads_bytes_opt_(additional_data_size) const void *additional_data,
    _In_ size_t additional_data_size,
    _In_ size_t additional_data_alignment,
    _In_ POINT3 hit_point,
    _Out_ PHIT *hit
    )
{
    ISTATUS status = HitAllocatorAllocateWithHitPoint(allocator,
                                                      next,
                                                      distance,
                                                      front_face,
                                                      back_face,
                                                      additional_data,
                                                      additional_data_size,
                                                      additional_data_alignment,
                                                      hit_point,
                                                      hit);

    return status;
}

#endif // _IRIS_HIT_ALLOCATOR_