/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_surfacenormal.h

Abstract:

    This file contains the definitions for the SURFACE_NORMAL type.

--*/

#ifndef _SURFACE_NORMAL_IRIS_SHADING_MODEL_
#define _SURFACE_NORMAL_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef struct _SURFACE_NORMAL SURFACE_NORMAL, *PSURFACE_NORMAL;
typedef CONST SURFACE_NORMAL *PCSURFACE_NORMAL;

//
// Functions
//

IRISSHADINGMODELAPI
VOID
SuraceNormalGetWorldNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 WorldNormal
    );

IRISSHADINGMODELAPI
VOID
SuraceNormalGetModelNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 ModelNormal
    );

IRISSHADINGMODELAPI
VOID
SuraceNormalGetNormalizedWorldNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 NormalizedWorldNormal
    );

IRISSHADINGMODELAPI
VOID
SuraceNormalGetNormalizedModelNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 NormalizedWorldNormal
    );

#endif // _SURFACE_NORMAL_IRIS_SHADING_MODEL_