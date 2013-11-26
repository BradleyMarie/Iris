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

typedef ISTATUS (*PGEOMTRACE_ROUTINE)(_In_ PVOID Context, 
                                      _In_ PRAY Ray,
                                      _In_ PVOID GeometryHitList,
                                      _Inout_ PSHAPE_HIT_LIST ShapeHitList);

//
// Function declarations
//

ISTATUS
GeometryTraceShapeCallback(
    _In_ PVOID Context, 
    _In_ PRAY Ray,
    _In_ PVOID GeometryHitList,
    _Inout_ PSHAPE_HIT_LIST ShapeHitList
    );

//
// Function Definitions
//

SFORCEINLINE
ISTATUS
GeometryTrace(
    _In_ PGEOMETRY Geometry,
    _In_ PRAY Ray,
    _In_ PVOID GeometryHitList,
    _Inout_ PSHAPE_HIT_LIST ShapeHitList
    )
{
    PGEOMTRACE_ROUTINE TraceRoutine;

    ASSERT(Geometry != NULL);
    ASSERT(Ray != NULL);
    ASSERT(GeometryHitList != NULL);
    ASSERT(ShapeHitList != NULL);

    TraceRoutine = Geometry->VTable->GeometryTraceRoutine;

    return TraceRoutine(Geometry, Ray, GeometryHitList, ShapeHitList);
}

#endif // _IRIS_GEOMETRY_INTERNAL_