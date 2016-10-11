/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_light.h

Abstract:

    This file contains the definitions for the PBR_LIGHT type.

--*/

#ifndef _PBR_LIGHT_IRIS_PHYSX_
#define _PBR_LIGHT_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef struct _PBR_VISIBILITY_TESTER PBR_VISIBILITY_TESTER, *PPBR_VISIBILITY_TESTER;
typedef CONST PBR_VISIBILITY_TESTER *PCPBR_VISIBILITY_TESTER;

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPBR_LIGHT_SAMPLE)(
    _In_ PCVOID Context,
    _In_ POINT3 HitPoint,
    _Inout_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PVECTOR3 ToLight,
    _Out_ PFLOAT Pdf
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPBR_LIGHT_COMPUTE_EMISSIVE)(
    _In_ PCVOID Context,
    _In_ RAY ToLight,
    _Inout_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPBR_LIGHT_COMPUTE_EMISSIVE_WITH_PDF)(
    _In_ PCVOID Context,
    _In_ RAY ToLight,
    _Inout_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PFLOAT Pdf
    );

typedef struct _PBR_LIGHT_VTABLE {
    PPBR_LIGHT_SAMPLE SampleRoutine;
    PPBR_LIGHT_COMPUTE_EMISSIVE ComputeEmissiveRoutine;
    PPBR_LIGHT_COMPUTE_EMISSIVE_WITH_PDF ComputeEmissiveWithPdfRoutine;
    PFREE_ROUTINE FreeRoutine;
} PBR_LIGHT_VTABLE, *PPBR_LIGHT_VTABLE;

typedef CONST PBR_LIGHT_VTABLE *PCPBR_LIGHT_VTABLE;

typedef struct _PBR_LIGHT PBR_LIGHT, *PPBR_LIGHT;
typedef CONST PBR_LIGHT *PCPBR_LIGHT;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrLightAllocate(
    _In_ PCPBR_LIGHT_VTABLE PbrLightVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_LIGHT *PbrLight
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrLightSample(
    _In_ PCPBR_LIGHT Light,
    _In_ POINT3 HitPoint,
    _Inout_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PVECTOR3 ToLight,
    _Out_ PFLOAT Pdf
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrLightComputeEmissive(
    _In_ PCPBR_LIGHT Light,
    _In_ RAY ToLight,
    _Inout_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PbrLightComputeEmissiveWithPdf(
    _In_ PCPBR_LIGHT Light,
    _In_ RAY ToLight,
    _Inout_ PPBR_VISIBILITY_TESTER PBRVisibilityTester,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum,
    _Out_ PFLOAT Pdf
    );

IRISPHYSXAPI
VOID
PbrLightRetain(
    _In_opt_ PPBR_LIGHT Light
    );

IRISPHYSXAPI
VOID
PbrLightRelease(
    _In_opt_ _Post_invalid_ PPBR_LIGHT Light
    );

#endif // _PBR_LIGHT_IRIS_PHYSX_