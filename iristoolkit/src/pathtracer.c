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
    PRAYSHADER NextRayShader;
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
    _In_ FLOAT Distance,
    _In_ PCVECTOR3 WorldViewer,
    _In_ PCPOINT3 WorldHit,
    _In_ PCVECTOR3 ModelViewer,
    _In_ PCPOINT3 ModelHit,
    _In_ PCMATRIX ModelToWorld,
    _In_ PCVOID AdditionalData,
    _In_ PCSHADER Shader,
    _In_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PCOLOR4 Color
    )
{
    return ISTATUS_SUCCESS;
}

STATIC
VOID
PathTracerFreeInternal(
    _Pre_maybenull_ _Post_invalid_ PVOID Tracer,
    _In_ BOOL FreeVisibilityTester
    )
{
    PPATHTRACER PathTracer;

    if (Tracer == NULL)
    {
        return;
    }

    PathTracer = (PPATHTRACER) Tracer;

    if (PathTracer->NextRayShader != NULL)
    {
        PathTracerFreeInternal(PathTracer->NextRayShader, 
                               FreeVisibilityTester);
    }
    else if (FreeVisibilityTester != FALSE)
    {
        VisibilityTesterFree(PathTracer->VisibilityTester);
    }
    
    free(PathTracer);
}

STATIC
VOID
PathTracerFree(
    _Pre_maybenull_ _Post_invalid_ PVOID PathTracer
    )
{
    PathTracerFreeInternal(PathTracer, TRUE);
}

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
    _In_ PVISIBILITY_TESTER VisibilityTester,
    _In_ UINT8 CurrentDepth
    )
{
    PRAYSHADER_HEADER Header;
    PRAYSHADER NextRayShader;
    PPATHTRACER PathTracer;

    ASSERT(Scene != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsNormalFloat(Epsilon));
    ASSERT(IsFiniteFloat(Epsilon));
    ASSERT(IsNormalFloat(MaximumContinueProbability));
    ASSERT(IsFiniteFloat(MaximumContinueProbability));
    ASSERT(IsNormalFloat(MaximumContinueProbability));
    ASSERT(IsFiniteFloat(MaximumContinueProbability));
    ASSERT(MinimumContinueProbability < MaximumContinueProbability);

    PathTracer = (PPATHTRACER) malloc(sizeof(PATHTRACER));

    if (PathTracer == NULL)
    {
        return NULL;
    }

    if (CurrentDepth < MaximumRecursionDepth)
    {
        NextRayShader = PathTracerAllocateInternal(Scene,
                                                   Rng,
                                                   Epsilon,
                                                   MinimumContinueProbability,
                                                   MaximumContinueProbability,
                                                   RussianRouletteStartDepth,
                                                   MaximumRecursionDepth,
                                                   VisibilityTester,
                                                   CurrentDepth + 1);

        if (NextRayShader == NULL)
        {
            free(PathTracer);
            return NULL;
        }
    }
    else
    {
        NextRayShader = NULL;
    }

    if (CurrentDepth <= RussianRouletteStartDepth)
    {
        MinimumContinueProbability = (FLOAT) 1.0;
        MinimumContinueProbability = (FLOAT) 1.0;
    }

    Header = RayShaderHeaderAllocate(Scene,
                                     Rng,
                                     Epsilon,
                                     MinimumContinueProbability,
                                     MaximumContinueProbability,
                                     PathTracerShadeHit,
                                     PathTracerFree);

    if (Header == NULL)
    {
        free(PathTracer);
        PathTracerFreeInternal(NextRayShader, FALSE);
        return NULL;
    }

    PathTracer->Header = Header;
    PathTracer->VisibilityTester = VisibilityTester;
    PathTracer->NextRayShader = NextRayShader;

    return (PRAYSHADER) PathTracer;
}

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
    )
{
    PVISIBILITY_TESTER VisibilityTester;
    PRAYSHADER RayShader;

    ASSERT(Scene != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsNormalFloat(Epsilon));
    ASSERT(IsFiniteFloat(Epsilon));
    ASSERT(IsNormalFloat(MaximumContinueProbability));
    ASSERT(IsFiniteFloat(MaximumContinueProbability));
    ASSERT(IsNormalFloat(MaximumContinueProbability));
    ASSERT(IsFiniteFloat(MaximumContinueProbability));
    ASSERT(MinimumContinueProbability < MaximumContinueProbability);

    VisibilityTester = VisibilityTesterAllocate(Scene, Epsilon);

    if (VisibilityTester == NULL)
    {
        return NULL;
    }

    RayShader = PathTracerAllocateInternal(Scene,
                                           Rng,
                                           Epsilon,
                                           MinimumContinueProbability,
                                           MaximumContinueProbability,
                                           RussianRouletteStartDepth,
                                           MaximumRecursionDepth,
                                           VisibilityTester,
                                           0);

    if (RayShader == NULL)
    {
        VisibilityTesterFree(VisibilityTester);
    }

    return RayShader;
}