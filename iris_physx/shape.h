/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    shape.h

Abstract:

    The interface representing a shape.

--*/

#ifndef _IRIS_PHYSX_SHAPE_
#define _IRIS_PHYSX_SHAPE_

#include "iris/iris.h"
#include "iris_advanced/iris_advanced.h"
#include "iris_physx/material.h"
#include "iris_physx/hit_allocator.h"

//
// Types
//

typedef
ISTATUS
(*PSHAPE_TRACE_ROUTINE)(
    _In_ const void *context,
    _In_ RAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT_LIST *hit_list
    );

typedef
ISTATUS
(*PSHAPE_COMPUTE_NORMAL_ROUTINE)(
    _In_ const void *context,
    _In_ uint32_t face_hit,
    _In_ POINT3 hit_point,
    _Out_ PVECTOR3 surface_normal
    );

typedef
ISTATUS
(*PSHAPE_CHECK_BOUNDS_ROUTINE)(
    _In_ const void *context,
    _In_ PCMATRIX model_to_world,
    _In_ BOUNDING_BOX world_aligned_bounding_box,
    _Out_ bool *is_inside
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
    PSHAPE_COMPUTE_NORMAL_ROUTINE compute_normal_routine;
    PSHAPE_CHECK_BOUNDS_ROUTINE check_bounds_routine;
    PSHAPE_GET_MATERIAL_ROUTINE get_material_routine;
    PFREE_ROUTINE free_routine;
} SHAPE_VTABLE, *PSHAPE_VTABLE;

typedef const SHAPE_VTABLE *PCSHAPE_VTABLE;

typedef struct _SHAPE SHAPE, *PSHAPE;
typedef const SHAPE *PCSHAPE;

//
// Functions
//

ISTATUS
ShapeAllocate(
    _In_ PCSHAPE_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PSHAPE *shape
    );

ISTATUS 
ShapeCheckBounds(
    _In_ PCSHAPE shape,
    _In_ PCMATRIX model_to_world,
    _In_ BOUNDING_BOX world_aligned_bounding_box,
    _Out_ bool *is_inside
    );

ISTATUS
ShapeComputeNormal(
    _In_ PCSHAPE shape,
    _In_ uint32_t face_hit,
    _In_ POINT3 hit_point,
    _Out_ PVECTOR3 surface_normal
    );

ISTATUS
ShapeGetMaterial(
    _In_ PCSHAPE shape,
    _In_ uint32_t face_hit,
    _Outptr_result_maybenull_ PCMATERIAL *material
    );

/*
ISTATUS
ShapeGetLight(
    _In_ PCSHAPE shape,
    _In_ uint32_t face_hit,
    _Outptr_result_maybenull_ PCLIGHT *light
    );
*/

void
ShapeRetain(
    _In_opt_ PSHAPE Shape
    );

void
ShapeRelease(
    _In_opt_ _Post_invalid_ PSHAPE Shape
    );

#endif // _IRIS_PHYSX_SHAPE_
