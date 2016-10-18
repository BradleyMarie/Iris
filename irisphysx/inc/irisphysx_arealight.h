/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_arealight.h

Abstract:

    This file contains the typedefs for the PHYSX_AREA_LIGHT type.

--*/

#ifndef _PHYSX_AREA_LIGHT_IRIS_PHYSX_
#define _PHYSX_AREA_LIGHT_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_AREA_LIGHT_COMPUTE_EMISSIVE)(
    _In_ PCVOID Context,
    _In_ POINT3 OnLight,
    _Inout_ PSPECTRUM_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCSPECTRUM *Spectrum
    );

typedef struct _PHYSX_AREA_LIGHT_VTABLE {
    PPHYSX_AREA_LIGHT_COMPUTE_EMISSIVE ComputeEmissiveRoutine;
    PFREE_ROUTINE FreeRoutine;
} PHYSX_AREA_LIGHT_VTABLE, *PPHYSX_AREA_LIGHT_VTABLE;

typedef CONST PHYSX_AREA_LIGHT_VTABLE *PCPHYSX_AREA_LIGHT_VTABLE;

#endif // _PHYSX_AREA_LIGHT_IRIS_PHYSX_