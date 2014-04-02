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

typedef struct _COMMON_TRACER {
    TRACER TracerHeader;
    PVISIBILITY_TESTER VisibilityTester;
    PRANDOM Rng;
} COMMON_TRACER, *PCOMMON_TRACER;

typedef CONST COMMON_TRACER *PCCOMMON_TRACER;

//
// Static
//

STATIC
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PathTracerShadeHit(
    _In_opt_ PCVOID Context,
    _Inout_ PRAYSHADER NextRayShader,
    _In_ UINT8 CurrentDepth,
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
    PCOMMON_TRACER Tracer;
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

    Tracer = (PCOMMON_TRACER) Context;

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
                                     WorldViewer,
                                     ModelViewer,
                                     AdditionalData,
                                     SurfaceNormal,
                                     Rng,
                                     VisibilityTester,
                                     NextRayShader,
                                     &ComponentColor);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        Color3Add(&ColorWithoutAlpha, &ComponentColor, &ColorWithoutAlpha);
    }

    Color4InitializeFromColor3(Color, &ColorWithoutAlpha, Alpha);

    return ISTATUS_SUCCESS;
}

STATIC
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RecursiveRayTracerShadeHit(
    _In_opt_ PCVOID Context,
    _Inout_ PRAYSHADER NextRayShader,
    _In_ UINT8 CurrentDepth,
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
    PCOMMON_TRACER Tracer;
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

    Tracer = (PCOMMON_TRACER) Context;

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
                                     WorldViewer,
                                     ModelViewer,
                                     AdditionalData,
                                     SurfaceNormal,
                                     Rng,
                                     VisibilityTester,
                                     NextRayShader,
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
                                   WorldViewer,
                                   ModelViewer,
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

    Color4InitializeFromColor3(Color, &ColorWithoutAlpha, Alpha);

    return ISTATUS_SUCCESS;
}

STATIC
VOID
CommonTracerFree(
    _Pre_maybenull_ _Post_invalid_ PVOID Context
    )
{
    PCOMMON_TRACER Tracer;

    ASSERT(Context != NULL);

    Tracer = (PCOMMON_TRACER) Context;

    RayShaderFree(Tracer->TracerHeader.RayShader);
    VisibilityTesterFree(Tracer->VisibilityTester);
    free(Tracer);
}

_Check_return_
_Ret_maybenull_
STATIC
PTRACER
CommonTracerAllocateInternal(
    _In_ PCSCENE Scene,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon,
    _In_ FLOAT MinimumContinueProbability,
    _In_ FLOAT MaximumContinueProbability,
    _In_ PSHADE_RAY_ROUTINE TracerShadeRoutine,
    _In_ UINT8 RussianRouletteStartDepth,
    _In_ UINT8 MaximumRecursionDepth
    )
{
    PVISIBILITY_TESTER VisibilityTester;
    PCOMMON_TRACER Tracer;
    PRAYSHADER RayShader;

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

    Tracer = (PCOMMON_TRACER) malloc(sizeof(COMMON_TRACER));

    if (Tracer == NULL)
    {
        return NULL;
    }

    VisibilityTester = VisibilityTesterAllocate(Scene, Epsilon);

    if (VisibilityTester == NULL)
    {
        free(Tracer);
        return NULL;
    }

    RayShader = RayShaderAllocate(Tracer,
                                  TracerShadeRoutine,
                                  Scene,
                                  Rng,
                                  Epsilon,
                                  MinimumContinueProbability,
                                  MaximumContinueProbability,
                                  RussianRouletteStartDepth,
                                  MaximumRecursionDepth);

    if (RayShader == NULL)
    {
        VisibilityTesterFree(VisibilityTester);
        free(Tracer);
        return NULL;
    }

    Tracer->TracerHeader.FreeRoutine = TracerFree;
    Tracer->TracerHeader.RayShader = RayShader;
    Tracer->VisibilityTester = VisibilityTester;
    Tracer->Rng = Rng;

    return (PTRACER) Tracer;
}

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
PTRACER
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

    PathTracer = CommonTracerAllocateInternal(Scene,
                                              Rng,
                                              Epsilon,
                                              MinimumContinueProbability,
                                              MaximumContinueProbability,
                                              PathTracerShadeHit,
                                              RussianRouletteStartDepth,
                                              MaximumRecursionDepth);

    return (PTRACER) PathTracer;
}

_Check_return_
_Ret_maybenull_
PTRACER
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

    RecursiveRayTracer = CommonTracerAllocateInternal(Scene,
                                                      Rng,
                                                      Epsilon,
                                                      MinimumContinueProbability,
                                                      MaximumContinueProbability,
                                                      RecursiveRayTracerShadeHit,
                                                      RussianRouletteStartDepth,
                                                      MaximumRecursionDepth);

    return (PTRACER) RecursiveRayTracer;
}

