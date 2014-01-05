/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_raytracer.h

Abstract:

    This file contains the definitions for the SCENE_OBJECT type.

--*/

#ifndef _RAYTRACER_IRIS_
#define _RAYTRACER_IRIS_

#include <iris.h>

//
// Types
//

typedef struct _RAYTRACER RAYTRACER, *PRAYTRACER;
typedef CONST RAYTRACER *PCRAYTRACER;

//
// Functions
//

_Check_return_
_Ret_maybenull_
PRAYTRACER
RayTracerAllocate(
    VOID
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RayTracerTraceScene(
    _Inout_ PRAYTRACER RayTracer,
    _In_ PCRAY WorldRay,
    _In_ PCSCENE Scene,
    _In_ BOOL SortResults,
    _Outptr_result_buffer_(HitListSize) PCGEOMETRY_HIT **HitList,
    _Out_ PSIZE_T HitListSize
    );

VOID
RayTracerFree(
    _Pre_maybenull_ _Post_invalid_ PRAYTRACER RayTracer
    );

#endif // _RAYTRACER_IRIS_