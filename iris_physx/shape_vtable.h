/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    shape_vtable.h

Abstract:

    The vtable for a shape.

--*/

#ifndef _IRIS_PHYSX_SHAPE_VTABLE_
#define _IRIS_PHYSX_SHAPE_VTABLE_

#include "iris_physx/hit_allocator.h"
#include "iris_physx/material.h"

//
// Types
//

typedef
ISTATUS
(*PSHAPE_TRACE_ROUTINE)(
    _In_ const void *context,
    _In_ RAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    );

typedef
ISTATUS
(*PSHAPE_CHECK_BOUNDS_ROUTINE)(
    _In_ const void *context,
    _In_opt_ PCMATRIX model_to_world,
    _In_ BOUNDING_BOX world_aligned_bounding_box,
    _Out_ bool *is_inside
    );

typedef
ISTATUS
(*PSHAPE_COMPUTE_NORMAL_ROUTINE)(
    _In_ const void *context,
    _In_ POINT3 hit_point,
    _In_ uint32_t face_hit,
    _Out_ PVECTOR3 surface_normal
    );

typedef
ISTATUS
(*PSHAPE_GET_MATERIAL_ROUTINE)(
    _In_opt_ const void *context, 
    _In_ uint32_t face_hit,
    _Outptr_result_maybenull_ PCMATERIAL *material
    );

typedef struct _SHAPE_VTABLE {
    PSHAPE_TRACE_ROUTINE trace_routine;
    PSHAPE_CHECK_BOUNDS_ROUTINE check_bounds_routine;
    PSHAPE_COMPUTE_NORMAL_ROUTINE compute_normal_routine;
    PSHAPE_GET_MATERIAL_ROUTINE get_material_routine;
    PFREE_ROUTINE free_routine;
} SHAPE_VTABLE, *PSHAPE_VTABLE;

typedef const SHAPE_VTABLE *PCSHAPE_VTABLE;

#endif // _IRIS_PHYSX_SHAPE_VTABLE_