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
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ VECTOR3 ModelViewer,
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
    ISTATUS Status;
    RAY Reflected;

    ASSERT(Context != NULL);

    if (RayTracer == NULL)
    {
        *Indirect = Color3InitializeBlack();
        return ISTATUS_SUCCESS;
    }

    PerfectSpecularIndirectShader = (PCPERFECT_SPECULAR_INDIRECT_SHADER) Context;

    Status = SurfaceNormalGetNormalizedWorldNormal(SurfaceNormal,
                                                   &WorldSurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    ReflectedDirection = VectorReflect(WorldViewer, WorldSurfaceNormal);

    Reflected = RayCreate(WorldHitPoint, ReflectedDirection);

    Status = RayShaderTraceRayMontecarlo(RayTracer,
                                         Reflected,
                                         PerfectSpecularIndirectShader->Reflectance,
                                         Indirect);

    return Status;
}

//
// Static variables
//

CONST STATIC INDIRECT_SHADER_VTABLE PerfectSpecularShaderVTable = {
    PerfectSpecularShaderShade,
    NULL
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
PINDIRECT_SHADER
PerfectSpecularIndirectShaderAllocate(
    _In_ COLOR3 Reflectance
    )
{
    PERFECT_SPECULAR_INDIRECT_SHADER PerfectSpecularIndirectShader;
    PINDIRECT_SHADER IndirectShader;

    PerfectSpecularIndirectShader.Reflectance = Reflectance;

    IndirectShader = IndirectShaderAllocate(&PerfectSpecularShaderVTable,
                                            &PerfectSpecularIndirectShader,
                                            sizeof(PERFECT_SPECULAR_INDIRECT_SHADER),
                                            sizeof(PVOID));

    return IndirectShader;
}