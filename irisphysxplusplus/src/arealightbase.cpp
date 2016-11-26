/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    arealightbase.cpp

Abstract:

    This file contains the definitions for the 
    IrisPysx++ AreaLightBase type.

--*/

#include <irisphysxplusplusp.h>
namespace IrisPhysx {

static
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
AreaLightComputeEmissiveAdapter(
    _In_ PCVOID Context,
    _In_ POINT3 OnLight,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum
    )
{
    assert(Context != nullptr);
    assert(PointValidate(OnLight) != FALSE);
    assert(Compositor != nullptr);
    assert(Spectrum != nullptr);

    const AreaLightBase **AreaLightBasePtr = (const AreaLightBase**) Context;

    IrisSpectrum::SpectrumReference Result = (*AreaLightBasePtr)->ComputeEmissive(Iris::Point(OnLight),
                                                                                  IrisSpectrum::SpectrumCompositorReference(Compositor));

    *Spectrum = Result.AsPCSPECTRUM();
    return ISTATUS_SUCCESS;
}

static
VOID
AreaLightFreeAdapter(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != nullptr);

    AreaLightBase **AreaLightBasePtr = (AreaLightBase**) Context;
    delete *AreaLightBasePtr;
}

static const PHYSX_AREA_LIGHT_VTABLE InteropVTable = {
    AreaLightComputeEmissiveAdapter,
    AreaLightFreeAdapter
};

//
// Functions
//

SIZE_T
AreaLightBase::Create(
    _In_ AreaLightBuilder & Builder,
    _In_ std::unique_ptr<AreaLightBase> AreaLightBasePtr
    )
{
    if (!AreaLightBasePtr)
    {
        throw std::invalid_argument("AreaLightBasePtr");
    }
    
    AreaLightBase *UnmanagedAreaLightBasePtr = AreaLightBasePtr.release();
    SIZE_T AreaLightIndex;

    ISTATUS Success = PhysxAreaLightBuilderAddLight(Builder.AsPPHYSX_AREA_LIGHT_BUILDER(),
                                                    &InteropVTable,
                                                    &UnmanagedAreaLightBasePtr,
                                                    sizeof(AreaLightBase*),
                                                    alignof(AreaLightBase*),
                                                    &AreaLightIndex);

    if (Success != ISTATUS_SUCCESS)
    {
        delete UnmanagedAreaLightBasePtr;
        throw std::bad_alloc();
    }
    
    return AreaLightIndex;
}

} // namespace IrisPhysx
