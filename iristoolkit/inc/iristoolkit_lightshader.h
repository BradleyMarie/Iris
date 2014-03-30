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
    _In_ PCVOID Context,
    _In_ PCVOID Light,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 Direct
    );

typedef struct _LIGHT_SHADER_VTABLE {
    DIRECT_SHADER_VTABLE DirectShaderVTable;
    PLIGHT_SHADING_ROUTINE LightRoutine;
} LIGHT_SHADER_VTABLE, *PLIGHT_SHADER_VTABLE;

typedef CONST LIGHT_SHADER_VTABLE *PCLIGHT_SHADER_VTABLE;

typedef struct _LIGHT_SHADER {
    PCLIGHT_SHADER_VTABLE LightShaderVTable;
    _Field_size_(NumberOfLights) PCVOID *Lights;
    SIZE_T NumberOfLights;
} LIGHT_SHADER, *PLIGHT_SHADER;

typedef CONST LIGHT_SHADER *PCLIGHT_SHADER;

//
// Direct lighting routines used to populate
// the first entry of a LIGHT_SHADER_VTABLE
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
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
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
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
    );

#endif // _LIGHT_SHADER_IRIS_TOOLKIT_