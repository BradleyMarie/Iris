/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_raytracer.h

Abstract:

    This file contains the definitions for the SPECTRUM_RAYTRACER type.

--*/

#ifndef _SPECTRUM_RAYTRACER_IRIS_PHYSX_INTERNAL_
#define _SPECTRUM_RAYTRACER_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _SPECTRUM_RAYTRACER {
    PRAYTRACER_OWNER RayTracerOwner;
    PCSPECTRUM_SCENE Scene;
};

//
// Functions
//

SFORCEINLINE
ISTATUS
SpectrumRayTracerInitialize(
    _Out_ PSPECTRUM_RAYTRACER SpectrumRayTracer
    )
{
    PRAYTRACER_OWNER RayTracerOwner;
    ISTATUS Status;

    ASSERT(SpectrumRayTracer != NULL);

    Status = RayTracerOwnerAllocate(&RayTracerOwner);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    SpectrumRayTracer->RayTracerOwner = RayTracerOwner;
    SpectrumRayTracer->Scene = NULL;

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
SpectrumRayTracerSetScene(
    _Inout_ PSPECTRUM_RAYTRACER SpectrumRayTracer,
    _In_ PCSPECTRUM_SCENE Scene
    )
{
    ASSERT(SpectrumRayTracer != NULL);
    ASSERT(Scene != NULL);
    
    SpectrumRayTracer->Scene = Scene;
}

SFORCEINLINE
VOID
SpectrumRayTracerDestroy(
    _In_ _Post_invalid_ PSPECTRUM_RAYTRACER SpectrumRayTracer
    )
{
    RayTracerOwnerFree(SpectrumRayTracer->RayTracerOwner);
}

#endif // _SPECTRUM_RAYTRACER_IRIS_PHYSX_INTERNAL_