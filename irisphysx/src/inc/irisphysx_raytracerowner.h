/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_raytracer.h

Abstract:

    This file contains the definitions for the SPECTRUM_RAYTRACER type.

--*/

#ifndef _SPECTRUM_RAYTRACER_OWNER_IRIS_PHYSX_INTERNAL_
#define _SPECTRUM_RAYTRACER_OWNER_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

typedef struct _SPECTRUM_RAYTRACER_OWNER {
    SPECTRUM_RAYTRACER RayTracer;
} SPECTRUM_RAYTRACER_OWNER, *PSPECTRUM_RAYTRACER_OWNER;

typedef CONST SPECTRUM_RAYTRACER_OWNER *PCSPECTRUM_RAYTRACER_OWNER;

//
// Functions
//

SFORCEINLINE
ISTATUS
SpectrumRayTracerOwnerInitialize(
    _Out_ PSPECTRUM_RAYTRACER_OWNER SpectrumRayTracerOwner
    )
{
    ISTATUS Status;
    
    ASSERT(SpectrumRayTracerOwner != NULL);
    
    Status = SpectrumRayTracerInitialize(&SpectrumRayTracerOwner->RayTracer);
    
    return Status;
}

SFORCEINLINE
VOID
SpectrumRayTracerOwnerGetRayTracer(
    _Inout_ PSPECTRUM_RAYTRACER_OWNER SpectrumRayTracerOwner,
    _In_ PCSPECTRUM_SCENE Scene,
    _Out_ PSPECTRUM_RAYTRACER *SpectrumRayTracer
    )
{
    PSPECTRUM_RAYTRACER RayTracer;
    
    ASSERT(SpectrumRayTracerOwner != NULL);
    ASSERT(Scene != NULL);
    ASSERT(SpectrumRayTracer != NULL);
    
    RayTracer = &SpectrumRayTracerOwner->RayTracer;
    
    SpectrumRayTracerSetScene(RayTracer, Scene);
    *SpectrumRayTracer = RayTracer;
}

SFORCEINLINE
VOID
SpectrumRayTracerOwnerDestroy(
    _In_ _Post_invalid_ PSPECTRUM_RAYTRACER_OWNER SpectrumRayTracerOwner
    )
{
    SpectrumRayTracerDestroy(&SpectrumRayTracerOwner->RayTracer);
}
 
#endif // _SPECTRUM_RAYTRACER_OWNER_IRIS_PHYSX_INTERNAL_