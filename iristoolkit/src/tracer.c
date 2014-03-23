/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_tracer.h

Abstract:

    This file contains the prototypes for the TRACER types.

--*/

#include <iristoolkitp.h>

//
// Types
//

typedef struct _TRACER {
    PRAYSHADER_HEADER Header;
    PVISIBILITY_TESTER VisibilityTester;
    PRAYSHADER NextRayShader;
    PRANDOM Rng;
} TRACER, *PTRACER;

typedef CONST TRACER *PCTRACER;

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
    PCTRANSLUCENT_SHADER TranslucentShader;
    PVISIBILITY_TESTER VisibilityTester;
    PCINDIRECT_SHADER IndirectShader;
    PCEMISSIVE_SHADER EmissiveShader;
    COLOR3 ColorWithoutAlpha;
    COLOR3 ComponentColor;
    PTRACER Tracer;
    ISTATUS Status;
    PRANDOM Rng;
    FLOAT Alpha;

    ASSERT(Context != NULL);
    ASSERT(IsNormalFloat(Distance));
    ASSERT(IsFiniteFloat(Distance));
    ASSERT(Distance > (FLOAT) 0.0);
    ASSERT(WorldViewer != NULL);
    ASSERT(WorldHit != NULL);
    ASSERT(ModelViewer != NULL);
    ASSERT(ModelHit != NULL);
    ASSERT(ModelToWorld != NULL);
    ASSERT(Shader != NULL);
    ASSERT(Color != NULL);

    Tracer = (PTRACER) Context;

    TranslucentShader = Shader->TranslucentShader;

    if (TranslucentShader != NULL)
    {
        Status = TranslucentShaderShade(TranslucentShader,
                                        WorldHit,
                                        ModelHit,
                                        AdditionalData,
                                        &Alpha);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        if (Alpha == (FLOAT) 0.0)
        {
            Color4InitializeTransparent(Color);
            return ISTATUS_SUCCESS;
        }
    }
    else
    {
        Alpha = (FLOAT) 1.0;
    }

    Color3InitializeBlack(&ColorWithoutAlpha);

    EmissiveShader = Shader->EmissiveShader;

    if (EmissiveShader != NULL)
    {
        Status = EmissiveShaderShade(EmissiveShader,
                                     WorldHit,
                                     ModelHit,
                                     AdditionalData,
                                     &ComponentColor);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        Color3Add(&ColorWithoutAlpha, &ComponentColor, &ColorWithoutAlpha);
    }

    IndirectShader = Shader->IndirectShader;

    VisibilityTester = Tracer->VisibilityTester;
    Rng = Tracer->Rng;

    if (IndirectShader != NULL)
    {
        Status = IndirectShaderShade(IndirectShader,
                                     WorldHit,
                                     ModelHit,
                                     AdditionalData,
                                     SurfaceNormal,
                                     Rng,
                                     VisibilityTester,
                                     Tracer->NextRayShader,
                                     &ComponentColor);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        Color3Add(&ColorWithoutAlpha, &ComponentColor, &ColorWithoutAlpha);
    }

    return ISTATUS_SUCCESS;
}

