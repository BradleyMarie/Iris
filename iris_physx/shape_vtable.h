/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    shape_vtable.h

Abstract:

    The vtable for a shape.

--*/

#ifndef _IRIS_PHYSX_SHAPE_VTABLE_
#define _IRIS_PHYSX_SHAPE_VTABLE_

#include "iris_physx/emissive_material.h"
#include "iris_physx/hit_allocator.h"
#include "iris_physx/material.h"
#include "iris_physx/normal_map.h"
#include "iris_physx/texture_coordinate_allocator.h"

//
// Types
//

typedef
ISTATUS
(*PSHAPE_TRACE_ROUTINE)(
    _In_ const void *context,
    _In_ PCRAY ray,
    _In_ PSHAPE_HIT_ALLOCATOR allocator,
    _Out_ PHIT *hit
    );

typedef
ISTATUS
(*PSHAPE_COMPUTE_BOUNDS_ROUTINE)(
    _In_ const void *context,
    _In_opt_ PCMATRIX model_to_world,
    _Out_ PBOUNDING_BOX world_bounds
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
(*PSHAPE_GET_NORMAL_MAP)(
    _In_opt_ const void *context,
    _In_ uint32_t face_hit,
    _Outptr_ PCNORMAL_MAP *normal_map
    );

typedef
ISTATUS
(*PSHAPE_GET_MATERIAL_ROUTINE)(
    _In_opt_ const void *context,
    _In_ uint32_t face_hit,
    _Outptr_result_maybenull_ PCMATERIAL *material
    );

typedef
ISTATUS
(*PSHAPE_GET_EMISSIVE_MATERIAL_ROUTINE)(
    _In_opt_ const void *context,
    _In_ uint32_t face_hit,
    _Outptr_result_maybenull_ PCEMISSIVE_MATERIAL *emissive_material
    );

typedef
ISTATUS
(*PSHAPE_SAMPLE_FACE)(
    _In_opt_ const void *context,
    _In_ uint32_t face_hit,
    _Inout_ PRANDOM rng,
    _Out_ PPOINT3 point
    );

typedef
ISTATUS
(*PSHAPE_COMPUTE_PDF_BY_SOLID_ANGLE)(
    _In_opt_ const void *context,
    _In_ PCRAY to_shape,
    _In_ float_t distance,
    _In_ uint32_t face_hit,
    _Out_ float_t *pdf
    );

typedef
ISTATUS
(*PSHAPE_COMPUTE_TEXTURE_COORDINATES)(
    _In_opt_ const void *context,
    _In_ POINT3 hit_point,
    _In_ uint32_t face_hit,
    _In_ const void *additional_data,
    _Inout_ PTEXTURE_COORDINATE_ALLOCATOR allocator,
    _Out_ const void **texture_coordinates
    );

typedef struct _SHAPE_VTABLE {
    PSHAPE_TRACE_ROUTINE trace_routine;
    PSHAPE_COMPUTE_BOUNDS_ROUTINE compute_bounds_routine;
    PSHAPE_COMPUTE_NORMAL_ROUTINE compute_normal_routine;
    PSHAPE_GET_NORMAL_MAP get_normal_map_routine;
    PSHAPE_GET_MATERIAL_ROUTINE get_material_routine;
    PSHAPE_GET_EMISSIVE_MATERIAL_ROUTINE get_emissive_material_routine;
    PSHAPE_SAMPLE_FACE sample_face_routine;
    PSHAPE_COMPUTE_PDF_BY_SOLID_ANGLE compute_pdf_by_solid_angle_routine;
    PSHAPE_COMPUTE_TEXTURE_COORDINATES compute_texture_coordinates;
    PFREE_ROUTINE free_routine;
} SHAPE_VTABLE, *PSHAPE_VTABLE;

typedef const SHAPE_VTABLE *PCSHAPE_VTABLE;

#endif // _IRIS_PHYSX_SHAPE_VTABLE_