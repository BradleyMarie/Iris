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

struct _RAYSHADER {
    BOOL DoRouletteTermination;
    PCOLOR3 CurrentReflectanceEstimatePointer;
    FLOAT MinimumContinueProbability;
    FLOAT MaximumContinueProbability;
    PRANDOM Rng;
    PCSCENE Scene;
    PRAYTRACER RayTracer;
    FLOAT Epsilon;
    UINT8 CurrentDepth;
    PCVOID Context;
    PRAYSHADER NextRayShader;
    PSHADE_RAY_ROUTINE ShadeRayRoutine;
    COLOR3 CurrentReflectanceEstimate;
};

//
// Static Variables
//

STATIC
CONST
MATRIX IdentityMatrix = { (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          (FLOAT) 0.0,
                          &IdentityMatrix };

//
// Static Functions
//

_Ret_maybenull_
STATIC
PRAYSHADER
RayShaderAllocateInternal(
    _In_opt_ PCVOID Context,
    _In_ PSHADE_RAY_ROUTINE ShadeRayRoutine,
    _In_ PCSCENE Scene,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon,
    _In_ PCOLOR3 CurrentReflectanceEstimatePointer,
    _In_ FLOAT MinimumContinueProbability,
    _In_ FLOAT MaximumContinueProbability,
    _In_ UINT8 RussianRouletteStartDepth,
    _In_ UINT8 MaximumRecursionDepth,
    _In_ UINT8 CurrentDepth
    )
{
    PCOLOR3 NextReflectanceEstimatePointer;
    BOOL DoRouletteTermination;
    PRAYSHADER NextRayShader;
    PRAYSHADER RayShader;
    PRAYTRACER RayTracer;

    ASSERT(ShadeRayRoutine != NULL);
    ASSERT(Scene != NULL);
    ASSERT(Rng != NULL);
    ASSERT(IsNormalFloat(Epsilon) != FALSE);
    ASSERT(IsFiniteFloat(Epsilon) != FALSE);
    ASSERT(IsNormalFloat(MinimumContinueProbability) != FALSE);
    ASSERT(IsFiniteFloat(MinimumContinueProbability) != FALSE);
    ASSERT(IsNormalFloat(MaximumContinueProbability) != FALSE);
    ASSERT(IsFiniteFloat(MaximumContinueProbability) != FALSE);
    ASSERT(CurrentDepth <= MaximumRecursionDepth);
    ASSERT(MinimumContinueProbability <= MaximumContinueProbability);
    ASSERT(MinimumContinueProbability >= (FLOAT) 0.0);
    ASSERT(MinimumContinueProbability <= (FLOAT) 1.0);
    ASSERT(MaximumContinueProbability >= (FLOAT) 0.0);
    ASSERT(MaximumContinueProbability <= (FLOAT) 1.0);
    ASSERT(Epsilon >= (FLOAT) 0.0);

    RayShader = (PRAYSHADER) malloc(sizeof(RAYSHADER));

    if (RayShader == NULL)
    {
        return NULL;
    }

    if (CurrentReflectanceEstimatePointer == NULL)
    {
        NextReflectanceEstimatePointer = &RayShader->CurrentReflectanceEstimate;
    }
    else
    {
        NextReflectanceEstimatePointer = CurrentReflectanceEstimatePointer;
    }

    if (CurrentDepth < MaximumRecursionDepth)
    {
        NextRayShader = RayShaderAllocateInternal(Context,
                                                  ShadeRayRoutine,
                                                  Scene,
                                                  Rng,
                                                  Epsilon,
                                                  NextReflectanceEstimatePointer,
                                                  MinimumContinueProbability,
                                                  MaximumContinueProbability,
                                                  RussianRouletteStartDepth,
                                                  MaximumRecursionDepth,
                                                  CurrentDepth + 1);

        if (NextRayShader == NULL)
        {
            free(RayShader);
            return NULL;
        }
    }
    else
    {
        NextRayShader = NULL;
    }

    RayTracer = RayTracerAllocate();

    if (RayTracer == NULL)
    {
        RayShaderFree(NextRayShader);
        free(RayShader);
        return NULL;
    }

    if (RussianRouletteStartDepth <= CurrentDepth &&
        RussianRouletteStartDepth != DISABLE_RUSSAIAN_ROULETTE_TERMINATION)
    {
        DoRouletteTermination = MinimumContinueProbability != (FLOAT) 1.0;
    }
    else
    {
        DoRouletteTermination = FALSE;
    }

    RayShader->DoRouletteTermination = DoRouletteTermination;
    RayShader->CurrentReflectanceEstimatePointer = CurrentReflectanceEstimatePointer;
    RayShader->MinimumContinueProbability = MinimumContinueProbability;
    RayShader->MaximumContinueProbability = MaximumContinueProbability;
    RayShader->Rng = Rng;
    RayShader->Scene = Scene;
    RayShader->RayTracer = RayTracer;
    RayShader->Epsilon = Epsilon;
    RayShader->CurrentDepth = CurrentDepth;
    RayShader->Context = Context;
    RayShader->NextRayShader = NextRayShader;
    RayShader->ShadeRayRoutine = ShadeRayRoutine;

    Color3InitializeWhite(&RayShader->CurrentReflectanceEstimate);

    return RayShader;
}

SFORCEINLINE
VOID
RayShaderRestoreReflectanceEstimate(
    _Inout_ PRAYSHADER RayShader
    )
{
    ASSERT(RayShader != NULL);

    if (RayShader->DoRouletteTermination == FALSE)
    {
        return;
    }

    if (RayShader->CurrentDepth == 0)
    {
        Color3InitializeWhite(&RayShader->CurrentReflectanceEstimate);
    }
    else
    {
        *RayShader->CurrentReflectanceEstimatePointer = RayShader->CurrentReflectanceEstimate;
    }
}

SFORCEINLINE
VOID
RayShaderSaveReflectanceEstimateAndComputeContinueProbability(
    _Inout_ PRAYSHADER RayShader,
    _In_ PCCOLOR3 ReflectanceHint,
    _Out_ PFLOAT ContinueProbability
    )
{
    PCOLOR3 CurrentReflectanceEstimate;
    FLOAT LightReturnedFromPath;
    FLOAT LightReturnedFromRay;

    ASSERT(RayShader != NULL);
    ASSERT(ReflectanceHint != NULL);
    ASSERT(ContinueProbability != NULL);
    ASSERT(RayShader->DoRouletteTermination != FALSE);
    ASSERT(RayShader->MinimumContinueProbability <= RayShader->MaximumContinueProbability);
    ASSERT(RayShader->MinimumContinueProbability >= (FLOAT) 0.0);
    ASSERT(RayShader->MinimumContinueProbability <= (FLOAT) 1.0);
    ASSERT(RayShader->MaximumContinueProbability >= (FLOAT) 0.0);
    ASSERT(RayShader->MaximumContinueProbability <= (FLOAT) 1.0);

    CurrentReflectanceEstimate = RayShader->CurrentReflectanceEstimatePointer;

    if (RayShader->CurrentDepth == 0)
    {
        *CurrentReflectanceEstimate = *ReflectanceHint;
    }
    else
    {
        RayShader->CurrentReflectanceEstimate = *CurrentReflectanceEstimate;

        Color3ScaleByColor(CurrentReflectanceEstimate,
                           ReflectanceHint,
                           CurrentReflectanceEstimate);
    }

    LightReturnedFromPath = Color3AverageComponents(CurrentReflectanceEstimate);
    LightReturnedFromRay = Color3AverageComponents(ReflectanceHint);

    *ContinueProbability = LightReturnedFromPath / LightReturnedFromRay;
}

//
// Functions
//

_Ret_maybenull_
PRAYSHADER
RayShaderAllocate(
    _In_opt_ PCVOID Context,
    _In_ PSHADE_RAY_ROUTINE ShadeRayRoutine,
    _In_ PCSCENE Scene,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon,
    _In_ FLOAT MinimumContinueProbability,
    _In_ FLOAT MaximumContinueProbability,
    _In_ UINT8 RussianRouletteStartDepth,
    _In_ UINT8 MaximumRecursionDepth
    )
{
    PRAYSHADER RayShader;

    if (ShadeRayRoutine == NULL ||
        Scene == NULL ||
        Rng == NULL ||
        IsNormalFloat(Epsilon) == FALSE ||
        IsFiniteFloat(Epsilon) == FALSE ||
        IsNormalFloat(MinimumContinueProbability) == FALSE ||
        IsFiniteFloat(MinimumContinueProbability) == FALSE ||
        IsNormalFloat(MaximumContinueProbability) == FALSE ||
        IsFiniteFloat(MaximumContinueProbability) == FALSE ||
        MinimumContinueProbability > MaximumContinueProbability ||
        MinimumContinueProbability < (FLOAT) 0.0 ||
        MinimumContinueProbability > (FLOAT) 1.0 ||
        MaximumContinueProbability < (FLOAT) 0.0 ||
        MaximumContinueProbability > (FLOAT) 1.0 ||
        Epsilon < (FLOAT) 0.0)
    {
        return NULL;
    }

    RayShader = RayShaderAllocateInternal(Context,
                                          ShadeRayRoutine,
                                          Scene,
                                          Rng,
                                          Epsilon,
                                          NULL,
                                          MinimumContinueProbability,
                                          MaximumContinueProbability,
                                          RussianRouletteStartDepth,
                                          MaximumRecursionDepth,
                                          0);

    return RayShader;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
RayShaderTraceRayMontecarlo(
    _Inout_ PRAYSHADER RayShader,
    _In_ PCRAY WorldRay,
    _In_ PCCOLOR3 ReflectanceHint,
    _Out_ PCOLOR3 Color
    )
{
    PCSHARED_GEOMETRY_HIT SharedGeometryHit;
    PSURFACE_NORMAL SurfaceNormalPointer;
    SURFACE_NORMAL SurfaceNormal;
    PCDRAWING_SHAPE DrawingShape;
    PCGEOMETRY_HIT GeometryHit;
    FLOAT ContinueProbability;
    PCGEOMETRY_HIT *HitList;
    RAY NormalizedWorldRay;
    PCMATRIX ModelToWorld;
    PCVOID AdditionalData;
    PRAYTRACER RayTracer;
    COLOR4 BlendedColor;
    SIZE_T NumberOfHits;
    VECTOR3 ModelViewer;
    PCTEXTURE Texture;
    FLOAT NextRandom;
    PCSHADER Shader;
    PCNORMAL Normal;
    POINT3 WorldHit;
    POINT3 ModelHit;
    COLOR4 HitColor;
    FLOAT Distance;
    ISTATUS Status;
    SIZE_T Index;

    ASSERT(RayShader != NULL);
    ASSERT(WorldRay != NULL);
    ASSERT(ReflectanceHint != NULL);
    ASSERT(Color != NULL);

    if (RayShader->DoRouletteTermination != FALSE)
    {
        RayShaderSaveReflectanceEstimateAndComputeContinueProbability(RayShader,
                                                                      ReflectanceHint,
                                                                      &ContinueProbability);

        if (ContinueProbability == (FLOAT) 0.0)
        {
            RayShaderRestoreReflectanceEstimate(RayShader);
            Color3InitializeBlack(Color);
            return ISTATUS_SUCCESS;
        }

        NextRandom = RandomGenerateFloat(RayShader->Rng,
                                         (FLOAT) 0.0,
                                         (FLOAT) 1.0);

        if (ContinueProbability < NextRandom)
        {
            RayShaderRestoreReflectanceEstimate(RayShader);
            Color3InitializeBlack(Color);
            return ISTATUS_SUCCESS;
        }   
    }

    VectorNormalize(&WorldRay->Direction, &NormalizedWorldRay.Direction);
    NormalizedWorldRay.Origin = WorldRay->Origin;

    RayTracer = RayShader->RayTracer;

    RayTracerClearResults(RayTracer);

    Status = SceneTrace(RayShader->Scene,
                        &NormalizedWorldRay,
                        RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        RayShaderRestoreReflectanceEstimate(RayShader);
        return Status;
    }

    RayTracerGetResults(RayTracer,
                        TRUE,
                        &HitList,
                        &NumberOfHits);

    if (NumberOfHits == 0)
    {
        RayShaderRestoreReflectanceEstimate(RayShader);
        Color3InitializeBlack(Color);
        return ISTATUS_SUCCESS;
    }

    Color4InitializeTransparent(&BlendedColor);

    for (Index = 0; 
         Index < NumberOfHits && BlendedColor.Alpha < (FLOAT) 1.0; 
         Index++)
    {
        GeometryHit = HitList[Index];
        Distance = GeometryHit->Distance;

        if (Distance <= RayShader->Epsilon)
        {
            continue;
        }

        DrawingShape = (PCDRAWING_SHAPE) GeometryHit->Shape;

        Texture = DrawingShapeGetTexture(DrawingShape,
                                         GeometryHit->FaceHit);

        if (Texture == NULL)
        {
            continue;
        }

        RayEndpoint(WorldRay, Distance, &WorldHit);

        SharedGeometryHit = GeometryHit->SharedGeometryHit;

        SharedGeometryHitComputeModelHit(SharedGeometryHit,
                                         &WorldHit,
                                         &ModelHit);

        AdditionalData = GeometryHit->AdditionalData;

        Shader = TextureGetShader(Texture,
                                  &WorldHit,
                                  &ModelHit,
                                  AdditionalData);

        if (Shader == NULL)
        {
            continue;
        }

        SharedGeometryHitComputeModelViewer(SharedGeometryHit,
                                            &WorldRay->Direction,
                                            &ModelViewer);

        ModelToWorld = SharedGeometryHitGetModelToWorld(SharedGeometryHit);

        Normal = DrawingShapeGetNormal(DrawingShape,
                                       GeometryHit->FaceHit);

        if (Normal != NULL)
        {
            SurfaceNormalInitialize(&SurfaceNormal,
                                    Normal,
                                    &ModelHit,
                                    ModelToWorld,
                                    AdditionalData);

            SurfaceNormalPointer = &SurfaceNormal;
        }
        else
        {
            SurfaceNormalPointer = NULL;
        }

        if (ModelToWorld == NULL)
        {
            ModelToWorld = &IdentityMatrix;
        }

        Status = RayShader->ShadeRayRoutine(RayShader->Context,
                                            RayShader->NextRayShader,
                                            RayShader->CurrentDepth,
                                            Distance,
                                            &NormalizedWorldRay.Direction,
                                            &WorldHit,
                                            &ModelViewer,
                                            &ModelHit,
                                            ModelToWorld,
                                            AdditionalData,
                                            Shader,
                                            SurfaceNormalPointer,
                                            &HitColor);

        if (Status != ISTATUS_SUCCESS)
        {
            RayShaderRestoreReflectanceEstimate(RayShader);
            return Status;
        }

        Color4Over(&BlendedColor, &HitColor, &BlendedColor);
    }

    Color3InitializeFromColor4(Color, &BlendedColor);

    if (RayShader->DoRouletteTermination != FALSE)
    {
        Color3DivideByScalar(Color, ContinueProbability, Color);
    }

    RayShaderRestoreReflectanceEstimate(RayShader);
    return ISTATUS_SUCCESS;
}

IRISSHADINGMODELAPI
VOID
RayShaderFree(
    _Pre_maybenull_ _Post_invalid_ PRAYSHADER RayShader
    )
{
    PRAYSHADER NextRayShader;

    if (RayShader == NULL)
    {
        return;
    }

    NextRayShader = RayShader->NextRayShader;

    if (NextRayShader != NULL)
    {
        RayShaderFree(NextRayShader);
    }

    RayTracerFree(RayShader->RayTracer);
    free(RayShader);
}