STATIC
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RecursiveRayTracerShadeHit(
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
    PCTRANSLUCENT_SHADER TranslucentShader;
    PVISIBILITY_TESTER VisibilityTester;
    PCINDIRECT_SHADER IndirectShader;
    PCEMISSIVE_SHADER EmissiveShader;
    PCDIRECT_SHADER DirectShader;
    COLOR3 ColorWithoutAlpha;
    COLOR3 ComponentColor;
    PTRACER Tracer;
    ISTATUS Status;
    PRANDOM Rng;
    FLOAT Alpha;

    ASSERT(Context != NULL);
    ASSERT(IsNormalFloat(Distance));
    ASSERT(IsFiniteFloat(Distance));
    ASSERT(Distance > (FLOAT) 0.0);
    ASSERT(WorldViewer != NULL);
    ASSERT(WorldHit != NULL);
    ASSERT(ModelViewer != NULL);
    ASSERT(ModelHit != NULL);
    ASSERT(ModelToWorld != NULL);
    ASSERT(Shader != NULL);
    ASSERT(Color != NULL);

    Tracer = (PTRACER) Context;

    TranslucentShader = Shader->TranslucentShader;

    if (TranslucentShader != NULL)
    {
        Status = TranslucentShaderShade(TranslucentShader,
                                        WorldHit,
                                        ModelHit,
                                        AdditionalData,
                                        &Alpha);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        if (Alpha == (FLOAT) 0.0)
        {
            Color4InitializeTransparent(Color);
            return ISTATUS_SUCCESS;
        }
    }
    else
    {
        Alpha = (FLOAT) 1.0;
    }

    Color3InitializeBlack(&ColorWithoutAlpha);

    EmissiveShader = Shader->EmissiveShader;

    if (EmissiveShader != NULL)
    {
        Status = EmissiveShaderShade(EmissiveShader,
                                     WorldHit,
                                     ModelHit,
                                     AdditionalData,
                                     &ComponentColor);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        Color3Add(&ColorWithoutAlpha, &ComponentColor, &ColorWithoutAlpha);
    }

    IndirectShader = Shader->IndirectShader;

    VisibilityTester = Tracer->VisibilityTester;
    Rng = Tracer->Rng;

    if (IndirectShader != NULL)
    {
        Status = IndirectShaderShade(IndirectShader,
                                     WorldHit,
                                     ModelHit,
                                     AdditionalData,
                                     SurfaceNormal,
                                     Rng,
                                     VisibilityTester,
                                     Tracer->NextRayShader,
                                     &ComponentColor);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        Color3Add(&ColorWithoutAlpha, &ComponentColor, &ColorWithoutAlpha);
    }

    DirectShader = Shader->DirectShader;

    if (DirectShader != NULL)
    {
        Status = DirectShaderShade(DirectShader,
                                   WorldHit,
                                   ModelHit,
                                   AdditionalData,
                                   SurfaceNormal,
                                   Rng,
                                   VisibilityTester,
                                   &ComponentColor);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        Color3Add(&ColorWithoutAlpha, &ComponentColor, &ColorWithoutAlpha);
    }

    return ISTATUS_SUCCESS;
}

STATIC
VOID
TracerFreeInternal(
    _Pre_maybenull_ _Post_invalid_ PTRACER Tracer,
    _In_ BOOL FreeVisibilityTester
    )
{
    PTRACER NextTracer;

    if (Tracer == NULL)
    {
        return;
    }

    if (Tracer->NextRayShader != NULL)
    {
        NextTracer = (PTRACER) Tracer->NextRayShader;

        TracerFreeInternal(NextTracer, 
                           FreeVisibilityTester);
    }
    else if (FreeVisibilityTester != FALSE)
    {
        VisibilityTesterFree(Tracer->VisibilityTester);
    }
    
    free(Tracer);
}

STATIC
VOID
TracerFree(
    _Pre_maybenull_ _Post_invalid_ PVOID Context
    )
{
    PTRACER Tracer;

    Tracer = (PTRACER) Context;

    TracerFreeInternal(Tracer, TRUE);
}

