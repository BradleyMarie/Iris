/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_tracer.h

Abstract:

    This file contains the prototypes for the tracer type.

--*/

#ifndef _TRACER_IRIS_TOOLKIT_
#define _TRACER_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Types
//

typedef struct _TRACER {
    PFREE_ROUTINE FreeRoutine;
    PRAYSHADER RayShader;
} TRACER, *PTRACER;

typedef CONST TRACER *PCTRACER;

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PTRACER
PathTracerAllocate(
    _In_ PCSCENE Scene,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon,
    _In_ FLOAT MinimumContinueProbability,
    _In_ FLOAT MaximumContinueProbability,
    _In_ UINT8 RussianRouletteStartDepth,
    _In_ UINT8 MaximumRecursionDepth
    );

_Check_return_
_Ret_maybenull_
SFORCEINLINE
PTRACER
NonRoulettePathTracerAllocate(
    _In_ PCSCENE Scene,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon,
    _In_ UINT8 MaximumRecursionDepth
    )
{
    PTRACER PathTracer;

    ASSERT(Scene != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsNormalFloat(Epsilon));
    ASSERT(IsFiniteFloat(Epsilon));

    PathTracer = PathTracerAllocate(Scene, 
                                    Rng,
                                    Epsilon,
                                    (FLOAT) 1.0,
                                    (FLOAT) 1.0,
                                    DISABLE_RUSSAIAN_ROULETTE_TERMINATION,
                                    MaximumRecursionDepth);

    return PathTracer;
}

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PTRACER
RecursiveRayTracerAllocate(
    _In_ PCSCENE Scene,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon,
    _In_ FLOAT MinimumContinueProbability,
    _In_ FLOAT MaximumContinueProbability,
    _In_ UINT8 RussianRouletteStartDepth,
    _In_ UINT8 MaximumRecursionDepth
    );

_Check_return_
_Ret_maybenull_
SFORCEINLINE
PTRACER
RecursiveNonRouletteRayTracerAllocate(
    _In_ PCSCENE Scene,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon,
    _In_ UINT8 MaximumRecursionDepth
    )
{
    PTRACER RecursiveRayTracer;

    ASSERT(Scene != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsNormalFloat(Epsilon));
    ASSERT(IsFiniteFloat(Epsilon));

    RecursiveRayTracer = RecursiveRayTracerAllocate(Scene, 
                                                    Rng,
                                                    Epsilon,
                                                    (FLOAT) 1.0,
                                                    (FLOAT) 1.0,
                                                    DISABLE_RUSSAIAN_ROULETTE_TERMINATION,
                                                    MaximumRecursionDepth);

    return RecursiveRayTracer;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
TracerTraceRay(
    _Inout_ PTRACER Tracer,
    _In_ RAY WorldRay,
    _Out_ PCOLOR3 Color
    )
{
    COLOR3 AmountReflected;
    ISTATUS Status;

    ASSERT(Tracer != NULL);
    ASSERT(Color != NULL);

    AmountReflected = Color3InitializeWhite();

    Status = RayShaderTraceRayMontecarlo(Tracer->RayShader,
                                         WorldRay,
                                         AmountReflected,
                                         Color);

    return Status;
}

SFORCEINLINE
VOID
TracerFree(
    _In_opt_ _Post_invalid_ PTRACER Tracer
    )
{
    if (Tracer == NULL)
    {
        return;
    }

    Tracer->FreeRoutine(Tracer);
}

#endif // _TRACER_IRIS_TOOLKIT_