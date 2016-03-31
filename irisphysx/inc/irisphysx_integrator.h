/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisphysx_integrator.h

Abstract:

    This file contains the definitions for the PBR_INTEGRATOR type.

--*/

#ifndef _PBR_INTEGRATOR_IRIS_PHYSX_
#define _PBR_INTEGRATOR_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef struct _PBR_INTEGRATOR PBR_INTEGRATOR, *PPBR_INTEGRATOR;
typedef CONST PBR_INTEGRATOR *PCPBR_INTEGRATOR;

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE)(
    _In_opt_ PCVOID Context, 
    _Inout_ PPBR_HIT_TESTER PBRHitTester,
    _In_ RAY Ray
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PPBR_INTEGRATOR_INTEGRATE_ROUTINE)(
    _Inout_opt_ PVOID Context, 
    _Inout_ PPBR_RAYTRACER PBRRayTracer,
    _In_ RAY Ray
    );

//
// Functions
//

_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRIntegratorAllocate(
    _In_ SIZE_T MaximumDepth,
    _Out_ PPBR_INTEGRATOR *PBRIntegrator
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRIntegratorIntegrate(
    _In_ PPBR_INTEGRATOR PBRIntegrator,
    _In_ PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_ PPBR_INTEGRATOR_INTEGRATE_ROUTINE IntegrateRoutine,
    _Inout_opt_ PVOID IntegrateRoutineContext,
    _In_reads_(NumberOfLights) PCLIGHT *Lights,
    _In_ SIZE_T NumberOfLights,
    _In_ FLOAT VisibilityEpsilon,
    _In_ RAY WorldRay,
    _Inout_ PRANDOM Rng
    );

IRISPHYSXAPI
VOID
PBRIntegratorFree(
    _In_opt_ _Post_invalid_ PPBR_INTEGRATOR PBRIntegrator
    );

#endif // _PBR_INTEGRATOR_IRIS_PHYSX_