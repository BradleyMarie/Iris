/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisrgb_rayshader.h

Abstract:

    This file contains the definitions for the RAYSHADER type.

--*/

#ifndef _RAYSHADER_IRIS_RGB_
#define _RAYSHADER_IRIS_RGB_

#include <irisrgb.h>

//
// Macros
//

#define DISABLE_RUSSAIAN_ROULETTE_TERMINATION 0

//
// Forward declarations
//

typedef struct _EMISSIVE_SHADER EMISSIVE_SHADER, *PEMISSIVE_SHADER;
typedef CONST EMISSIVE_SHADER *PCEMISSIVE_SHADER;

typedef struct _DIRECT_SHADER DIRECT_SHADER, *PDIRECT_SHADER;
typedef CONST DIRECT_SHADER *PCDIRECT_SHADER;

typedef struct _INDIRECT_SHADER INDIRECT_SHADER, *PINDIRECT_SHADER;
typedef CONST INDIRECT_SHADER *PCINDIRECT_SHADER;

typedef struct _TRANSLUCENT_SHADER TRANSLUCENT_SHADER, *PTRANSLUCENT_SHADER;
typedef CONST TRANSLUCENT_SHADER *PCTRANSLUCENT_SHADER;

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
    _In_ VECTOR3 WorldViewer,
    _In_ POINT3 WorldHit,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHit,
    _In_opt_ PCMATRIX ModelToWorld,
    _In_opt_ PCVOID AdditionalData,
    _In_opt_ PCEMISSIVE_SHADER EmissiveShader,
    _In_opt_ PCDIRECT_SHADER DirectShader,
    _In_opt_ PCINDIRECT_SHADER IndirectShader,
    _In_opt_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PCOLOR3 Color
    );

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISRGBAPI
PRAYSHADER
RayShaderAllocate(
    _In_opt_ PCVOID Context,
    _In_ PSHADE_RAY_ROUTINE ShadeRayRoutine,
    _In_ PRGB_SCENE RgbScene,
    _In_ PRANDOM Rng,
    _In_ FLOAT Epsilon,
    _In_ FLOAT MinimumContinueProbability,
    _In_ FLOAT MaximumContinueProbability,
    _In_ UINT8 RussianRouletteStartDepth,
    _In_ UINT8 MaximumRecursionDepth
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISRGBAPI
ISTATUS
RayShaderTraceRayMontecarlo(
    _Inout_ PRAYSHADER RayShader,
    _In_ RAY WorldRay,
    _In_ COLOR3 Transmittance,
    _Out_ PCOLOR3 Color
    );

IRISRGBAPI
VOID
RayShaderFree(
    _In_opt_ _Post_invalid_ PRAYSHADER RayShader
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
RayShaderTraceRay(
    _Inout_ PRAYSHADER RayShader,
    _In_ RAY WorldRay,
    _Out_ PCOLOR3 Color
    )
{
    COLOR3 AmountReflected;
    ISTATUS Status;

    AmountReflected = Color3InitializeWhite();

    Status = RayShaderTraceRayMontecarlo(RayShader,
                                         WorldRay,
                                         AmountReflected,
                                         Color);

    return Status;
}

#endif // _RAYSHADER_IRIS_RGB_