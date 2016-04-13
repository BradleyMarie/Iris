/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    lightbase.cpp

Abstract:

    This file contains the definitions for the 
    IrisPysx++ LightBase type.

--*/

#include <irisphysxplusplusp.h>
namespace IrisPhysx {

//
// Adapter Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
LightSampleAdapter(
    _In_ PCVOID Context,
    _In_ POINT3 HitPoint,
    _Inout_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PVECTOR3 ToLight,
    _Out_ PFLOAT Pdf
    )
{
    ASSERT(Context != NULL);
    ASSERT(PointValidate(HitPoint) != FALSE);
    ASSERT(PBRVisibilityTester != NULL);
    ASSERT(Rng != NULL);
    ASSERT(Compositor != NULL);
    ASSERT(Spectrum != NULL);
    ASSERT(ToLight != NULL);
    ASSERT(Pdf != NULL);

    const LightBase **LightBasePtr = (const LightBase**) Context;

    auto Result = (*LightBasePtr)->Sample(Iris::Point(HitPoint),
                                          VisibilityTester(PBRVisibilityTester),
                                          IrisAdvanced::RandomReference(Rng),
                                          IrisSpectrum::SpectrumCompositorReference(Compositor));

    *Spectrum = std::get<0>(Result).AsPCSPECTRUM();
    *ToLight = std::get<1>(Result).AsVECTOR3();
    *Pdf = std::get<2>(Result);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
LightComputeEmissiveAdapter(
    _In_ PCVOID Context,
    _In_ RAY ToLight,
    _Inout_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _Out_ PCSPECTRUM *Spectrum
    )
{
    ASSERT(Context != NULL);
    ASSERT(RayValidate(ToLight) != FALSE);
    ASSERT(PBRVisibilityTester != NULL);
    ASSERT(Spectrum != NULL);

    const LightBase **LightBasePtr = (const LightBase**) Context;

    auto Result = (*LightBasePtr)->ComputeEmissive(Iris::Ray(ToLight),
                                                   VisibilityTester(PBRVisibilityTester));

    *Spectrum = Result.AsPCSPECTRUM();
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
LightComputeEmissiveWithPdfAdapter(
    _In_ PCVOID Context,
    _In_ RAY ToLight,
    _Inout_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PFLOAT Pdf
    )
{
    ASSERT(Context != NULL);
    ASSERT(RayValidate(ToLight) != FALSE);
    ASSERT(PBRVisibilityTester != NULL);
    ASSERT(Spectrum != NULL);
    ASSERT(Pdf != NULL);

    const LightBase **LightBasePtr = (const LightBase**) Context;

    auto Result = (*LightBasePtr)->ComputeEmissiveWithPdf(Iris::Ray(ToLight),
                                                          VisibilityTester(PBRVisibilityTester));

    *Spectrum = std::get<0>(Result).AsPCSPECTRUM();
    *Pdf = std::get<1>(Result);
    
    return ISTATUS_SUCCESS;
}

static
VOID
LightFreeAdapter(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != nullptr);

    LightBase **LightBasePtr = (LightBase**) Context;
    delete *LightBasePtr;
}

const static PBR_LIGHT_VTABLE InteropVTable {
    LightSampleAdapter,
    LightComputeEmissiveAdapter,
    LightComputeEmissiveWithPdfAdapter,
    LightFreeAdapter
};

//
// Functions
//

Light
LightBase::Create(
    _In_ std::unique_ptr<LightBase> LightBasePtr
    )
{
    if (!LightBasePtr)
    {
        throw std::invalid_argument("LightBasePtr");
    }
    
    LightBase *UnmanagedLightBasePtr = LightBasePtr.release();
    PPBR_LIGHT LightPtr;

    ISTATUS Success = PbrLightAllocate(&InteropVTable,
                                       &UnmanagedLightBasePtr,
                                       sizeof(LightBase*),
                                       alignof(LightBase*),
                                       &LightPtr);

    if (Success != ISTATUS_SUCCESS)
    {
        throw std::bad_alloc();
    }
    
    return Light(LightPtr, false);
}

} // namespace IrisPhysx