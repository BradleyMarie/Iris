/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisrgb_surfacenormal.h

Abstract:

    This file contains the definitions for the SURFACE_NORMAL type.

--*/

#ifndef _SURFACE_NORMAL_IRIS_RGB_
#define _SURFACE_NORMAL_IRIS_RGB_

#include <irisrgb.h>

//
// Types
//

typedef struct _SURFACE_NORMAL SURFACE_NORMAL, *PSURFACE_NORMAL;
typedef CONST SURFACE_NORMAL *PCSURFACE_NORMAL;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISRGBAPI
ISTATUS
SurfaceNormalGetWorldNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 WorldNormal
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISRGBAPI
ISTATUS
SurfaceNormalGetModelNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 ModelNormal
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISRGBAPI
ISTATUS
SurfaceNormalGetNormalizedWorldNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 NormalizedWorldNormal
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISRGBAPI
ISTATUS
SurfaceNormalGetNormalizedModelNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 NormalizedModelNormal
    );

#endif // _SURFACE_NORMAL_IRIS_RGB_