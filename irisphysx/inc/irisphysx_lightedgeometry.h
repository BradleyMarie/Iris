/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_lightedgeometry.h

Abstract:

    This file contains the typedefs for the PHYSX_LIGHTED_GEOMETRY type.

--*/

#ifndef _PHYSX_LIGHTED_GEOMETRY_IRIS_PHYSX_
#define _PHYSX_LIGHTED_GEOMETRY_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_LIGHTED_GEOMETRY_COMPUTE_SURFACE_AREA)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 Face,
    _Out_ PFLOAT SurfaceArea
    );

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PPHYSX_LIGHTED_GEOMETRY_SAMPLE_SURFACE)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 Face,
    _Out_ PPOINT3 Sample
    );

typedef struct _PHYSX_LIGHTED_GEOMETRY_VTABLE {
    PHYSX_GEOMETRY_VTABLE Header;
    PPHYSX_LIGHTED_GEOMETRY_COMPUTE_SURFACE_AREA ComputeSurfaceAreaRoutine;
    PPHYSX_LIGHTED_GEOMETRY_SAMPLE_SURFACE SampleSurfaceRoutine;
} PHYSX_LIGHTED_GEOMETRY_VTABLE, *PPHYSX_LIGHTED_GEOMETRY_VTABLE;

typedef CONST PHYSX_LIGHTED_GEOMETRY_VTABLE *PCPHYSX_LIGHTED_GEOMETRY_VTABLE;

#endif // _PHYSX_LIGHTED_GEOMETRY_IRIS_PHYSX_
