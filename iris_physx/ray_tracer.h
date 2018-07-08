/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    ray_tracer.h

Abstract:

    Traces through a set of geometry and returns back the light and the brdf
    associated with the nearest intersection.
    
    If there is no intersection, brdf will hit_point, surface_normal, and 
    shading_normal will not be initialized and brdf will be set to NULL.

    Any returned pointers are guaranteed to live at least as long as the ray
    tracer.

--*/

#ifndef _IRIS_PHYSX_RAY_TRACER_
#define _IRIS_PHYSX_RAY_TRACER_

#include "iris_physx/brdf.h"
#include "iris_physx/light.h"

//
// Types
//

typedef struct _SHAPE_RAY_TRACER SHAPE_RAY_TRACER, *PSHAPE_RAY_TRACER;
typedef const SHAPE_RAY_TRACER *PCSHAPE_RAY_TRACER;

//
// Functions
//

ISTATUS
ShapeRayTracerTrace(
    _Inout_ PSHAPE_RAY_TRACER ray_tracer,
    _In_ RAY ray,
    _Outptr_result_maybenull_ PLIGHT *light,
    _Outptr_result_maybenull_ PBRDF *brdf,
    _Out_ PPOINT3 hit_point,
    _Out_ PVECTOR3 surface_normal,
    _Out_ PVECTOR3 shading_normal
    );

#endif // _IRIS_PHYSX_RAY_TRACER_