_Check_return_
_Ret_maybenull_
STATIC
PTRACER
TracerAllocateInternal(
    _In_ PCSCENE Scene,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon,
    _In_ FLOAT MinimumContinueProbability,
    _In_ FLOAT MaximumContinueProbability,
    _In_ PSHADE_RAY_ROUTINE TracerShadeRoutine,
    _In_ UINT8 RussianRouletteStartDepth,
    _In_ UINT8 MaximumRecursionDepth,
    _In_ PVISIBILITY_TESTER VisibilityTester,
    _In_ UINT8 CurrentDepth
    )
{
    PRAYSHADER_HEADER Header;
    PRAYSHADER NextRayShader;
    PTRACER NextTracer;
    PTRACER Tracer;

    ASSERT(Scene != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsNormalFloat(Epsilon));
    ASSERT(IsFiniteFloat(Epsilon));
    ASSERT(IsNormalFloat(MaximumContinueProbability));
    ASSERT(IsFiniteFloat(MaximumContinueProbability));
    ASSERT(IsNormalFloat(MaximumContinueProbability));
    ASSERT(IsFiniteFloat(MaximumContinueProbability));
    ASSERT(MinimumContinueProbability <= MaximumContinueProbability);
    ASSERT(TracerShadeRoutine != NULL);
    ASSERT(VisibilityTester != NULL);

    Tracer = (PTRACER) malloc(sizeof(TRACER));

    if (Tracer == NULL)
    {
        return NULL;
    }

    if (CurrentDepth < MaximumRecursionDepth)
    {
        NextTracer = TracerAllocateInternal(Scene,
                                            Rng,
                                            Epsilon,
                                            MinimumContinueProbability,
                                            MaximumContinueProbability,
                                            TracerShadeRoutine,
                                            RussianRouletteStartDepth,
                                            MaximumRecursionDepth,
                                            VisibilityTester,
                                            CurrentDepth + 1);

        if (NextTracer == NULL)
        {
            free(Tracer);
            return NULL;
        }

        NextRayShader = (PRAYSHADER) NextTracer;
    }
    else
    {
        NextRayShader = NULL;
        NextTracer = NULL;
    }

    if (CurrentDepth <= RussianRouletteStartDepth ||
        RussianRouletteStartDepth == 0)
    {
        MinimumContinueProbability = (FLOAT) 1.0;
        MinimumContinueProbability = (FLOAT) 1.0;
    }

    Header = RayShaderHeaderAllocate(Scene,
                                     Rng,
                                     Epsilon,
                                     MinimumContinueProbability,
                                     MaximumContinueProbability,
                                     TracerShadeRoutine,
                                     TracerFree);

    if (Header == NULL)
    {
        free(Tracer);
        TracerFreeInternal(NextTracer, FALSE);
        return NULL;
    }

    Tracer->Header = Header;
    Tracer->VisibilityTester = VisibilityTester;
    Tracer->NextRayShader = NextRayShader;
    Tracer->Rng = Rng;

    return Tracer;
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
    PTRACER PathTracer;

    ASSERT(Scene != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsNormalFloat(Epsilon));
    ASSERT(IsFiniteFloat(Epsilon));
    ASSERT(IsNormalFloat(MaximumContinueProbability));
    ASSERT(IsFiniteFloat(MaximumContinueProbability));
    ASSERT(IsNormalFloat(MaximumContinueProbability));
    ASSERT(IsFiniteFloat(MaximumContinueProbability));
    ASSERT(MinimumContinueProbability <= MaximumContinueProbability);

    VisibilityTester = VisibilityTesterAllocate(Scene, Epsilon);

    if (VisibilityTester == NULL)
    {
        return NULL;
    }

    PathTracer = TracerAllocateInternal(Scene,
                                        Rng,
                                        Epsilon,
                                        MinimumContinueProbability,
                                        MaximumContinueProbability,
                                        PathTracerShadeHit,
                                        RussianRouletteStartDepth,
                                        MaximumRecursionDepth,
                                        VisibilityTester,
                                        0);

    if (PathTracer == NULL)
    {
        VisibilityTesterFree(VisibilityTester);
    }

    return (PRAYSHADER) PathTracer;
}

_Check_return_
_Ret_maybenull_
PRAYSHADER
RecursiveRayTracerAllocate(
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
    PTRACER RecursiveRayTracer;

    ASSERT(Scene != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsNormalFloat(Epsilon));
    ASSERT(IsFiniteFloat(Epsilon));
    ASSERT(IsNormalFloat(MaximumContinueProbability));
    ASSERT(IsFiniteFloat(MaximumContinueProbability));
    ASSERT(IsNormalFloat(MaximumContinueProbability));
    ASSERT(IsFiniteFloat(MaximumContinueProbability));
    ASSERT(MinimumContinueProbability <= MaximumContinueProbability);

    VisibilityTester = VisibilityTesterAllocate(Scene, Epsilon);

    if (VisibilityTester == NULL)
    {
        return NULL;
    }

    RecursiveRayTracer = TracerAllocateInternal(Scene,
                                                Rng,
                                                Epsilon,
                                                MinimumContinueProbability,
                                                MaximumContinueProbability,
                                                RecursiveRayTracerShadeHit,
                                                RussianRouletteStartDepth,
                                                MaximumRecursionDepth,
                                                VisibilityTester,
                                                0);

    if (RecursiveRayTracer == NULL)
    {
        VisibilityTesterFree(VisibilityTester);
    }

    return (PRAYSHADER) RecursiveRayTracer;
}

