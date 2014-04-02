/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    perfectspecular.c

Abstract:

    This file contains the definitions for the 
    PERFECT_SPECULAR_INDIRECT_SHADER type.

--*/

#include <iristoolkitp.h>

//
// Types
//

typedef struct _PERFECT_SPECULAR_INDIRECT_SHADER {
    INDIRECT_SHADER IndirectShaderHeader;
    COLOR3 Reflectance;
} PERFECT_SPECULAR_INDIRECT_SHADER, *PPERFECT_SPECULAR_INDIRECT_SHADER;

typedef CONST PERFECT_SPECULAR_INDIRECT_SHADER *PCPERFECT_SPECULAR_INDIRECT_SHADER;

//
// Static functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PerfectSpecularShaderShade(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _In_ PCVECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_opt_ PRAYSHADER RayTracer,
    _Out_ PCOLOR3 Indirect
    )
{
    PCPERFECT_SPECULAR_INDIRECT_SHADER PerfectSpecularIndirectShader;
    VECTOR3 WorldSurfaceNormal;
    VECTOR3 ReflectedDirection;
    COLOR3 OutputColor;
    ISTATUS Status;
    RAY Reflected;

    ASSERT(Context != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);

    if (RayTracer == NULL)
    {
        Color3InitializeBlack(Indirect);
        return ISTATUS_SUCCESS;
    }

    PerfectSpecularIndirectShader = (PCPERFECT_SPECULAR_INDIRECT_SHADER) Context;

    SurfaceNormalGetNormalizedWorldNormal(SurfaceNormal, &WorldSurfaceNormal);

    VectorReflect(WorldViewer, &WorldSurfaceNormal, &ReflectedDirection);

    RayInitialize(&Reflected, WorldHitPoint, &ReflectedDirection);

    Status = RayShaderTraceRayMontecarlo(RayTracer,
                                         &Reflected,
                                         &PerfectSpecularIndirectShader->Reflectance,
                                         &OutputColor);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Color3ScaleByColor(&OutputColor,
                       &PerfectSpecularIndirectShader->Reflectance,
                       &OutputColor);

    return ISTATUS_SUCCESS;
}

//
// Static variables
//

CONST STATIC INDIRECT_SHADER_VTABLE PerfectSpecularShaderVTable = {
    PerfectSpecularShaderShade,
    free
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
PINDIRECT_SHADER
PerfectSpecularIndirectShaderAllocate(
    _In_ PCOLOR3 Reflectance
    )
{
    PPERFECT_SPECULAR_INDIRECT_SHADER PerfectSpecularIndirectShader;

    if (Reflectance == NULL)
    {
        return NULL;
    }

    PerfectSpecularIndirectShader = (PPERFECT_SPECULAR_INDIRECT_SHADER) malloc(sizeof(PERFECT_SPECULAR_INDIRECT_SHADER));

    if (PerfectSpecularIndirectShader == NULL)
    {
        return NULL;
    }

    PerfectSpecularIndirectShader->IndirectShaderHeader.IndirectShaderVTable = &PerfectSpecularShaderVTable;
    PerfectSpecularIndirectShader->Reflectance = *Reflectance;

    return (PINDIRECT_SHADER) PerfectSpecularIndirectShader;
}