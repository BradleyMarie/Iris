/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    lambertianindirect.c

Abstract:

    This file contains the definitions for the 
    LAMBERTIAN_INDIRECT_SHADER type.

--*/

#include <iristoolkitp.h>

//
// Types
//

typedef struct _LAMBERTIAN_INDIRECT_SHADER {
    COLOR3 Reflectance;
} LAMBERTIAN_INDIRECT_SHADER, *PLAMBERTIAN_INDIRECT_SHADER;

typedef CONST LAMBERTIAN_INDIRECT_SHADER *PCLAMBERTIAN_INDIRECT_SHADER;

//
// Static functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
LambertianShaderShade(
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
    PCLAMBERTIAN_INDIRECT_SHADER LambertianIndirectShader;
    VECTOR3 WorldSurfaceNormal;
    VECTOR3 ReflectedDirection;
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

    LambertianIndirectShader = (PCLAMBERTIAN_INDIRECT_SHADER) Context;

    Status = SurfaceNormalGetNormalizedWorldNormal(SurfaceNormal,
                                                   &WorldSurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    IrisToolkitCosineSampleHemisphere(&WorldSurfaceNormal,
                                      Rng,
                                      &ReflectedDirection);

    Reflected = RayCreate(*WorldHitPoint, ReflectedDirection);

    Status = RayShaderTraceRayMontecarlo(RayTracer,
                                         &Reflected,
                                         &LambertianIndirectShader->Reflectance,
                                         Indirect);

    return Status;
}

//
// Static variables
//

CONST STATIC INDIRECT_SHADER_VTABLE LambertianShaderVTable = {
    LambertianShaderShade,
    NULL
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
PINDIRECT_SHADER
LambertianIndirectShaderAllocate(
    _In_ PCOLOR3 Reflectance
    )
{
    LAMBERTIAN_INDIRECT_SHADER LambertianIndirectShader;
    PINDIRECT_SHADER IndirectShader;

    if (Reflectance == NULL)
    {
        return NULL;
    }

    LambertianIndirectShader.Reflectance = *Reflectance;

    IndirectShader = IndirectShaderAllocate(&LambertianShaderVTable,
                                            &LambertianIndirectShader,
                                            sizeof(LAMBERTIAN_INDIRECT_SHADER),
                                            sizeof(PVOID));

    return IndirectShader;
}