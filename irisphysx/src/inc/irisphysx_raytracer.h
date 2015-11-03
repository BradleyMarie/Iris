/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_raytracer.h

Abstract:

    This file contains the definitions for the SPECTRUM_RAYTRACER type.

--*/

#ifndef _SPECTRUM_RAYTRACER_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _SPECTRUM_RAYTRACER {
    PRAYTRACER RayTracer;
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
    VECTOR3 InitialDirection;
    POINT3 InitialOrigin;
    RAY InitialRay;
    PRAYTRACER RayTracer;
    ISTATUS Status;

    ASSERT(SpectrumRayTracer != NULL);
    
    InitialOrigin = PointCreate((FLOAT) 0.0f, 
                                (FLOAT) 0.0f, 
                                (FLOAT) 0.0f);

    InitialDirection = VectorCreate((FLOAT) 0.0f, 
                                    (FLOAT) 0.0f, 
                                    (FLOAT) 1.0f);

    InitialRay = RayCreate(InitialOrigin, InitialDirection);

    Status = RayTracerAllocate(InitialRay, &RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    SpectrumRayTracer->RayTracer = RayTracer;

    return ISTATUS_SUCCESS;
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