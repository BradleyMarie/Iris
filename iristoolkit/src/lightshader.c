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
ISTATUS
LightShaderEvaluateAllLights(
    _In_opt_ PCVOID Context,
    _In_reads_(NumberOfLights) PCVOID *Lights,
    _In_ SIZE_T NumberOfLights,
    _In_ PLIGHT_SHADING_ROUTINE LightShadingRoutine,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _In_ PCVECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 Direct
    )
{
    SIZE_T LightIndex;
    COLOR3 LightColor;
    ISTATUS Status;

    ASSERT(Lights != NULL);
    ASSERT(NumberOfLights != 0);
    ASSERT(LightShadingRoutine != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(WorldViewer != NULL);
    ASSERT(ModelViewer != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(Direct != NULL);

    Color3InitializeBlack(Direct);

    for (LightIndex = 0; LightIndex < NumberOfLights; LightIndex++)
    {
        Status = LightShadingRoutine(Context,
                                     Lights[LightIndex],
                                     WorldHitPoint,
                                     ModelHitPoint,
                                     WorldViewer,
                                     ModelViewer,
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
    _In_opt_ PCVOID Context,
    _In_reads_(NumberOfLights) PCVOID *Lights,
    _In_ SIZE_T NumberOfLights,
    _In_ PLIGHT_SHADING_ROUTINE LightShadingRoutine,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _In_ PCVECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 Direct
    )
{
    SIZE_T LightIndex;
    ISTATUS Status;

    ASSERT(Lights != NULL);
    ASSERT(NumberOfLights != 0);
    ASSERT(LightShadingRoutine != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(WorldViewer != NULL);
    ASSERT(ModelViewer != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(Direct != NULL);

    LightIndex = RandomGenerateIndex(Rng, 0, NumberOfLights);

    Status = LightShadingRoutine(Context,
                                 Lights[LightIndex],
                                 WorldHitPoint,
                                 ModelHitPoint,
                                 WorldViewer,
                                 ModelViewer,
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