/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisphysx_raytracer.h

Abstract:

    This file contains the definitions for the PHYSX_RAYTRACER type.

--*/

#ifndef _PHYSX_RAYTRACER_IRIS_PHYSX_
#define _PHYSX_RAYTRACER_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef struct _PHYSX_RAYTRACER PHYSX_RAYTRACER, *PPHYSX_RAYTRACER;
typedef CONST PHYSX_RAYTRACER *PCPHYSX_RAYTRACER;

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_RAYTRACER_TEST_GEOMETRY_ROUTINE)(
    _In_opt_ PCVOID Context,
    _Inout_ PPHYSX_HIT_TESTER HitTester,
    _In_ RAY Ray
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_RAYTRACER_LIFETIME_CALLBACK_ROUTINE)(
    _Inout_opt_ PVOID Context,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE SpectrumCompositor,
    _Inout_ PPHYSX_RAYTRACER RayTracer
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PPHYSX_RAYTRACER_PROCESS_HIT_ROUTINE)(
    _Inout_opt_ PVOID Context, 
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_ UINT32 FrontFace,
    _In_ PCMATRIX ModelToWorld,
    _In_opt_ PCVOID AdditionalData,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHitPoint,
    _In_ POINT3 WorldHitPoint,
    _In_ RAY WorldRay,
    _In_opt_ PCPHYSX_LIGHT_LIST LightList,
    _Inout_opt_ PPHYSX_RAYTRACER RayTracer,
    _Inout_ PPHYSX_VISIBILITY_TESTER VisibilityTester,
    _Inout_ PPHYSX_BRDF_ALLOCATOR Allocator,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE SpectrumCompositor,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE ReflectorCompositor,
    _Inout_ PRANDOM Rng,
    _Outptr_result_maybenull_ PCSPECTRUM *Spectrum
    );

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxRayTracerAllocate(
    _In_ PPHYSX_RAYTRACER_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_ PPHYSX_RAYTRACER_LIFETIME_CALLBACK_ROUTINE Callback,
    _Inout_opt_ PVOID CallbackContext,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE SpectrumCompositor,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE ReflectorCompositor,
    _In_opt_ PCPHYSX_LIGHT_LIST LightList,
    _Inout_ PRANDOM Rng,
    _In_ SIZE_T MaximumDepth,
    _In_ FLOAT Epsilon
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxRayTracerTraceSceneProcessClosestHit(
    _Inout_ PPHYSX_RAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ PPHYSX_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext,
    _Outptr_result_maybenull_ PCSPECTRUM *Spectrum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxRayTracerTraceSceneProcessAllHitsInOrder(
    _Inout_ PPHYSX_RAYTRACER RayTracer,
    _In_ RAY Ray,
    _In_ PPHYSX_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext,
    _Outptr_result_maybenull_ PCSPECTRUM *Spectrum
    );

#endif // _PHYSX_RAYTRACER_IRIS_PHYSX_
