/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_geometrytracer.h

Abstract:

    This file contains the implementation of the geometry tracer object.

--*/

#ifndef _IRIS_GEOMETRY_TRACER_
#define _IRIS_GEOMETRY_TRACER_

#include <iris.h>

//
// Types
//

typedef struct _GEOMETRY_TRACER GEOMETRY_TRACER, *PGEOMETRY_TRACER;

//
// Function definitions
//
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
GeometryTracerTraceGeometry(
    _Inout_ PGEOMETRY_TRACER Tracer,
    _In_ PGEOMETRY Geometry,
    _Outptr_result_buffer_(NumberOfHits) PGEOMETRY_HIT *GeometryHits,
    _Out_ PSIZE_T NumberOfHits
    );

#endif // _IRIS_GEOMETRY_TRACER_