/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_raytracer.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ RayTracer type.

--*/

#include <irisphysxplusplus.h>

#ifndef _RAYTRACER_IRIS_PHYSX_PLUS_PLUS_
#define _RAYTRACER_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Forward Declarations
//

class RayTracer;

//
// Types
//

typedef std::function<IrisSpectrum::SpectrumReference(GeometryReference,
                                                      UINT32,
                                                      Iris::MatrixReference,
                                                      PCVOID,
                                                      const Iris::Vector &,
                                                      const Iris::Point &,
                                                      const Iris::Point &,
                                                      const Iris::Ray &,
                                                      std::optional<LightListReference>,
                                                      std::optional<RayTracer>,
                                                      VisibilityTester,
                                                      BRDFAllocator,
                                                      IrisSpectrum::SpectrumCompositorReference,
                                                      IrisSpectrum::ReflectorCompositorReference,
                                                      IrisAdvanced::RandomReference)> ProcessHitRoutine;

class RayTracer final {
public:
    RayTracer(
        _In_ PPBR_RAYTRACER RayTracerPtr
        )
    : Data(RayTracerPtr)
    { 
        if (RayTracerPtr == nullptr)
        {
            throw std::invalid_argument("RayTracerPtr");
        }
    }
    
    _Ret_
    PPBR_RAYTRACER
    AsPPBR_RAYTRACER(
        void
        )
    {
        return Data;
    }

    IRISPHYSXPLUSPLUSAPI
    IrisSpectrum::SpectrumReference
    TraceClosestHit(
        _In_ const Iris::Ray & WorldRay,
        _In_ ProcessHitRoutine ProcessHitFunction
        );

    IRISPHYSXPLUSPLUSAPI
    IrisSpectrum::SpectrumReference
    TraceAllHitsInOrder(
        _In_ const Iris::Ray & WorldRay,
        _In_ ProcessHitRoutine ProcessHitFunction
        );

private:
    PPBR_RAYTRACER Data;
};

} // namespace IrisPhysx

#endif // _HIT_TESTER_IRIS_PHYSX_PLUS_PLUS_