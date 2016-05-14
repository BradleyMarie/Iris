/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    raytracer.cpp

Abstract:

    This file contains the definitions for the 
    IrisPysx++ RayTracer type.

--*/

#include <irisphysxplusplusp.h>
namespace IrisPhysx {

//
// Adapter Function
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
ProcessHitAdapter(
    _Inout_opt_ PVOID Context, 
    _In_ PCPBR_GEOMETRY GeometryPtr,
    _In_ UINT32 FaceHit,
    _In_ PCMATRIX ModelToWorld,
    _In_opt_ PCVOID AdditionalData,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHitPoint,
    _In_ POINT3 WorldHitPoint,
    _In_ RAY WorldRay,
    _In_reads_(NumberOfLights) PCPBR_LIGHT *LightsPtr,
    _In_ SIZE_T NumberOfLights,
    _Inout_opt_ PPBR_RAYTRACER RayTracerPtr,
    _Inout_ PPBR_VISIBILITY_TESTER VisibilityTesterPtr,
    _Inout_ PPBR_BRDF_ALLOCATOR BrdfAllocatorPtr,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE SpectrumCompositorReferencePtr,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE ReflectorCompositorReferencePtr,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Outptr_result_maybenull_ PCSPECTRUM *SpectrumPtr
    )
{
    assert(Context != nullptr);
    assert(GeometryPtr != nullptr);
    assert(VectorValidate(ModelViewer) != FALSE);
    assert(PointValidate(ModelHitPoint) != FALSE);
    assert(PointValidate(WorldHitPoint) != FALSE);
    assert(RayValidate(WorldRay) != FALSE);
    assert(LightsPtr != nullptr || NumberOfLights == 0);
    assert(VisibilityTesterPtr != nullptr);
    assert(BrdfAllocatorPtr != nullptr);
    assert(SpectrumCompositorReferencePtr != nullptr);
    assert(ReflectorCompositorReferencePtr != nullptr);
    assert(Rng != nullptr);
    assert(SpectrumPtr != nullptr);

    ProcessHitRoutine * HitRoutine = static_cast<ProcessHitRoutine *>(Context);

    if (RayTracerPtr == nullptr)
    {
        IrisSpectrum::SpectrumReference Result = (*HitRoutine)(GeometryReference(GeometryPtr),
                                                               FaceHit,
                                                               Iris::MatrixReference(ModelToWorld),
                                                               AdditionalData,
                                                               Iris::Vector(ModelViewer),
                                                               Iris::Point(ModelHitPoint),
                                                               Iris::Point(WorldHitPoint),
                                                               Iris::Ray(WorldRay),
                                                               LightsPtr,
                                                               NumberOfLights,
                                                               nullptr,
                                                               VisibilityTester(VisibilityTesterPtr),
                                                               BRDFAllocator(BrdfAllocatorPtr),
                                                               IrisSpectrum::SpectrumCompositorReference(SpectrumCompositorReferencePtr),
                                                               IrisSpectrum::ReflectorCompositorReference(ReflectorCompositorReferencePtr),
                                                               IrisAdvanced::RandomReference(Rng));

        *SpectrumPtr = Result.AsPCSPECTRUM();
    }
    else
    {
        RayTracer Rt(RayTracerPtr);

        IrisSpectrum::SpectrumReference Result = (*HitRoutine)(GeometryReference(GeometryPtr),
                                                               FaceHit,
                                                               Iris::MatrixReference(ModelToWorld),
                                                               AdditionalData,
                                                               Iris::Vector(ModelViewer),
                                                               Iris::Point(ModelHitPoint),
                                                               Iris::Point(WorldHitPoint),
                                                               Iris::Ray(WorldRay),
                                                               LightsPtr,
                                                               NumberOfLights,
                                                               &Rt,
                                                               VisibilityTester(VisibilityTesterPtr),
                                                               BRDFAllocator(BrdfAllocatorPtr),
                                                               IrisSpectrum::SpectrumCompositorReference(SpectrumCompositorReferencePtr),
                                                               IrisSpectrum::ReflectorCompositorReference(ReflectorCompositorReferencePtr),
                                                               IrisAdvanced::RandomReference(Rng));

        *SpectrumPtr = Result.AsPCSPECTRUM();
    }

    return ISTATUS_SUCCESS;
}

//
// Functions
//

IrisSpectrum::SpectrumReference
RayTracer::TraceClosestHit(
    _In_ const Iris::Ray & WorldRay,
    _In_ ProcessHitRoutine ProcessHitFunction
    )
{
    PCSPECTRUM Result;

    ISTATUS Status = PBRRayTracerTraceSceneProcessClosestHit(Data,
                                                             WorldRay.AsRAY(),
                                                             ProcessHitAdapter,
                                                             &ProcessHitFunction,
                                                             &Result);

    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(Iris::ISTATUSToCString(Status));
    }

    return IrisSpectrum::SpectrumReference(Result);
}

IrisSpectrum::SpectrumReference
RayTracer::TraceAllHitsInOrder(
    _In_ const Iris::Ray & WorldRay,
    _In_ ProcessHitRoutine ProcessHitFunction
    )
{
    PCSPECTRUM Result;

    ISTATUS Status = PBRRayTracerTraceSceneProcessAllHitsInOrder(Data,
                                                                 WorldRay.AsRAY(),
                                                                 ProcessHitAdapter,
                                                                 &ProcessHitFunction,
                                                                 &Result);

    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(Iris::ISTATUSToCString(Status));
    }

    return IrisSpectrum::SpectrumReference(Result);
}

} // namespace IrisPhysx