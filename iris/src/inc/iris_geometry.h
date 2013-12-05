/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_geometry.h

Abstract:

    This module declares the internal geometry interface.

--*/

#ifndef _IRIS_GEOMETRY_INTERNAL_
#define _IRIS_GEOMETRY_INTERNAL_

#include <irisp.h>

//
// Types
//

typedef 
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PGEOMETRY_TRACE_ROUTINE)(
    _In_ PVOID Context, 
    _In_ PRAY Ray,
    _Inout_ PSHAPE_HIT_COLLECTION ShapeHitCollection
    );

//
// Function declarations
//

ISTATUS
GeometryTraceShapeCallback(
    _In_ PVOID Context, 
    _In_ PRAY Ray,
    _Inout_ PSHAPE_HIT_COLLECTION ShapeHitCollection
    );

//
// Function Definitions
//

SFORCEINLINE
ISTATUS
GeometryTrace(
    _In_ PGEOMETRY Geometry,
    _In_ PRAY Ray,
    _Inout_ PSHAPE_HIT_COLLECTION ShapeHitCollection
    )
{
    PGEOMETRY_TRACE_ROUTINE TraceRoutine;
    PGEOMETRY_VTABLE VTable;

    ASSERT(Geometry != NULL);
    ASSERT(Ray != NULL);
    ASSERT(ShapeHitCollection != NULL);

    VTable = Geometry->VTable;

    TraceRoutine = (PGEOMETRY_TRACE_ROUTINE) VTable->GeometryTraceRoutine;

    return TraceRoutine(Geometry, Ray, ShapeHitCollection);
}

#endif // _IRIS_GEOMETRY_INTERNAL_