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
// Adapter Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
TestGeometryRoutineAdapter(
    _In_opt_ PCVOID Context, 
    _Inout_ PPHYSX_HIT_TESTER HitTesterPtr,
    _In_ RAY WorldRay
    )
{
    assert(Context != nullptr);
    assert(HitTesterPtr != nullptr);
    assert(RayValidate(WorldRay) != FALSE);

    auto TestGeometryFunction = static_cast<const TestGeometryRoutine *>(Context);

    (*TestGeometryFunction)(Iris::Ray(WorldRay), HitTester(HitTesterPtr));

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
IntegrateRoutineAdapter(
    _Inout_opt_ PVOID Context,
    _Inout_ PPHYSX_RAYTRACER RayTracerPtr,
    _In_ RAY WorldRay
    )
{
    assert(Context != nullptr);
    assert(RayTracerPtr != nullptr);
    assert(RayValidate(WorldRay) != FALSE);

    auto IntegrateContext = static_cast<IntegrateRoutine *>(Context);

    (*IntegrateContext)(Iris::Ray(WorldRay), RayTracer(RayTracerPtr));

    return ISTATUS_SUCCESS;
}

//
// Functions
//

void
Integrator::Integrate(
    _In_ const TestGeometryRoutine TestGeometryFunction,
    _In_ IntegrateRoutine IntegrateFunction,
    _In_ LightListReference Lights,
    _In_ FLOAT Epsilon,
    _In_ const Iris::Ray & WorldRay,
    _Inout_ IrisAdvanced::Random Rng
    )
{
    ISTATUS Status = PhysxIntegratorIntegrate(Data,
                                              TestGeometryRoutineAdapter,
                                              &TestGeometryFunction,
                                              IntegrateRoutineAdapter,
                                              &IntegrateFunction,
                                              Lights.AsPCPHYSX_LIGHT_LIST(),
                                              Epsilon,
                                              WorldRay.AsRAY(),
                                              Rng.AsPRANDOM());

    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(Iris::ISTATUSToCString(Status));
    }
}

void
Integrator::Integrate(
    _In_ const TestGeometryRoutine TestGeometryFunction,
    _In_ IntegrateRoutine IntegrateFunction,
    _In_ const LightList & Lights,
    _In_ FLOAT Epsilon,
    _In_ const Iris::Ray & WorldRay,
    _Inout_ IrisAdvanced::Random Rng
    )
{
    ISTATUS Status = PhysxIntegratorIntegrate(Data,
                                              TestGeometryRoutineAdapter,
                                              &TestGeometryFunction,
                                              IntegrateRoutineAdapter,
                                              &IntegrateFunction,
                                              Lights.AsPCPHYSX_LIGHT_LIST(),
                                              Epsilon,
                                              WorldRay.AsRAY(),
                                              Rng.AsPRANDOM());

    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(Iris::ISTATUSToCString(Status));
    }
}

IRISPHYSXPLUSPLUSAPI
void
Integrator::Integrate(
    _In_ const TestGeometryRoutine TestGeometryFunction,
    _In_ IntegrateRoutine IntegrateFunction,
    _In_ FLOAT Epsilon,
    _In_ const Iris::Ray & WorldRay,
    _Inout_ IrisAdvanced::Random Rng
    )
{
    ISTATUS Status = PhysxIntegratorIntegrate(Data,
                                              TestGeometryRoutineAdapter,
                                              &TestGeometryFunction,
                                              IntegrateRoutineAdapter,
                                              &IntegrateFunction,
                                              nullptr,
                                              Epsilon,
                                              WorldRay.AsRAY(),
                                              Rng.AsPRANDOM());

    if (Status != ISTATUS_SUCCESS)
    {
        throw std::runtime_error(Iris::ISTATUSToCString(Status));
    }
}

} // namespace IrisPhysx
