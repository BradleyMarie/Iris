/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_pathtracer.h

Abstract:

    This file contains the prototypes for the PATHTRACER type.

--*/

#include <iristoolkitp.h>

//
// Types
//

typedef struct _PATHTRACER {
    PRAYSHADER_HEADER Header;
    PVISIBILITY_TESTER VisibilityTester;
    PRAYSHADER *NextRayShader;
} PATHTRACER, *PPATHTRACER;

typedef CONST PATHTRACER *PCPATHTRACER;

//
// Static
//

STATIC
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PathTracerShadeHit(
    _In_ PVOID Context,
    _In_ PCRAY WorldRay,
    _In_ PCGEOMETRY_HIT Hit,
    _Out_ PCOLOR4 Color
    );

STATIC
VOID
PathTracerFree(
    _Pre_maybenull_ _Post_invalid_ PVOID PathTracer
    );

_Check_return_
_Ret_maybenull_
STATIC
PRAYSHADER
PathTracerAllocateInternal(
    _In_ PCSCENE Scene,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon,
    _In_ FLOAT MinimumContinueProbability,
    _In_ FLOAT MaximumContinueProbability,
    _In_ UINT8 RussianRouletteStartDepth,
    _In_ UINT8 MaximumRecursionDepth,
    _In_ UINT8 CurrentDepth
    );

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
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