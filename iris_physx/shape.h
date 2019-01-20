/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    shape.h

Abstract:

    The interface representing a shape.

--*/

#ifndef _IRIS_PHYSX_SHAPE_
#define _IRIS_PHYSX_SHAPE_

#include "iris_physx/shape_vtable.h"

//
// Types
//

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
    _In_opt_ PCMATRIX model_to_world,
    _In_ BOUNDING_BOX world_bounds,
    _Out_ bool *contains
    );

void
ShapeRetain(
    _In_opt_ PSHAPE Shape
    );

void
ShapeRelease(
    _In_opt_ _Post_invalid_ PSHAPE Shape
    );

#endif // _IRIS_PHYSX_SHAPE_
