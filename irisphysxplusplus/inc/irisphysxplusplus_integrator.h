/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_integrator.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ Integrator type.

--*/

#include <irisphysxplusplus.h>

#ifndef _INTEGRATOR_IRIS_PHYSX_PLUS_PLUS_
#define _INTEGRATOR_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

typedef std::function<void(const Iris::Ray &, HitTester)> TestGeometryRoutine;
typedef std::function<void(const Iris::Ray &, RayTracer)> IntegrateRoutine;

class Integrator final {
public:
    Integrator(
        _In_ Integrator && ToMove
        )
    : Data(ToMove.Data)
    {
        ToMove.Data = nullptr;
    }
    
    static
    Integrator
    Create(
        SIZE_T MaximumRecursions
        )
    { 
        return Integrator(MaximumRecursions);
    }

    _Ret_
    PPBR_INTEGRATOR
    AsPPBR_INTEGRATOR(
        void
        )
    {
        return Data;
    }

    IRISPHYSXPLUSPLUSAPI
    void
    Integrate(
        _In_ const TestGeometryRoutine TestGeometryFunction,
        _In_ IntegrateRoutine IntegrateFunction,
        _In_ LightListReference Lights,
        _In_ FLOAT Epsilon,
        _In_ const Iris::Ray & WorldRay,
        _Inout_ IrisAdvanced::RandomReference Rng
        );

    IRISPHYSXPLUSPLUSAPI
    void
    Integrate(
        _In_ const TestGeometryRoutine TestGeometryFunction,
        _In_ IntegrateRoutine IntegrateFunction,
        _In_ const LightList & Lights,
        _In_ FLOAT Epsilon,
        _In_ const Iris::Ray & WorldRay,
        _Inout_ IrisAdvanced::RandomReference Rng
        );

    IRISPHYSXPLUSPLUSAPI
    void
    Integrate(
        _In_ const TestGeometryRoutine TestGeometryFunction,
        _In_ IntegrateRoutine IntegrateFunction,
        _In_ FLOAT Epsilon,
        _In_ const Iris::Ray & WorldRay,
        _Inout_ IrisAdvanced::RandomReference Rng
        );

    Integrator(
        _In_ Integrator & ToCopy
        ) = delete;
        
    Integrator &
    operator=(
        _In_ const Integrator & ToCopy
        ) = delete;
    
    ~Integrator(
        void
        )
     {
         PBRIntegratorFree(Data);
     }

private:
    PPBR_INTEGRATOR Data;

    Integrator(
        _In_ SIZE_T MaximumRecursions
        )
    {
        ISTATUS Status = PBRIntegratorAllocate(MaximumRecursions, &Data);

        if (Status != ISTATUS_SUCCESS)
        {
            assert(Status == ISTATUS_ALLOCATION_FAILED);
            throw std::bad_alloc();
        }
    }
};

} // namespace IrisPhysx

#endif // _INTEGRATOR_IRIS_PHYSX_PLUS_PLUS_