/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_pathtracer.h

Abstract:

    This file contains the prototypes for the PATHTRACER type.

--*/

#ifndef _PATHTRACER_IRIS_TOOLKIT_
#define _PATHTRACER_IRIS_TOOLKIT_

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

#endif // _TRIANGLE_IRIS_TOOLKIT_