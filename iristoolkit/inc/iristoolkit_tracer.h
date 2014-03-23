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
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PRAYSHADER
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
PRAYSHADER
NonRoulettePathTracerAllocate(
    _In_ PCSCENE Scene,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon,
    _In_ UINT8 MaximumRecursionDepth
    )
{
    PRAYSHADER PathTracer;

    ASSERT(Scene != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsNormalFloat(Epsilon));
    ASSERT(IsFiniteFloat(Epsilon));

    PathTracer = PathTracerAllocate(Scene, 
                                    Rng,
                                    Epsilon,
                                    (FLOAT) 1.0,
                                    (FLOAT) 1.0,
                                    0,
                                    MaximumRecursionDepth);

    return PathTracer;
}

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PRAYSHADER
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
PRAYSHADER
RecursiveNonRouletteRayTracerAllocate(
    _In_ PCSCENE Scene,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon,
    _In_ UINT8 MaximumRecursionDepth
    )
{
    PRAYSHADER RecursiveRayTracer;

    ASSERT(Scene != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsNormalFloat(Epsilon));
    ASSERT(IsFiniteFloat(Epsilon));

    RecursiveRayTracer = RecursiveRayTracerAllocate(Scene, 
                                                    Rng,
                                                    Epsilon,
                                                    (FLOAT) 1.0,
                                                    (FLOAT) 1.0,
                                                    0,
                                                    MaximumRecursionDepth);

    return RecursiveRayTracer;
}

#endif // _TRACER_IRIS_TOOLKIT_