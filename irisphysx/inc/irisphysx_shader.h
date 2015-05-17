/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_shader.h

Abstract:

    This file contains the definitions for the SPECTRUM_SHADER type.

--*/

#ifndef _SPECTRUM_SHADER_IRIS_PHYSX_
#define _SPECTRUM_SHADER_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PSPECTRUM_SHADER_SAMPLE_ROUTINE)(
    _In_ PCVOID Context,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_reads_opt_(2) PCRAY WorldDifferentials[2],
    _In_ FLOAT Distance,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_ PVOID RayTracer,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Output
    );

typedef struct _SPECTRUM_SHADER_VTABLE {
    PSPECTRUM_SHADER_SAMPLE_ROUTINE SampleRoutine;
    PFREE_ROUTINE FreeRoutine;
} SPECTRUM_SHADER_VTABLE, *PSPECTRUM_SHADER_VTABLE;

typedef CONST SPECTRUM_SHADER_VTABLE *PCSPECTRUM_SHADER_VTABLE;

typedef struct _SPECTRUM_SHADER SPECTRUM_SHADER, *PSPECTRUM_SHADER;
typedef CONST SPECTRUM_SHADER *PCSPECTRUM_SHADER;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISPHYSXAPI
PSPECTRUM_SHADER
SpectrumShaderAllocate(
    _In_ PCSPECTRUM_SHADER_VTABLE SpectrumShaderVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumShaderSample(
    _In_ PCSPECTRUM_SHADER SpectrumShader,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_reads_opt_(2) PCRAY WorldDifferentials[2],
    _In_ FLOAT Distance,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_ PVOID RayTracer,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Output
    );

IRISPHYSXAPI
VOID
SpectrumShaderReference(
    _In_opt_ PSPECTRUM_SHADER SpectrumShader
    );

IRISPHYSXAPI
VOID
SpectrumShaderDereference(
    _In_opt_ _Post_invalid_ PSPECTRUM_SHADER SpectrumShader
    );

#endif // _SPECTRUM_SHADER_IRIS_PHYSX_