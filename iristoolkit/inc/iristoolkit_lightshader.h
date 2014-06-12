/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_lightshader.h

Abstract:

    This file contains the definitions for the LIGHT_SHADER type.

--*/

#ifndef _LIGHT_SHADER_IRIS_TOOLKIT_
#define _LIGHT_SHADER_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PLIGHT_SHADING_ROUTINE)(
    _In_opt_ PCVOID Context,
    _In_ PCVOID Light,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _In_ PCVECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 Direct
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PLIGHT_SELECTION_ROUTINE)(
    _In_opt_ PCVOID Context,
    _In_reads_(NumberOfLights) PCVOID CONST *Lights,
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
    );

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISTOOLKITAPI
ISTATUS
LightShaderEvaluateAllLights(
    _In_opt_ PCVOID Context,
    _In_reads_(NumberOfLights) PCVOID CONST *Lights,
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
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISTOOLKITAPI
ISTATUS
LightShaderEvaluateOneLight(
    _In_opt_ PCVOID Context,
    _In_reads_(NumberOfLights) PCVOID CONST *Lights,
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
    );

#endif // _LIGHT_SHADER_IRIS_TOOLKIT_