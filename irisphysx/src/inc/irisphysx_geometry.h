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
// Prototypes
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxGeometryAllocateInternal(
    _In_ PCPHYSX_GEOMETRY_VTABLE GeometryVTable,
    _In_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_GEOMETRY *Geometry
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxGeometryTestRayAdapter(
    _In_ PCVOID Context,
    _In_ RAY Ray,
    _In_ PHIT_ALLOCATOR HitAllocator,
    _Out_ PHIT_LIST *HitList
    );

_Ret_
PCPHYSX_GEOMETRY_VTABLE
PhysxGeometryGetVTable(
    _In_ PCPHYSX_GEOMETRY Geometry
    );

_Ret_
PVOID
PhysxGeometryGetMutableData(
    _In_ PCPHYSX_GEOMETRY Geometry
    );

_Ret_
PCVOID
PhysxGeometryGetData(
    _In_ PCPHYSX_GEOMETRY Geometry
    );

VOID
PhysxGeometryFree(
    _In_ _Post_invalid_ PPHYSX_GEOMETRY Geometry
    );

#endif // _PBR_GEOMETRY_IRIS_PHYSX_INTERNAL_
