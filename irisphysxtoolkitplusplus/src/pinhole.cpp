/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    pinhole.cpp

Abstract:

    This file contains the definitions for the 
    IrisPysxToolkit++ Pinhole type.

--*/

#include <irisphysxtoolkitplusplusp.h>

namespace IrisPhysxToolkit {
namespace PinholeCamera {

//
// Types
//

struct CreateStateContext {
    CreateStateContext(
        _In_ IrisPhysxToolkit::CreateStateRoutine Func
        )
    : CreateStateFunction(Func)
    { }

    std::vector<IrisPhysx::ProcessHitRoutine> ProcessHitRoutines;
    std::vector<IrisPhysxToolkit::ToneMappingRoutine> ToneMappingRoutines;
    IrisPhysxToolkit::CreateStateRoutine CreateStateFunction;
};

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
    _In_ PCPHYSX_LIGHT_LIST LightsPtr,
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
    assert(VisibilityTesterPtr != nullptr);
    assert(BrdfAllocatorPtr != nullptr);
    assert(SpectrumCompositorReferencePtr != nullptr);
    assert(ReflectorCompositorReferencePtr != nullptr);
    assert(Rng != nullptr);
    assert(SpectrumPtr != nullptr);

    auto * ProcessHitContext = static_cast<IrisPhysx::ProcessHitRoutine *>(Context);

    std::optional<IrisPhysx::RayTracer> Tracer;

    if (RayTracerPtr != nullptr)
    {
        Tracer = IrisPhysx::RayTracer(RayTracerPtr);
    }

    std::optional<IrisPhysx::LightListReference> Lights;

    if (LightsPtr != nullptr)
    {
        Lights = IrisPhysx::LightListReference(LightsPtr);
    }

    IrisSpectrum::SpectrumReference Result = (*ProcessHitContext)(IrisPhysx::GeometryReference(GeometryPtr),
                                                                  FaceHit,
                                                                  Iris::MatrixReference(ModelToWorld),
                                                                  AdditionalData,
                                                                  Iris::Vector(ModelViewer),
                                                                  Iris::Point(ModelHitPoint),
                                                                  Iris::Point(WorldHitPoint),
                                                                  Iris::Ray(WorldRay),
                                                                  Lights,
                                                                  Tracer,
                                                                  IrisPhysx::VisibilityTester(VisibilityTesterPtr),
                                                                  IrisPhysx::BRDFAllocator(BrdfAllocatorPtr),
                                                                  IrisSpectrum::SpectrumCompositorReference(SpectrumCompositorReferencePtr),
                                                                  IrisSpectrum::ReflectorCompositorReference(ReflectorCompositorReferencePtr),
                                                                  IrisAdvanced::RandomReference(Rng));

