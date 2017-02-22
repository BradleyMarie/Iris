/*++

Copyright (c) 2017 Brad Weinberger

Module Name:

    raytracer.c

Abstract:

    This file contains the definitions for the SAMPLER_TRACER type.

--*/

#include <iriscamerap.h>

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SampleRayTracerTrace(
    _In_ PCSAMPLE_RAYTRACER RayTracer,
    _In_ RAY WorldRay,
    _Out_ PCOLOR3 Color
	)
{
    ISTATUS Status;

    if (RayTracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(WorldRay) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (Color == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Status = SampleTracerTrace(RayTracer->SampleTracer,
                               RayTracer->ThreadState,
                               WorldRay,
                               RayTracer->Rng,
                               Color);

    return Status;
}