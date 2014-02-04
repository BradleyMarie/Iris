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
// Types
//

typedef
ISTATUS
(*PSHADE_RAY_ROUTINE)(
    _In_ PVOID Context,
    _In_ PCRAY WorldRay,
    _Outptr_result_buffer_(HitListSize) PCGEOMETRY_HIT **HitList,
    _Out_ PSIZE_T HitListSize
    );

typedef struct _RAYSHADER_HEADER RAYSHADER_HEADER, *PRAYSHADER_HEADER;
typedef CONST RAYSHADER_HEADER *PCRAYSHADER_HEADER;

typedef struct _RAYSHADER {
    PRAYSHADER_HEADER Header;
} RAYSHADER, *PRAYSHADER;

typedef CONST RAYSHADER *PCRAYSHADER;

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
    _In_ PFREE_ROUTINE RaytracerFreeRoutine
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
RayShaderTraceRayMontecarlo(
    _Inout_ PRAYSHADER RayShader,
    _In_ PCRAY WorldRay,
    _In_ FLOAT PreferredContinueProbability,
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
    RayShaderTraceRayMontecarlo(RayShader, WorldRay, (FLOAT) 1.0, Color);
}

#endif // _RAYSHADER_IRIS_SHADING_MODEL_