    *SpectrumPtr = Result.AsPCSPECTRUM();

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
TestGeometryAdapter(
    _In_opt_ PCVOID Context, 
    _Inout_ PPBR_HIT_TESTER PBRHitTester,
    _In_ RAY WorldRay
    )
{
    assert(Context != nullptr);
    assert(PBRHitTester != nullptr);
    assert(RayValidate(WorldRay) != FALSE);

    auto TestGeometryFunction = static_cast<const IrisPhysx::TestGeometryRoutine *>(Context);

    (*TestGeometryFunction)(Iris::Ray(WorldRay), IrisPhysx::HitTester(PBRHitTester));

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
ToneMappingAdapter(
    _In_opt_ PCVOID Context,
    _In_opt_ PCSPECTRUM Spectrum,
    _Out_ PCOLOR3 Result
    )
{
    assert(Context != nullptr);
    assert(Result != nullptr);

    IrisPhysxToolkit::ToneMappingRoutine * ToneMappingFunction = (IrisPhysxToolkit::ToneMappingRoutine *) Context;

    IrisAdvanced::Color3 Color = (*ToneMappingFunction)(IrisSpectrum::SpectrumReference(Spectrum));

    *Result = Color.AsCOLOR3();

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
CreateStateAdapter(
    _In_opt_ PVOID Context,
    _Out_writes_(NumberOfThreads) PRANDOM_REFERENCE *Rngs,
    _Out_writes_(NumberOfThreads) PPBR_RAYTRACER_PROCESS_HIT_ROUTINE *ProcessHitRoutine,
    _Out_writes_(NumberOfThreads) PVOID *ProcessHitContexts,
    _Out_writes_(NumberOfThreads) PPBR_TONE_MAPPING_ROUTINE *ToneMappingRoutines,
    _Out_writes_(NumberOfThreads) PVOID *ToneMappingContexts,
    _In_ SIZE_T NumberOfThreads
    )
{
    assert(Context != nullptr);
    assert(Rngs != nullptr);
    assert(ProcessHitContexts != nullptr);
    assert(ToneMappingRoutines != nullptr);
    assert(ToneMappingContexts != nullptr);
    assert(NumberOfThreads != 0);

    CreateStateContext * StateContext = (CreateStateContext *) Context;
    std::vector<IrisAdvanced::RandomReference> RngReferences;

    StateContext->CreateStateFunction(RngReferences,
                                      StateContext->ProcessHitRoutines,
                                      StateContext->ToneMappingRoutines,
                                      NumberOfThreads);

    for (SIZE_T Index = 0; Index < NumberOfThreads; Index++)
    {
        Rngs[Index] = RngReferences.at(Index).AsPRANDOM_REFERENCE();
        ProcessHitRoutine[Index] = ProcessHitAdapter;
        ProcessHitContexts[Index] = &StateContext->ProcessHitRoutines.at(Index);
        ToneMappingRoutines[Index] = ToneMappingAdapter;
        ToneMappingContexts[Index] = &StateContext->ToneMappingRoutines.at(Index);
    }

    return ISTATUS_SUCCESS;
}

//
// Functions
//

void
Render(
    _In_ const Iris::Point & PinholeLocation,
    _In_ FLOAT ImagePlaneDistance,
    _In_ FLOAT ImagePlaneHeight,
    _In_ FLOAT ImagePlaneWidth,
    _In_ const Iris::Vector & CameraDirection,
    _In_ const Iris::Vector & Up,
    _In_ SIZE_T AdditionalXSamplesPerPixel,
    _In_ SIZE_T AdditionalYSamplesPerPixel,
    _In_ FLOAT Epsilon,
    _In_ SIZE_T MaxDepth,
    _In_ bool Jitter,
    _In_ bool Parallelize,
    _In_ const IrisPhysx::TestGeometryRoutine TestGeometryFunction,
    _In_ CreateStateRoutine CreateStateFunction,
    _Inout_ IrisAdvancedToolkit::Framebuffer & Framebuffer
    )
{
    CreateStateContext CreateStateCtxt(CreateStateFunction);

    ISTATUS Status = PinholeRender(PinholeLocation.AsPOINT3(),
                                   ImagePlaneDistance,
                                   ImagePlaneHeight,
                                   ImagePlaneWidth,
                                   CameraDirection.AsVECTOR3(),
                                   Up.AsVECTOR3(),
                                   AdditionalXSamplesPerPixel,
                                   AdditionalYSamplesPerPixel,
                                   Epsilon,
                                   MaxDepth,
                                   (Jitter) ? TRUE : FALSE,
                                   (Parallelize) ? TRUE : FALSE,
                                   TestGeometryAdapter,
                                   &TestGeometryFunction,
                                   nullptr,
                                   CreateStateAdapter,
                                   nullptr,
                                   &CreateStateCtxt,
                                   Framebuffer.AsPFRAMEBUFFER());
}

void
Render(
    _In_ const Iris::Point & PinholeLocation,
    _In_ FLOAT ImagePlaneDistance,
    _In_ FLOAT ImagePlaneHeight,
    _In_ FLOAT ImagePlaneWidth,
    _In_ const Iris::Vector & CameraDirection,
    _In_ const Iris::Vector & Up,
    _In_ SIZE_T AdditionalXSamplesPerPixel,
    _In_ SIZE_T AdditionalYSamplesPerPixel,
    _In_ FLOAT Epsilon,
    _In_ SIZE_T MaxDepth,
    _In_ bool Jitter,
    _In_ bool Parallelize,
    _In_ const IrisPhysx::TestGeometryRoutine TestGeometryFunction,
    _In_ IrisPhysx::LightListReference Lights,
    _In_ CreateStateRoutine CreateStateFunction,
    _Inout_ IrisAdvancedToolkit::Framebuffer & Framebuffer
    )
{
    CreateStateContext CreateStateCtxt(CreateStateFunction);

    ISTATUS Status = PinholeRender(PinholeLocation.AsPOINT3(),
                                   ImagePlaneDistance,
                                   ImagePlaneHeight,
                                   ImagePlaneWidth,
                                   CameraDirection.AsVECTOR3(),
                                   Up.AsVECTOR3(),
                                   AdditionalXSamplesPerPixel,
                                   AdditionalYSamplesPerPixel,
                                   Epsilon,
                                   MaxDepth,
                                   (Jitter) ? TRUE : FALSE,
                                   (Parallelize) ? TRUE : FALSE,
                                   TestGeometryAdapter,
                                   &TestGeometryFunction,
                                   Lights.AsPCPHYSX_LIGHT_LIST(),
                                   CreateStateAdapter,
                                   nullptr,
                                   &CreateStateCtxt,
                                   Framebuffer.AsPFRAMEBUFFER());
}

void
Render(
    _In_ const Iris::Point & PinholeLocation,
    _In_ FLOAT ImagePlaneDistance,
    _In_ FLOAT ImagePlaneHeight,
    _In_ FLOAT ImagePlaneWidth,
    _In_ const Iris::Vector & CameraDirection,
    _In_ const Iris::Vector & Up,
    _In_ SIZE_T AdditionalXSamplesPerPixel,
    _In_ SIZE_T AdditionalYSamplesPerPixel,
    _In_ FLOAT Epsilon,
    _In_ SIZE_T MaxDepth,
    _In_ bool Jitter,
    _In_ bool Parallelize,
    _In_ const IrisPhysx::TestGeometryRoutine TestGeometryFunction,
    _In_ const IrisPhysx::LightList & Lights,
    _In_ CreateStateRoutine CreateStateFunction,
    _Inout_ IrisAdvancedToolkit::Framebuffer & Framebuffer
    )
{
    CreateStateContext CreateStateCtxt(CreateStateFunction);

    ISTATUS Status = PinholeRender(PinholeLocation.AsPOINT3(),
                                   ImagePlaneDistance,
                                   ImagePlaneHeight,
                                   ImagePlaneWidth,
                                   CameraDirection.AsVECTOR3(),
                                   Up.AsVECTOR3(),
                                   AdditionalXSamplesPerPixel,
                                   AdditionalYSamplesPerPixel,
                                   Epsilon,
                                   MaxDepth,
                                   (Jitter) ? TRUE : FALSE,
                                   (Parallelize) ? TRUE : FALSE,
                                   TestGeometryAdapter,
                                   &TestGeometryFunction,
                                   Lights.AsPCPHYSX_LIGHT_LIST(),
                                   CreateStateAdapter,
                                   nullptr,
                                   &CreateStateCtxt,
                                   Framebuffer.AsPFRAMEBUFFER());
}

} // namespace PinholeCamera
} // namespace IrisPhysxToolkit