/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_integrator.h

Abstract:

    This file contains the definitions for the INTEGRATOR type.

--*/

#ifndef _PHYSX_INTEGRATOR_IRIS_PHYSX_
#define _PHYSX_INTEGRATOR_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PINTEGRATOR_INTEGRATE_ROUTINE)(
    _In_ PCVOID Context, 
    _In_ RAY WorldRay,
    _In_reads_(NumberOfLights) PCLIGHT *Lights,
    _In_ SIZE_T NumberOfLights,
    _Inout_ PSPECTRUM_RAYTRACER RayTracer,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_ PBRDF_ALLOCATOR BrdfAllocator,
    _Inout_ PSPECTRUM_COMPOSITOR SpectrumCompositor,
    _Inout_ PREFLECTOR_COMPOSITOR ReflectorCompositor,
    _Inout_ PRANDOM Rng,
    _Out_ PSPECTRUM *Spectrum
    );

typedef struct _INTEGRATOR_VTABLE {
    PINTEGRATOR_INTEGRATE_ROUTINE IntegrateRoutine;
    PFREE_ROUTINE FreeRoutine;
} INTEGRATOR_VTABLE, *PINTEGRATOR_VTABLE;

typedef CONST INTEGRATOR_VTABLE *PCINTEGRATOR_VTABLE;

typedef struct _INTEGRATOR INTEGRATOR, *PINTEGRATOR;
typedef CONST INTEGRATOR *PCINTEGRATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
IntegratorAllocate(
    _In_ PCINTEGRATOR_VTABLE IntegratorVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PINTEGRATOR *Integrator
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
IntegratorIntegrate(
    _In_ PINTEGRATOR Integrator,
    _In_ PCSPECTRUM_SCENE Scene,
    _In_ FLOAT VisibilityEpsilon,
    _In_ RAY WorldRay,
    _Inout_ PRANDOM Rng,
    _Out_ PSPECTRUM *Spectrum
    );

IRISPHYSXAPI
VOID
IntegratorFree(
    _In_opt_ _Post_invalid_ PINTEGRATOR Integrator
    );

#endif // _PHYSX_INTEGRATOR_IRIS_PHYSX_