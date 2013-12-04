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
(*PGEOMTRACE_ROUTINE)(
	_In_ PVOID Context, 
	_In_ PRAY Ray,
	_Out_ PGEOMETRY_HIT GeometryHit,
	_Inout_ PSHAPE_HIT_COLLECTION ShapeHitCollection
	);

//
// Function declarations
//

ISTATUS
GeometryTraceShapeCallback(
    _In_ PVOID Context, 
    _In_ PRAY Ray,
    _Out_ PVOID GeometryHitList,
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
    _Out_ PGEOMETRY_HIT GeometryHit,
	_Inout_ PSHAPE_HIT_COLLECTION ShapeHitCollection
    )
{
    PGEOMTRACE_ROUTINE TraceRoutine;

    ASSERT(Geometry != NULL);
    ASSERT(Ray != NULL);
    ASSERT(GeometryHit != NULL);
	ASSERT(ShapeHitCollection != NULL);

    TraceRoutine = Geometry->VTable->GeometryTraceRoutine;

	return TraceRoutine(Geometry, Ray, GeometryHit, ShapeHitCollection);
}

#endif // _IRIS_GEOMETRY_INTERNAL_