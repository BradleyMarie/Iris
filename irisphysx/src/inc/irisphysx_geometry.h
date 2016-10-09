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
PBRGeometryAllocateInternal(
    _In_ PCPBR_GEOMETRY_VTABLE PBRGeometryVTable,
    _In_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_GEOMETRY *PBRGeometry
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRGeometryTestRayAdapter(
    _In_ PCVOID Context,
    _In_ RAY Ray,
    _In_ PHIT_ALLOCATOR HitAllocator,
    _Out_ PHIT_LIST *HitList
    );

_Ret_
PVOID
PBRGeometryGetData(
    _In_ PCPBR_GEOMETRY PBRGeometry
    );

VOID
PBRGeometryFree(
    _In_ _Post_invalid_ PPBR_GEOMETRY PBRGeometry
    );

#endif // _PBR_GEOMETRY_IRIS_PHYSX_INTERNAL_
