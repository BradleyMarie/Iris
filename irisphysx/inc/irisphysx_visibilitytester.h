/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_visibilitytester.h

Abstract:

    This file contains the definitions for the SPECTRUM_VISIBILITY_TESTER type.

--*/

#ifndef _SPECTRUM_VISIBILITY_TESTER_IRIS_PHYSX_
#define _SPECTRUM_VISIBILITY_TESTER_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types are forward declared in irisphysx_light.h
//

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumVisibilityTesterTestVisibility(
    _In_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumVisibilityTesterTestVisibilityAnyDistance(
    _In_ PSPECTRUM_VISIBILITY_TESTER Tester,
    _In_ RAY WorldRay,
    _Out_ PBOOL Visible
    );

#endif // _SPECTRUM_VISIBILITY_TESTER_IRIS_PHYSX_