/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysxtoolkit_pinhole.h

Abstract:

    This file contains the pinhole camera routines.

--*/

#ifndef _PINHOLE_IRIS_PHYSX_TOOLKIT_
#define _PINHOLE_IRIS_PHYSX_TOOLKIT_

#include <irisphysxtoolkit.h>

//
// Functions
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPBR_TONE_MAPPING_ROUTINE)(
    _In_opt_ PCVOID Context,
    _In_opt_ PCSPECTRUM Spectrum,
    _Out_ PCOLOR3 Color
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PPBR_TOOLKIT_CREATE_CAMERA_STATE_ROUTINE)(
    _In_opt_ PVOID Context, 
    _Out_writes_(NumberOfThreads) PRANDOM_REFERENCE *Rngs,
    _Out_writes_(NumberOfThreads) PPBR_RAYTRACER_PROCESS_HIT_ROUTINE *ProcessHitRoutine,
    _Out_writes_(NumberOfThreads) PVOID *ProcessHitContexts,
    _Out_writes_(NumberOfThreads) PPBR_TONE_MAPPING_ROUTINE *ToneMappingRoutines,
    _Out_writes_(NumberOfThreads) PVOID *ToneMappingContexts,
    _In_ SIZE_T NumberOfThreads
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PPBR_TOOLKIT_FREE_CAMERA_STATE_ROUTINE)(
    _In_opt_ PVOID Context,
    _In_reads_(NumberOfThreads) PVOID *ProcessHitContexts,
    _In_reads_(NumberOfThreads) PVOID *ToneMappingContexts,
    _In_ SIZE_T NumberOfThreads
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXTOOLKITAPI
ISTATUS
PinholeRender(
    _In_ POINT3 PinholeLocation,
    _In_ FLOAT ImagePlaneDistance,
    _In_ FLOAT ImagePlaneHeight,
    _In_ FLOAT ImagePlaneWidth,
    _In_ VECTOR3 CameraDirection,
    _In_ VECTOR3 Up,
    _In_ SIZE_T AdditionalXSamplesPerPixel,
    _In_ SIZE_T AdditionalYSamplesPerPixel,
    _In_ FLOAT Epsilon,
    _In_ SIZE_T MaxDepth,
    _In_ BOOL Jitter,
    _In_ BOOL Parallelize,
    _In_ PPBR_INTEGRATOR_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID TestGeometryRoutineContext,
    _In_opt_ PCPHYSX_LIGHT_LIST Lights,
    _In_ PPBR_TOOLKIT_CREATE_CAMERA_STATE_ROUTINE CreateStateRoutine,
    _In_opt_ PPBR_TOOLKIT_FREE_CAMERA_STATE_ROUTINE FreeCameraStateRoutine,
    _Inout_opt_ PVOID CreateStateContext,
    _Inout_ PFRAMEBUFFER Framebuffer
    );

#endif // _PINHOLE_IRIS_PHYSX_TOOLKIT_