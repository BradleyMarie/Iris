/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_rayshader.h

Abstract:

    This file contains the definitions for the RAYSHADER type.

--*/

#ifndef _RAYSHADER_IRIS_SHADING_MODEL_
#define _RAYSHADER_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Macros
//

#define DISABLE_RUSSAIAN_ROULETTE_TERMINATION 0

//
// Forward declarations
//

typedef struct _SHADER SHADER, *PSHADER;
typedef CONST SHADER *PCSHADER;

//
// Types
//

typedef struct _RAYSHADER RAYSHADER, *PRAYSHADER;
typedef CONST RAYSHADER *PCRAYSHADER;

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PSHADE_RAY_ROUTINE)(
    _In_opt_ PCVOID Context,
    _Inout_opt_ PRAYSHADER NextRayShader,
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
    );

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISSHADINGMODELAPI
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
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
RayShaderTraceRayMontecarlo(
    _Inout_ PRAYSHADER RayShader,
    _In_ PCRAY WorldRay,
    _In_ PCCOLOR3 Transmittance,
    _Out_ PCOLOR3 Color
    );

IRISSHADINGMODELAPI
VOID
RayShaderFree(
    _Pre_maybenull_ _Post_invalid_ PRAYSHADER RayShader
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayShaderTraceRay(
    _Inout_ PRAYSHADER RayShader,
    _In_ PCRAY WorldRay,
    _Out_ PCOLOR3 Color
    )
{
    COLOR3 AmountReflected;
    ISTATUS Status;

    Color3InitializeWhite(&AmountReflected);

    Status = RayShaderTraceRayMontecarlo(RayShader,
                                         WorldRay,
                                         &AmountReflected,
                                         Color);

    return Status;
}

#endif // _RAYSHADER_IRIS_SHADING_MODEL_