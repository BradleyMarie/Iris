/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    hit_allocator.h

Abstract:

    Allocates hits during a successful intersection. Hits are allocated directly
    into a linked list of all of the hits generated during an intersection. The
    context of the hits are not exposed directly from this routine. Instead,
    the contexts are opaquely assocated with each allocated hit.

    The arguments distance, front_face, and back_face are are set directly into 
    the allocated hit context. The argument additional_data is not used directly 
    and is instead copied into a newly allocated block of memory associated with 
    the hit object. Since there is no cleanup logic run when a hit goes out of 
    scope, only data which is trivially copyable should be specified as 
    additional data in order to avoid leaking.

    The allocator also optionally consumes a hit_point argument which can be
    used as an optimization if the exact coordinates of a hit point were
    computed during the intersection test to avoid having to recompute it later
    on. This should only be specified if the hit point must be computed during
    intersection testing and is not worthwhile to specify otherwise.

    The hits allocated by this allocator are managed by Iris and should not be 
    explicitly freed.

--*/

#ifndef _IRIS_HIT_ALLOCATOR_
#define _IRIS_HIT_ALLOCATOR_

#include "iris/point.h"
#include "iris/sal.h"

#include <float.h>
#include <stdint.h>

//
// Types
//

typedef struct _HIT {
    struct _HIT *next;
    const float_t *distance;
} HIT, *PHIT;

typedef const HIT *PCHIT;

typedef struct _HIT_ALLOCATOR HIT_ALLOCATOR, *PHIT_ALLOCATOR;
typedef const HIT_ALLOCATOR *PCHIT_ALLOCATOR;

//
// Functions
//

//IRISAPI
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
    );

//IRISAPI
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
    );

#endif // _IRIS_HIT_ALLOCATOR_