/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    lightshader.c

Abstract:

    This file contains the function definitions for the LIGHT_SHADER type.

--*/

#include <iristoolkitp.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
LightShaderShadeLight(
    _In_ PCLIGHT_SHADER LightShader,
    _In_ PCVOID Light,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 Direct
    )
{
    ISTATUS Status;

    ASSERT(LightShader != NULL);
    ASSERT(Light != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(Direct != NULL);

    Status = LightShader->LightShaderVTable->LightRoutine(LightShader,
                                                          Light,
                                                          WorldHitPoint,
                                                          ModelHitPoint,
                                                          AdditionalData,
                                                          SurfaceNormal,
                                                          Rng,
                                                          VisibilityTester,
                                                          Direct);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
LightShaderEvaluateAllLights(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 Direct
    )
{
    PCLIGHT_SHADER LightShader;
    SIZE_T NumberOfLights;
    SIZE_T LightIndex;
    COLOR3 LightColor;
    ISTATUS Status;
    PCVOID Light;

    ASSERT(Context != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(Direct != NULL);

    LightShader = (PCLIGHT_SHADER) Context;

    NumberOfLights = LightShader->NumberOfLights;

    Color3InitializeBlack(Direct);

    for (LightIndex = 0; LightIndex < NumberOfLights; LightIndex++)
    {
        Status = LightShaderShadeLight(LightShader,
                                       LightShader->Lights[LightIndex],
                                       WorldHitPoint,
                                       ModelHitPoint,
                                       AdditionalData,
                                       SurfaceNormal,
                                       Rng,
                                       VisibilityTester,
                                       &LightColor);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        Color3Add(Direct, &LightColor, Direct);
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
LightShaderEvaluateOneLight(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 Direct
    )
{
    PCLIGHT_SHADER LightShader;
    SIZE_T NumberOfLights;
    SIZE_T LightIndex;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(Direct != NULL);

    LightShader = (PCLIGHT_SHADER) Context;

    NumberOfLights = LightShader->NumberOfLights;

    LightIndex = RandomGenerateIndex(Rng, 0, NumberOfLights);

    Status = LightShaderShadeLight(LightShader,
                                   LightShader->Lights[LightIndex],
                                   WorldHitPoint,
                                   ModelHitPoint,
                                   AdditionalData,
                                   SurfaceNormal,
                                   Rng,
                                   VisibilityTester,
                                   Direct);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Color3ScaleByScalar(Direct, (FLOAT) NumberOfLights, Direct);

    return ISTATUS_SUCCESS;
}