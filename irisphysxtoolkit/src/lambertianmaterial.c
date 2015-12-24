/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    lambertianmaterial.c

Abstract:

    This file contains the definitions for Lambertian material types.

--*/

#include <irisphysxtoolkitp.h>

//
// Types
//

typedef struct _SIMPLE_LAMBERTIAN_MATERIAL {
    PREFLECTOR Reflectance;
} SIMPLE_LAMBERTIAN_MATERIAL, *PSIMPLE_LAMBERTIAN_MATERIAL;

typedef CONST SIMPLE_LAMBERTIAN_MATERIAL *PCSIMPLE_LAMBERTIAN_MATERIAL;

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SimpleLambertianMaterialSample(
    _In_ PCVOID Context,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _In_ VECTOR3 SurfaceNormal,
    _In_opt_ PCMATRIX ModelToWorld,
    _Inout_ PBRDF_ALLOCATOR BrdfAllocator,
    _Out_ PBRDF *Brdf
    )
{
    PCSIMPLE_LAMBERTIAN_MATERIAL Material;
    ISTATUS Status;
    
    ASSERT(Context != NULL);
    ASSERT(PointValidate(ModelHitPoint) != FALSE);
    ASSERT(VectorValidate(SurfaceNormal) != FALSE);
    ASSERT(BrdfAllocator != NULL);
    ASSERT(Brdf != NULL);
    
    Material = (PCSIMPLE_LAMBERTIAN_MATERIAL) Context;
    
    Status = SpectrumLambertianBrdfAllocate(BrdfAllocator,
                                            Material->Reflectance,
                                            SurfaceNormal,
                                            Brdf);
                                            
    return Status;
}

STATIC
VOID
SimpleLambertianMaterialFree(
    _In_ PCVOID Context
    )
{
    PCSIMPLE_LAMBERTIAN_MATERIAL Material;

    ASSERT(Context != NULL);

    Material = (PCSIMPLE_LAMBERTIAN_MATERIAL) Context;

    ReflectorDereference(Material->Reflectance);
}

//
// Static Variables
//

CONST STATIC MATERIAL_VTABLE SimpleLambertianMaterialVTable = {
    SimpleLambertianMaterialSample,
    SimpleLambertianMaterialFree
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXTOOLKITAPI
ISTATUS
LambertianMaterialAllocate(
    _In_ PREFLECTOR Reflectance,
    _Out_ PMATERIAL *Material
    )
{
    SIMPLE_LAMBERTIAN_MATERIAL SimpleMaterial;
    ISTATUS Status;
    
    if (Reflectance == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (Material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    SimpleMaterial.Reflectance = Reflectance;
    
    Status = MaterialAllocate(&SimpleLambertianMaterialVTable,
                              &SimpleMaterial,
                              sizeof(SIMPLE_LAMBERTIAN_MATERIAL),
                              _Alignof(SIMPLE_LAMBERTIAN_MATERIAL),
                              Material);
                            
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    ReflectorReference(Reflectance);
    
    return ISTATUS_SUCCESS;
}