/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    rayshader.c

Abstract:

    This file contains the function definitions for the RAYSHADER type.

--*/

#include <irisshadingmodelp.h>

//
// Types
//

struct _RAYSHADER_HEADER {
    PSHADE_RAY_ROUTINE ShadeRayRoutine;
    PFREE_ROUTINE RayShaderFreeRoutine;
    FLOAT MinimumContinueProbability;
    FLOAT MaximumContinueProbability;
    BOOL DoRouletteTermination;
    PRAYTRACER RayTracer;
    PCSCENE Scene;
    PRANDOM Rng;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISSHADINGMODELAPI
PRAYSHADER_HEADER
RayShaderHeaderAllocate(
    _In_ PCSCENE Scene,
    _In_ PRANDOM Rng,
    _In_ FLOAT MinimumContinueProbability,
    _In_ FLOAT MaximumContinueProbability,
    _In_ PSHADE_RAY_ROUTINE ShadeRayRoutine,
    _In_opt_ PFREE_ROUTINE RayShaderFreeRoutine
    )
{
    PRAYSHADER_HEADER RayShaderHeader;
    BOOL DoRouletteTermination;
    PRAYTRACER RayTracer;

    ASSERT(Scene != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsNormalFloat(MinimumContinueProbability));
    ASSERT(IsFiniteFloat(MinimumContinueProbability));
    ASSERT((FLOAT) 0.0 < MinimumContinueProbability);
    ASSERT(MinimumContinueProbability < (FLOAT) 1.0);
    ASSERT(IsNormalFloat(MaximumContinueProbability));
    ASSERT(IsFiniteFloat(MaximumContinueProbability));
    ASSERT((FLOAT) 0.0 < MaximumContinueProbability);
    ASSERT(MaximumContinueProbability < (FLOAT) 1.0);
    ASSERT(MinimumContinueProbability < MaximumContinueProbability);
    ASSERT(ShadeRayRoutine != NULL);

    RayShaderHeader = (PRAYSHADER_HEADER) malloc(sizeof(RAYSHADER_HEADER));

    if (RayShaderHeader == NULL)
    {
        return NULL;
    }

    RayTracer = RayTracerAllocate();

    if (RayTracer == NULL)
    {
        free(RayShaderHeader);
        return NULL;
    }

    DoRouletteTermination = MinimumContinueProbability != (FLOAT) 1.0 && 
                            MaximumContinueProbability != (FLOAT) 1.0;

    RayShaderHeader->MinimumContinueProbability = MinimumContinueProbability;
    RayShaderHeader->MaximumContinueProbability = MaximumContinueProbability;
    RayShaderHeader->DoRouletteTermination = DoRouletteTermination;
    RayShaderHeader->RayShaderFreeRoutine = RayShaderFreeRoutine;
    RayShaderHeader->ShadeRayRoutine = ShadeRayRoutine;
    RayShaderHeader->RayTracer = RayTracer;
    RayShaderHeader->Scene = Scene;
    RayShaderHeader->Rng = Rng;

    return RayShaderHeader;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
RayShaderTraceRayMontecarlo(
    _Inout_ PRAYSHADER RayShader,
    _In_ PCRAY WorldRay,
    _In_ FLOAT PreferredContinueProbability,
    _Out_ PCOLOR3 Color
    )
{
    PRAYSHADER_HEADER Header;
    PCGEOMETRY_HIT *HitList;
    RAY NormalizedWorldRay;
    PRAYTRACER RayTracer;
    FLOAT NextRandom;
    SIZE_T HitCount;
    ISTATUS Status;

    ASSERT(IsNormalFloat(PreferredContinueProbability));
    ASSERT(IsFiniteFloat(PreferredContinueProbability));
    ASSERT(RayShader != NULL);
    ASSERT(WorldRay != NULL);
    ASSERT(Color != NULL);

    Header = RayShader->RayShaderHeader;

    if (Header->DoRouletteTermination != FALSE)
    {
        PreferredContinueProbability = MinFloat(Header->MaximumContinueProbability, 
                                                PreferredContinueProbability);

        PreferredContinueProbability = MaxFloat(Header->MinimumContinueProbability, 
                                                PreferredContinueProbability);

        if (PreferredContinueProbability == (FLOAT) 0.0)
        {
            Color3InitializeBlack(Color);
            return ISTATUS_SUCCESS;
        }

        NextRandom = RandomGenerateFloat(Header->Rng, (FLOAT) 0.0, (FLOAT) 1.0);

        if (PreferredContinueProbability < NextRandom)
        {
            Color3InitializeBlack(Color);
            return ISTATUS_SUCCESS;
        }   
    }

    VectorNormalize(&WorldRay->Direction, &NormalizedWorldRay.Direction);
    NormalizedWorldRay.Origin = WorldRay->Origin;

    RayTracer = Header->RayTracer;

    RayTracerClearResults(RayTracer);

    Status = SceneTrace(Header->Scene,
                        &NormalizedWorldRay,
                        RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    RayTracerGetResults(RayTracer,
                        TRUE,
                        &HitList,
                        &HitCount);

    if (HitCount == 0)
    {
        Color3InitializeBlack(Color);
        return ISTATUS_SUCCESS;
    }

    Status = Header->ShadeRayRoutine(RayShader,
                                     &NormalizedWorldRay,
                                     HitList,
                                     HitCount,
                                     Color);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (Header->DoRouletteTermination != FALSE)
    {
        Color3DivideByScalar(Color, PreferredContinueProbability, Color);
    }

    return ISTATUS_SUCCESS;
}

IRISSHADINGMODELAPI
VOID
RayShaderFree(
    _Pre_maybenull_ _Post_invalid_ PRAYSHADER RayShader
    )
{
    PRAYSHADER_HEADER Header;
    PFREE_ROUTINE FreeRoutine;

    if (RayShader == NULL)
    {
        return;
    }

    Header = RayShader->RayShaderHeader;
    FreeRoutine = Header->RayShaderFreeRoutine;

    RayTracerFree(Header->RayTracer);
    free(Header);

    if (FreeRoutine != NULL)
    {
        FreeRoutine(RayShader);
    }
}