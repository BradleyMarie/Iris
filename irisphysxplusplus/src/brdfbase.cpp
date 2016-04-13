/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    brdfbase.cpp

Abstract:

    This file contains the definitions for the 
    IrisPysx++ BRDFBase type.

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
BRDFSampleAdapter(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *ReflectorPtr,
    _Out_ PVECTOR3 Outgoing,
    _Out_ PFLOAT Pdf
    )
{
    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(Rng != NULL);
    ASSERT(Compositor != NULL);
    ASSERT(ReflectorPtr != NULL);
    ASSERT(Outgoing != NULL);
    ASSERT(Pdf != NULL);

    const BRDFBase **BRDFBasePtr = (const BRDFBase**) Context;

    auto Result = (*BRDFBasePtr)->Sample(Iris::Vector(Incoming),
                                         IrisAdvanced::RandomReference(Rng),
                                         IrisSpectrum::ReflectorCompositorReference(Compositor));

    *ReflectorPtr = std::get<0>(Result).AsPCREFLECTOR();
    *Outgoing = std::get<1>(Result).AsVECTOR3();
    *Pdf = std::get<2>(Result);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
BRDFSampleWithLambertianFalloffAdapter(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *ReflectorPtr,
    _Out_ PVECTOR3 Outgoing,
    _Out_ PFLOAT Pdf
    )
{
    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(Rng != NULL);
    ASSERT(Compositor != NULL);
    ASSERT(ReflectorPtr != NULL);
    ASSERT(Outgoing != NULL);
    ASSERT(Pdf != NULL);

    const BRDFBase **BRDFBasePtr = (const BRDFBase**) Context;

    auto Result = (*BRDFBasePtr)->SampleWithLambertianFalloff(Iris::Vector(Incoming),
                                                              IrisAdvanced::RandomReference(Rng),
                                                              IrisSpectrum::ReflectorCompositorReference(Compositor));

    *ReflectorPtr = std::get<0>(Result).AsPCREFLECTOR();
    *Outgoing = std::get<1>(Result).AsVECTOR3();
    *Pdf = std::get<2>(Result);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
BRFFComputeReflectanceAdapter(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *ReflectorPtr
    )
{
    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(VectorValidate(Outgoing) != FALSE);
    ASSERT(Compositor != NULL);
    ASSERT(ReflectorPtr != NULL);

    const BRDFBase **BRDFBasePtr = (const BRDFBase**) Context;

    auto Result = (*BRDFBasePtr)->ComputeReflectance(Iris::Vector(Incoming),
                                                     Iris::Vector(Outgoing),
                                                     IrisSpectrum::ReflectorCompositorReference(Compositor));

    *ReflectorPtr = Result.AsPCREFLECTOR();
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
BRFFComputeReflectanceWithLambertianFalloffAdapter(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *ReflectorPtr
    )
{
    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(VectorValidate(Outgoing) != FALSE);
    ASSERT(Compositor != NULL);
    ASSERT(ReflectorPtr != NULL);

    const BRDFBase **BRDFBasePtr = (const BRDFBase**) Context;

    auto Result = (*BRDFBasePtr)->ComputeReflectanceWithLambertianFalloff(Iris::Vector(Incoming),
                                                                          Iris::Vector(Outgoing),
                                                                          IrisSpectrum::ReflectorCompositorReference(Compositor));

    *ReflectorPtr = Result.AsPCREFLECTOR();
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
BRDFComputeReflectanceWithPdfAdapter(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *ReflectorPtr,
    _Out_ PFLOAT Pdf
    )
{
    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(VectorValidate(Outgoing) != FALSE);
    ASSERT(Compositor != NULL);
    ASSERT(ReflectorPtr != NULL);
    ASSERT(Pdf != NULL);

    const BRDFBase **BRDFBasePtr = (const BRDFBase**) Context;

    auto Result = (*BRDFBasePtr)->ComputeReflectanceWithPdf(Iris::Vector(Incoming),
                                                            Iris::Vector(Outgoing),
                                                            IrisSpectrum::ReflectorCompositorReference(Compositor));

    *ReflectorPtr = std::get<0>(Result).AsPCREFLECTOR();
    *Pdf = std::get<1>(Result);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
BRDFComputeReflectanceWithPdfWithLambertianFalloffAdapter(
    _In_ PCVOID Context,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *ReflectorPtr,
    _Out_ PFLOAT Pdf
    )
{
    ASSERT(Context != NULL);
    ASSERT(VectorValidate(Incoming) != FALSE);
    ASSERT(VectorValidate(Outgoing) != FALSE);
    ASSERT(Compositor != NULL);
    ASSERT(ReflectorPtr != NULL);
    ASSERT(Pdf != NULL);

    const BRDFBase **BRDFBasePtr = (const BRDFBase**) Context;

    auto Result = (*BRDFBasePtr)->ComputeReflectanceWithPdfWithLambertianFalloff(Iris::Vector(Incoming),
                                                                                 Iris::Vector(Outgoing),
                                                                                 IrisSpectrum::ReflectorCompositorReference(Compositor));

    *ReflectorPtr = std::get<0>(Result).AsPCREFLECTOR();
    *Pdf = std::get<1>(Result);

    return ISTATUS_SUCCESS;
}

static
VOID
BRDFFreeAdapter(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != nullptr);

    BRDFBase **BRDFBasePtr = (BRDFBase**) Context;
    delete *BRDFBasePtr;
}

const static PBR_BRDF_VTABLE InteropVTable {
    BRDFSampleAdapter,
    BRDFSampleWithLambertianFalloffAdapter,
    BRFFComputeReflectanceAdapter,
    BRFFComputeReflectanceWithLambertianFalloffAdapter,
    BRDFComputeReflectanceWithPdfAdapter,
    BRDFComputeReflectanceWithPdfWithLambertianFalloffAdapter,
    BRDFFreeAdapter
};

//
// Functions
//

BRDF
BRDFBase::Create(
    _In_ std::unique_ptr<BRDFBase> BRDFBasePtr
    )
{
    if (!BRDFBasePtr)
    {
        throw std::invalid_argument("BRDFBasePtr");
    }
    
    BRDFBase *UnmanagedBRDFBasePtr = BRDFBasePtr.release();
    PPBR_BRDF BRDFPtr;

    ISTATUS Success = PbrBrdfAllocate(&InteropVTable,
                                      &UnmanagedBRDFBasePtr,
                                      sizeof(BRDFBase*),
                                      alignof(BRDFBase*),
                                      &BRDFPtr);

    if (Success != ISTATUS_SUCCESS)
    {
        throw std::bad_alloc();
    }
    
    return BRDF(BRDFPtr, false);
}

} // namespace IrisPhysx