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
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ VECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM_REFERENCE Rng,
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

    if (RayTracer == NULL)
    {
        *Indirect = Color3InitializeBlack();
        return ISTATUS_SUCCESS;
    }

    LambertianIndirectShader = (PCLAMBERTIAN_INDIRECT_SHADER) Context;

    Status = SurfaceNormalGetNormalizedWorldNormal(SurfaceNormal,
                                                   &WorldSurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    IrisToolkitCosineSampleHemisphere(WorldSurfaceNormal,
                                      Rng,
                                      &ReflectedDirection);

    Reflected = RayCreate(WorldHitPoint, ReflectedDirection);

    Status = RayShaderTraceRayMontecarlo(RayTracer,
                                         Reflected,
                                         LambertianIndirectShader->Reflectance,
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
    _In_ COLOR3 Reflectance
    )
{
    LAMBERTIAN_INDIRECT_SHADER LambertianIndirectShader;
    PINDIRECT_SHADER IndirectShader;

    LambertianIndirectShader.Reflectance = Reflectance;

    IndirectShader = IndirectShaderAllocate(&LambertianShaderVTable,
                                            &LambertianIndirectShader,
                                            sizeof(LAMBERTIAN_INDIRECT_SHADER),
                                            _Alignof(LAMBERTIAN_INDIRECT_SHADER));

    return IndirectShader;
}