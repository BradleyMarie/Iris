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
    _In_opt_ PPHYSX_RAYTRACER NextRayTracer,
    _In_ PPHYSX_SHARED_CONTEXT SharedContext,
    _Out_ PPHYSX_RAYTRACER *Result
    );

VOID
PhysxRayTracerFree(
    _In_opt_ _Post_invalid_ PPHYSX_RAYTRACER RayTracer
    );

#endif // _RAYTRACER_IRIS_PHYSX_INTERNAL_
