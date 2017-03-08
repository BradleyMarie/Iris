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
    _In_ PCPHYSX_GEOMETRY GeometryPtr,
    _In_ UINT32 FaceHit,
    _In_ PCMATRIX ModelToWorld,
    _In_opt_ PCVOID AdditionalData,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHitPoint,
    _In_ POINT3 WorldHitPoint,
    _In_ RAY WorldRay,
    _In_ PCPHYSX_LIGHT_LIST LightsPtr,
    _Inout_opt_ PPHYSX_RAYTRACER RayTracerPtr,
    _Inout_ PPHYSX_VISIBILITY_TESTER VisibilityTesterPtr,
    _Inout_ PPHYSX_BRDF_ALLOCATOR AllocatorPtr,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE SpectrumCompositorReferencePtr,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE ReflectorCompositorReferencePtr,
    _Inout_ PRANDOM Rng,
    _Outptr_result_maybenull_ PCSPECTRUM *SpectrumPtr
    )
{
    assert(Context != nullptr);
    assert(GeometryPtr != nullptr);
    assert(VectorValidate(ModelViewer) != FALSE);
    assert(PointValidate(ModelHitPoint) != FALSE);
    assert(PointValidate(WorldHitPoint) != FALSE);
    assert(RayValidate(WorldRay) != FALSE);
    assert(VisibilityTesterPtr != nullptr);
    assert(AllocatorPtr != nullptr);
    assert(SpectrumCompositorReferencePtr != nullptr);
    assert(ReflectorCompositorReferencePtr != nullptr);
    assert(Rng != nullptr);
    assert(SpectrumPtr != nullptr);

    auto * ProcessHitContext = static_cast<ProcessHitRoutine *>(Context);

    std::optional<RayTracer> Tracer;

    if (RayTracerPtr != nullptr)
    {
        Tracer = RayTracer(RayTracerPtr);
    }

    std::optional<LightListReference> Lights;

    if (LightsPtr != nullptr)
    {
        Lights = LightListReference(LightsPtr);
    }

    IrisSpectrum::SpectrumReference Result = (*ProcessHitContext)(GeometryReference(GeometryPtr),
                                                                  FaceHit,
                                                                  Iris::MatrixReference(ModelToWorld),
                                                                  AdditionalData,
                                                                  Iris::Vector(ModelViewer),
                                                                  Iris::Point(ModelHitPoint),
                                                                  Iris::Point(WorldHitPoint),
                                                                  Iris::Ray(WorldRay),
                                                                  Lights,
                                                                  Tracer,
                                                                  VisibilityTester(VisibilityTesterPtr),
                                                                  BRDFAllocator(AllocatorPtr),
                                                                  IrisSpectrum::SpectrumCompositorReference(SpectrumCompositorReferencePtr),
                                                                  IrisSpectrum::ReflectorCompositorReference(ReflectorCompositorReferencePtr),
                                                                  IrisAdvanced::Random(Rng));

    *SpectrumPtr = Result.AsPCSPECTRUM();

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
TestGeometryAdapter(
    _In_opt_ PCVOID Context,
    _Inout_ PPHYSX_HIT_TESTER HitTesterPtr,
    _In_ RAY Ray
    )
{
    assert(Context != nullptr);
    assert(HitTesterPtr != nullptr);
    assert(RayValidate(Ray) != FALSE);

    auto TestGeometryFunction = static_cast<const TestGeometryRoutine *>(Context);

    (*TestGeometryFunction)(Iris::Ray(Ray), HitTester(HitTesterPtr));

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
LifetimeCallbackAdapter(
    _Inout_opt_ PVOID Context,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE SpectrumCompositorPtr,
    _Inout_ PPHYSX_RAYTRACER RayTracerPtr
    )
{
    assert(Context != nullptr);
    assert(SpectrumCompositorPtr != nullptr);
    assert(RayTracerPtr != nullptr);

    auto LifetimeCallback = static_cast<RayTracerCallback *>(Context);

    (*LifetimeCallback)(RayTracer(RayTracerPtr),
                        IrisSpectrum::SpectrumCompositorReference(SpectrumCompositorPtr));

    return ISTATUS_SUCCESS;
}

//
// Functions
//

void
RayTracer::Create(
    _In_ const TestGeometryRoutine TestGeometryFunction,
    _In_ RayTracerCallback LifetimeCallback,
    _In_ IrisSpectrum::SpectrumCompositorReference SpectrumCompositorRef,
    _In_ IrisSpectrum::ReflectorCompositorReference ReflectorCompositorRef,
    _In_ LightListReference Lights,
    _Inout_ IrisAdvanced::Random Rng,
    _In_ SIZE_T MaximumDepth,
    _In_ FLOAT Epsilon
    )
{
    ISTATUS Status = PhysxRayTracerAllocate(TestGeometryAdapter,
                                            &TestGeometryFunction,
                                            LifetimeCallbackAdapter,
                                            &LifetimeCallback,
                                            SpectrumCompositorRef.AsPSPECTRUM_COMPOSITOR_REFERENCE(),
                                            ReflectorCompositorRef.AsPREFLECTOR_COMPOSITOR_REFERENCE(),
                                            Lights.AsPCPHYSX_LIGHT_LIST(),
                                            Rng.AsPRANDOM(),
                                            MaximumDepth,
                                            Epsilon);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_ALLOCATION_FAILED:
            throw std::bad_alloc();
        default:
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
    }
}

IrisSpectrum::SpectrumReference
RayTracer::TraceClosestHit(
    _In_ const Iris::Ray & WorldRay,
    _In_ ProcessHitRoutine ProcessHitFunction
    )
{
    PCSPECTRUM Result;

    ISTATUS Status = PhysxRayTracerTraceSceneProcessClosestHit(Data,
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

    ISTATUS Status = PhysxRayTracerTraceSceneProcessAllHitsInOrder(Data,
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
