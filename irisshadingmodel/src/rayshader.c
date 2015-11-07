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
    PCOLOR3 PathThroughputPointer;
    FLOAT MinimumContinueProbability;
    FLOAT MaximumContinueProbability;
    PRANDOM Rng;
    PSCENE Scene;
    PRAYTRACER_OWNER RayTracerOwner;
    FLOAT Epsilon;
    UINT8 CurrentDepth;
    PCVOID Context;
    PRAYSHADER NextRayShader;
    PSHADE_RAY_ROUTINE ShadeRayRoutine;
    COLOR3 OldPathThroughput;
};

//
// Static Functions
//

_Ret_maybenull_
STATIC
PRAYSHADER
RayShaderAllocateInternal(
    _In_opt_ PCVOID Context,
    _In_ PSHADE_RAY_ROUTINE ShadeRayRoutine,
    _In_ PSCENE Scene,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon,
    _In_opt_ PCOLOR3 PathThroughputPointer,
    _In_ FLOAT MinimumContinueProbability,
    _In_ FLOAT MaximumContinueProbability,
    _In_ UINT8 RussianRouletteStartDepth,
    _In_ UINT8 MaximumRecursionDepth,
    _In_ UINT8 CurrentDepth
    )
{
    VECTOR3 TemporaryDirection;
    PRAYSHADER NextRayShader;
    POINT3 TemporaryOrigin;
    PRAYSHADER RayShader;
    PRAYTRACER_OWNER RayTracerOwner;
    RAY TemporaryRay;
    ISTATUS Status;

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

    if (PathThroughputPointer == NULL)
    {
        PathThroughputPointer = &RayShader->OldPathThroughput;
    }

    if (CurrentDepth < MaximumRecursionDepth)
    {
        NextRayShader = RayShaderAllocateInternal(Context,
                                                  ShadeRayRoutine,
                                                  Scene,
                                                  Rng,
                                                  Epsilon,
                                                  PathThroughputPointer,
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

    TemporaryDirection = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    TemporaryOrigin = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    TemporaryRay = RayCreate(TemporaryOrigin, TemporaryDirection);

    Status = RayTracerOwnerAllocate(TemporaryRay, &RayTracerOwner);

    if (Status != ISTATUS_SUCCESS)
    {
        RayShaderFree(NextRayShader);
        free(RayShader);
        return NULL;
    }

    if (RussianRouletteStartDepth == DISABLE_RUSSAIAN_ROULETTE_TERMINATION ||
        CurrentDepth < RussianRouletteStartDepth)
    {
        MinimumContinueProbability = (FLOAT) 1.0;
        MaximumContinueProbability = (FLOAT) 1.0;
    }

    RandomReference(Rng);
    SceneReference(Scene);

    RayShader->PathThroughputPointer = PathThroughputPointer;
    RayShader->MinimumContinueProbability = MinimumContinueProbability;
    RayShader->MaximumContinueProbability = MaximumContinueProbability;
    RayShader->Rng = Rng;
    RayShader->Scene = Scene;
    RayShader->RayTracerOwner = RayTracerOwner;
    RayShader->Epsilon = Epsilon;
    RayShader->CurrentDepth = CurrentDepth;
    RayShader->Context = Context;
    RayShader->NextRayShader = NextRayShader;
    RayShader->ShadeRayRoutine = ShadeRayRoutine;

    RayShader->OldPathThroughput = Color3InitializeWhite();

    return RayShader;
}

SFORCEINLINE
VOID
RayShaderPopPathThroughput(
    _Inout_ PRAYSHADER RayShader
    )
{
    ASSERT(RayShader != NULL);

    if (RayShader->CurrentDepth == 0)
    {
        *RayShader->PathThroughputPointer = Color3InitializeWhite();
    }
    else
    {
        *RayShader->PathThroughputPointer = RayShader->OldPathThroughput;
    }
}

SFORCEINLINE
VOID
RayShaderPushPathThroughputAndComputeContinueProbability(
    _Inout_ PRAYSHADER RayShader,
    _In_ COLOR3 Transmittance,
    _Out_ PFLOAT ContinueProbability
    )
{
    PCOLOR3 PathThroughputPointer;
    FLOAT RayContinueProbability;

    ASSERT(RayShader != NULL);
    ASSERT(ContinueProbability != NULL);
    ASSERT(RayShader->MinimumContinueProbability <= RayShader->MaximumContinueProbability);
    ASSERT(RayShader->MinimumContinueProbability >= (FLOAT) 0.0);
    ASSERT(RayShader->MinimumContinueProbability <= (FLOAT) 1.0);
    ASSERT(RayShader->MaximumContinueProbability >= (FLOAT) 0.0);
    ASSERT(RayShader->MaximumContinueProbability <= (FLOAT) 1.0);

    PathThroughputPointer = RayShader->PathThroughputPointer;

    if (RayShader->CurrentDepth != 0)
    {
        RayShader->OldPathThroughput = *PathThroughputPointer;
    }

    *PathThroughputPointer = Color3ScaleByColor(Transmittance,
                                                *PathThroughputPointer);

    if (RayShader->MinimumContinueProbability == (FLOAT) 1.0)
    {
        *ContinueProbability = (FLOAT) 1.0;
        return;
    }

    RayContinueProbability = MinFloat(Color3AverageComponents(*PathThroughputPointer),
                                      RayShader->MaximumContinueProbability);

    RayContinueProbability = MaxFloat(RayShader->MinimumContinueProbability,
                                      RayContinueProbability);

    if (RayContinueProbability != (FLOAT) 0.0)
    {
        *PathThroughputPointer = Color3DivideByScalar(*PathThroughputPointer,
                                                      RayContinueProbability);
    }
    else
    {
        *PathThroughputPointer = Color3InitializeBlack();
    }

    *ContinueProbability = RayContinueProbability;
}

//
// Functions
//

_Check_return_
_Ret_maybenull_
PRAYSHADER
RayShaderAllocate(
    _In_opt_ PCVOID Context,
    _In_ PSHADE_RAY_ROUTINE ShadeRayRoutine,
    _In_ PSCENE Scene,
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
    _In_ RAY WorldRay,
    _In_ COLOR3 Transmittance,
    _Out_ PCOLOR3 Color
    )
{
    PSURFACE_NORMAL SurfaceNormalPointer;
    SURFACE_NORMAL SurfaceNormal;
    PCDRAWING_SHAPE DrawingShape;
    TEXTURE_SHADER TextureShader;
    FLOAT ContinueProbability;
    PCVOID AdditionalData;
    PCMATRIX ModelToWorld;
    PCSHAPE_HIT ShapeHit;
    POINT3 WorldHitPoint;
    POINT3 ModelHitPoint;
    PRAYTRACER RayTracer;
    VECTOR3 ModelViewer;
    COLOR4 BlendedColor;
    PCTEXTURE Texture;
    FLOAT NextRandom;
    PCNORMAL Normal;
    COLOR4 HitColor;
    FLOAT Distance;
    ISTATUS Status;

    if (RayShader == NULL ||
        Color == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT;
    }

    RayShaderPushPathThroughputAndComputeContinueProbability(RayShader,
                                                             Transmittance,
                                                             &ContinueProbability);

    if (ContinueProbability == (FLOAT) 0.0)
    {
        RayShaderPopPathThroughput(RayShader);
        *Color = Color3InitializeBlack();
        return ISTATUS_SUCCESS;
    }

    if (ContinueProbability < (FLOAT) 1.0)
    {
        Status = RandomGenerateFloat(RayShader->Rng,
                                     (FLOAT) 0.0,
                                     (FLOAT) 1.0,
                                     &NextRandom);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        if (ContinueProbability <= NextRandom)
        {
            RayShaderPopPathThroughput(RayShader);
            *Color = Color3InitializeBlack();
            return ISTATUS_SUCCESS;
        }
    }

    Status = RayTracerOwnerSetRay(RayShader->RayTracerOwner, 
                                  WorldRay, 
                                  TRUE);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    RayTracer = RayTracerOwnerGetRayTracer(RayShader->RayTracerOwner);

    Status = SceneTrace(RayShader->Scene,
                        RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        RayShaderPopPathThroughput(RayShader);
        return Status;
    }

    RayTracerOwnerSort(RayShader->RayTracerOwner);

    Status = RayTracerOwnerGetNextHit(RayShader->RayTracerOwner,
                                      &ShapeHit,
                                      &ModelViewer,
                                      &ModelHitPoint,
                                      &WorldHitPoint,
                                      &ModelToWorld);

    if (Status == ISTATUS_NO_MORE_DATA)
    {
        RayShaderPopPathThroughput(RayShader);
        *Color = Color3InitializeBlack();
        return ISTATUS_SUCCESS;
    }

    BlendedColor = Color4InitializeTransparent();

    while (Status == ISTATUS_SUCCESS && BlendedColor.Alpha < (FLOAT) 1.0)
    {
        Distance = ShapeHit->Distance;

        if (Distance <= RayShader->Epsilon)
        {
            Status = RayTracerOwnerGetNextHit(RayShader->RayTracerOwner,
                                              &ShapeHit,
                                              &ModelViewer,
                                              &ModelHitPoint,
                                              &WorldHitPoint,
                                              &ModelToWorld);
            continue;
        }

        DrawingShape = (PCDRAWING_SHAPE) ShapeHit->Shape;

        Texture = DrawingShapeGetTexture(DrawingShape,
                                         ShapeHit->FaceHit);

        if (Texture == NULL)
        {
            Status = RayTracerOwnerGetNextHit(RayShader->RayTracerOwner,
                                              &ShapeHit,
                                              &ModelViewer,
                                              &ModelHitPoint,
                                              &WorldHitPoint,
                                              &ModelToWorld);
            continue;
        }

        Normal = DrawingShapeGetNormal(DrawingShape,
                                       ShapeHit->FaceHit);

        AdditionalData = ShapeHit->AdditionalData;

        if (Normal != NULL)
        {
            SurfaceNormalInitialize(&SurfaceNormal,
                                    Normal,
                                    ModelHitPoint,
                                    ModelToWorld,
                                    AdditionalData);

            SurfaceNormalPointer = &SurfaceNormal;
        }
        else
        {
            SurfaceNormalPointer = NULL;
        }

        TextureShaderInitialize(&TextureShader,
                                RayShader->ShadeRayRoutine,
                                RayShader->Context,
                                RayShader->NextRayShader,
                                RayShader->CurrentDepth,
                                Distance,
                                WorldRay.Direction,
                                WorldHitPoint,
                                ModelViewer,
                                ModelHitPoint,
                                ModelToWorld,
                                AdditionalData,
                                SurfaceNormalPointer,
                                &HitColor);

        HitColor = Color4InitializeTransparent();

        Status = TextureShade(Texture,
                              WorldHitPoint,
                              ModelHitPoint,
                              AdditionalData,
                              &TextureShader);

        if (Status != ISTATUS_SUCCESS)
        {
            RayShaderPopPathThroughput(RayShader);
            return Status;
        }

        BlendedColor = Color4Over(BlendedColor, HitColor);

        Status = RayTracerOwnerGetNextHit(RayShader->RayTracerOwner,
                                          &ShapeHit,
                                          &ModelViewer,
                                          &ModelHitPoint,
                                          &WorldHitPoint,
                                          &ModelToWorld);
    }

    *Color = Color3InitializeFromColor4(BlendedColor);

    if (ContinueProbability < (FLOAT) 1.0)
    {
        *Color = Color3DivideByScalar(*Color, ContinueProbability);
    }

    *Color = Color3ScaleByColor(*Color, Transmittance);

    RayShaderPopPathThroughput(RayShader);
    return ISTATUS_SUCCESS;
}

IRISSHADINGMODELAPI
VOID
RayShaderFree(
    _In_opt_ _Post_invalid_ PRAYSHADER RayShader
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

    RayTracerOwnerFree(RayShader->RayTracerOwner);
    RandomDereference(RayShader->Rng);
    SceneDereference(RayShader->Scene);
    free(RayShader);
}