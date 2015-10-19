/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisadvaced_advancedshape.h

Abstract:

    This file contains the definitions for the ADVANCED_SHAPE base type.

--*/

#ifndef _ADVANCED_SHAPE_IRIS_ADVANCED_
#define _ADVANCED_SHAPE_IRIS_ADVANCED_

#include <irisadvanced.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PNORMAL_COMPUTE_SHAPE_NORMAL_ROUTINE)(
    _In_ PCVOID Context, 
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PADVANCED_SHAPE_CHECK_BOUNDS)(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    );

typedef struct _ADVANCED_SHAPE_VTABLE {
    SHAPE_VTABLE ShapeVTable;
    PNORMAL_COMPUTE_SHAPE_NORMAL_ROUTINE ComputeShapeNormalRoutine;
    PADVANCED_SHAPE_CHECK_BOUNDS CheckBoundsRoutine;
} ADVANCED_SHAPE_VTABLE, *PADVANCED_SHAPE_VTABLE;

typedef CONST ADVANCED_SHAPE_VTABLE *PCADVANCED_SHAPE_VTABLE;

typedef struct _ADVANCED_SHAPE ADVANCED_SHAPE, *PADVANCED_SHAPE;
typedef CONST ADVANCED_SHAPE *PCADVANCED_SHAPE;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
AdvancedShapeAllocate(
    _In_ PCADVANCED_SHAPE_VTABLE AdvancedShapeVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PADVANCED_SHAPE *AdvancedShape
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS 
AdvancedShapeComputeNormal(
    _In_ PCADVANCED_SHAPE AdvancedShape,
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS 
AdvancedShapeCheckBounds(
    _In_ PCADVANCED_SHAPE AdvancedShape,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    );

IRISADVANCEDAPI
VOID
AdvancedShapeReference(
    _In_opt_ PADVANCED_SHAPE AdvancedShape
    );

IRISADVANCEDAPI
VOID
AdvancedShapeDereference(
    _In_opt_ _Post_invalid_ PADVANCED_SHAPE AdvancedShape
    );

#endif // _ADVANCED_SHAPE_IRIS_ADVANCED_