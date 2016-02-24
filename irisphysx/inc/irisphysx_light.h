/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_light.h

Abstract:

    This file contains the definitions for the LIGHT type.

--*/

#ifndef _LIGHT_IRIS_PHYSX_
#define _LIGHT_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef struct _SPECTRUM_VISIBILITY_TESTER SPECTRUM_VISIBILITY_TESTER, *PSPECTRUM_VISIBILITY_TESTER;
typedef CONST SPECTRUM_VISIBILITY_TESTER *PCSPECTRUM_VISIBILITY_TESTER;

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PLIGHT_SAMPLE)(
    _In_ PCVOID Context,
    _In_ POINT3 HitPoint,
    _Inout_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PVECTOR3 ToLight,
    _Out_ PFLOAT Pdf
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PLIGHT_COMPUTE_EMISSIVE)(
    _In_ PCVOID Context,
    _In_ RAY ToLight,
    _Inout_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _Out_ PCSPECTRUM *Spectrum
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PLIGHT_COMPUTE_EMISSIVE_WITH_PDF)(
    _In_ PCVOID Context,
    _In_ RAY ToLight,
    _Inout_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PFLOAT Pdf
    );

typedef struct _LIGHT_VTABLE {
    PLIGHT_SAMPLE SampleRoutine;
    PLIGHT_COMPUTE_EMISSIVE ComputeEmissiveRoutine;
    PLIGHT_COMPUTE_EMISSIVE_WITH_PDF ComputeEmissiveWithPdfRoutine;
    PFREE_ROUTINE FreeRoutine;
} LIGHT_VTABLE, *PLIGHT_VTABLE;

typedef CONST LIGHT_VTABLE *PCLIGHT_VTABLE;

typedef struct _LIGHT LIGHT, *PLIGHT;
typedef CONST LIGHT *PCLIGHT;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
LightAllocate(
    _In_ PCLIGHT_VTABLE LightVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PLIGHT *Light
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
LightSample(
    _In_ PCLIGHT Light,
    _In_ POINT3 HitPoint,
    _Inout_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PVECTOR3 ToLight,
    _Out_ PFLOAT Pdf
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
LightComputeEmissive(
    _In_ PCLIGHT Light,
    _In_ RAY ToLight,
    _Inout_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _Out_ PCSPECTRUM *Spectrum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
LightComputeEmissiveWithPdf(
    _In_ PCLIGHT Light,
    _In_ RAY ToLight,
    _Inout_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PFLOAT Pdf
    );

IRISPHYSXAPI
VOID
LightReference(
    _In_opt_ PLIGHT Light
    );

IRISPHYSXAPI
VOID
LightDereference(
    _In_opt_ _Post_invalid_ PLIGHT Light
    );

#endif // _LIGHT_IRIS_PHYSX_