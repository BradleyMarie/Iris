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
    PRAYTRACER RayTracer;
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
    PRAYTRACER RayTracer;
    ISTATUS Status;

    ASSERT(SpectrumRayTracer != NULL);

    Status = RayTracerAllocate(&RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    SpectrumRayTracer->RayTracer = RayTracer;
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
    RayTracerFree(SpectrumRayTracer->RayTracer);
}

#endif // _SPECTRUM_RAYTRACER_IRIS_PHYSX_INTERNAL_