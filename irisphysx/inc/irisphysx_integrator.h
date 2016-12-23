/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisphysx_integrator.h

Abstract:

    This file contains the definitions for the PHYSX_INTEGRATOR type.

--*/

#ifndef _PHYSX_INTEGRATOR_IRIS_PHYSX_
#define _PHYSX_INTEGRATOR_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef struct _PHYSX_INTEGRATOR PHYSX_INTEGRATOR, *PPHYSX_INTEGRATOR;
typedef CONST PHYSX_INTEGRATOR *PCPHYSX_INTEGRATOR;

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE)(
    _In_opt_ PCVOID Context, 
    _Inout_ PPHYSX_HIT_TESTER HitTester,
    _In_ RAY Ray
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PPHYSX_INTEGRATOR_INTEGRATE_ROUTINE)(
    _Inout_opt_ PVOID Context, 
    _Inout_ PPHYSX_RAYTRACER RayTracer,
    _In_ RAY Ray
    );

//
// Functions
//

_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxIntegratorAllocate(
    _In_ SIZE_T MaximumDepth,
    _Out_ PPHYSX_INTEGRATOR *Integrator
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxIntegratorIntegrate(
    _In_ PPHYSX_INTEGRATOR Integrator,
    _In_ PPHYSX_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_ PPHYSX_INTEGRATOR_INTEGRATE_ROUTINE IntegrateRoutine,
    _Inout_opt_ PVOID IntegrateRoutineContext,
    _In_opt_ PCPHYSX_LIGHT_LIST LightList,
    _In_ FLOAT Epsilon,
    _In_ RAY WorldRay,
    _Inout_ PRANDOM Rng
    );

IRISPHYSXAPI
VOID
PhysxIntegratorFree(
    _In_opt_ _Post_invalid_ PPHYSX_INTEGRATOR Integrator
    );

#endif // _PHYSX_INTEGRATOR_IRIS_PHYSX_
