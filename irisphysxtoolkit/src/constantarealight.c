/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    constantarealight.c

Abstract:

    This file contains the definitions for constant area light.

--*/

#include <irisphysxtoolkitp.h>

//
// Types
//

typedef struct _PHYSX_CONSTANT_AREA_LIGHT {
    PSPECTRUM Spectrum;
} PHYSX_CONSTANT_AREA_LIGHT, *PPHYSX_CONSTANT_AREA_LIGHT;

typedef CONST PHYSX_CONSTANT_AREA_LIGHT *PCPHYSX_CONSTANT_AREA_LIGHT;

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxConstantAreaLightComputeEmisive(
    _In_ PCVOID Context,
    _In_ POINT3 OnLight,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum
    )
{
    PCPHYSX_CONSTANT_AREA_LIGHT ConstantAreaLight;

    ASSERT(Context != NULL);
    ASSERT(PointValidate(OnLight) != FALSE);
    ASSERT(Compositor != NULL);
    ASSERT(Spectrum != NULL);

    ConstantAreaLight = (PCPHYSX_CONSTANT_AREA_LIGHT) Context;

    *Spectrum = ConstantAreaLight->Spectrum;

    return ISTATUS_SUCCESS;
}

STATIC
VOID
PhysxConstantAreaLightFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PCPHYSX_CONSTANT_AREA_LIGHT ConstantAreaLight;

    ASSERT(Context != NULL);

    ConstantAreaLight = (PCPHYSX_CONSTANT_AREA_LIGHT) Context;

    SpectrumRelease(ConstantAreaLight->Spectrum);
}

//
// Static Variables
//

STATIC CONST PHYSX_AREA_LIGHT_VTABLE ConstantAreaLightVTable = {
    PhysxConstantAreaLightComputeEmisive,
    PhysxConstantAreaLightFree
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxConstantAreaLightAllocate(
    _In_ PPHYSX_AREA_LIGHT_BUILDER Builder,
    _In_ PSPECTRUM Spectrum,
    _Out_ PSIZE_T AreaLightIndex
    )
{
    PHYSX_CONSTANT_AREA_LIGHT ConstantAreaLight;
    ISTATUS Status;

    if (Builder == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (AreaLightIndex == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    ConstantAreaLight.Spectrum = Spectrum;

    Status = PhysxAreaLightBuilderAddLight(Builder,
                                           &ConstantAreaLightVTable,
                                           &ConstantAreaLight,
                                           sizeof(PHYSX_CONSTANT_AREA_LIGHT),
                                           _Alignof(PHYSX_CONSTANT_AREA_LIGHT),
                                           AreaLightIndex);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    SpectrumRetain(Spectrum);
    return ISTATUS_SUCCESS;
}
