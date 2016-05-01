/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxtoolkitplusplus_pinhole.h

Abstract:

    This file contains the definition for the Pinhole type.

--*/

#include <irisphysxtoolkitplusplus.h>

#ifndef _PINHOLE_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_
#define _PINHOLE_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_

namespace IrisPhysxToolkit {
namespace PinholeCamera {

//
// Types
//

typedef std::function<IrisAdvanced::Color3(IrisSpectrum::SpectrumReference)> ToneMappingRoutine;

typedef 
std::function<void(std::vector<IrisAdvanced::Random> &,
                   std::vector<IrisPhysx::RayTracer::ProcessHitRoutineType> &,
                   std::vector<ToneMappingRoutine> &,
                   SIZE_T NumberOfThreads)> CreateStateRoutine;

//
// Functions
//

IRISPHYSXTOOLKITPLUSPLUSAPI
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
    _In_ const IrisPhysx::Integrator::TestGeometryRoutine TestGeometryFunction,
    _In_reads_(NumberOfLights) const std::vector<IrisPhysx::Light> Lights,
    _In_ CreateStateRoutine CreateStateFunction,
    _Inout_ IrisAdvancedToolkit::Framebuffer & Framebuffer
    );

} // namespace PinholeCamera
} // namespace IrisPhysxToolkit

#endif // _PINHOLE_IRIS_PHYSX_TOOLKIT_PLUS_PLUS_