/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisphysx_raytracer.h

Abstract:

    This file contains the definitions for the PBR_RAYTRACER type.

--*/

#ifndef _PBR_RAYTRACER_IRIS_PHYSX_
#define _PBR_RAYTRACER_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef struct _PBR_RAYTRACER PBR_RAYTRACER, *PPBR_RAYTRACER;
typedef CONST PBR_RAYTRACER *PCPBR_RAYTRACER;

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PPBR_RAYTRACER_PROCESS_HIT_ROUTINE)(
    _Inout_opt_ PVOID Context, 
    _In_ PCPBR_GEOMETRY PBRGeometry,
    _In_ PCMATRIX ModelToWorld,
    _In_ VECTOR3 ModelViewer,
    _In_ POINT3 ModelHitPoint,
    _In_ POINT3 WorldHitPoint,
    _In_ RAY WorldRay,
    _In_reads_(NumberOfLights) PCLIGHT *Lights,
    _In_ SIZE_T NumberOfLights,
    _Inout_opt_ PPBR_RAYTRACER PBRRayTracer,
    _Inout_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _Inout_ PBRDF_ALLOCATOR BrdfAllocator,
    _Inout_ PSPECTRUM_COMPOSITOR SpectrumCompositor,
    _Inout_ PREFLECTOR_COMPOSITOR ReflectorCompositor,
    _Inout_ PRANDOM Rng,
    _Out_ PSPECTRUM *Spectrum
    );

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRRayTracerTraceSceneProcessClosestHit(
    _Inout_ PPBR_RAYTRACER PBRRayTracer,
    _In_ RAY Ray,
    _In_ PPBR_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext,
    _Out_ PSPECTRUM *Spectrum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRRayTracerTraceSceneProcessAllHitsInOrder(
    _Inout_ PPBR_RAYTRACER PBRRayTracer,
    _In_ RAY Ray,
    _In_ PPBR_RAYTRACER_PROCESS_HIT_ROUTINE ProcessHitRoutine,
    _Inout_opt_ PVOID ProcessHitContext,
    _Out_ PSPECTRUM *Spectrum
    );

#endif // _PBR_RAYTRACER_IRIS_PHYSX_