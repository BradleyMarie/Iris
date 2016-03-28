/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_geometry.h

Abstract:

    This file contains the definitions for the PBR_GEOMETRY type.

--*/

#ifndef _PBR_GEOMETRY_IRIS_PHYSX_INTERNAL_
#define _PBR_GEOMETRY_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _PBR_GEOMETRY {
    PCPBR_GEOMETRY_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRGeometryTestRayAdapter(
    _In_ PCVOID Context,
    _In_ RAY Ray,
    _In_ PHIT_ALLOCATOR HitAllocator,
    _Out_ PHIT_LIST *HitList
    );

#endif // _PBR_GEOMETRY_IRIS_PHYSX_INTERNAL_