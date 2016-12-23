/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_raytracer.h

Abstract:

    This file contains the prototypes for the RayTracer functions.

--*/

#ifndef _RAYTRACER_IRIS_PHYSX_INTERNAL_
#define _RAYTRACER_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxRayTracerAllocate(
    _In_ SIZE_T MaximumDepth,
    _Out_ PPHYSX_RAYTRACER *RayTracer
    );

VOID
PhysxRayTracerConfigure(
    _Inout_ PPHYSX_RAYTRACER RayTracer,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_opt_ PCPHYSX_LIGHT_LIST LightList,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon
    );

VOID
PhysxRayTracerFree(
    _In_opt_ _Post_invalid_ PPHYSX_RAYTRACER RayTracer
    );

#endif // _RAYTRACER_IRIS_PHYSX_INTERNAL_
