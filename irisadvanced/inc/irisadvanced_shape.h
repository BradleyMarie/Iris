/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvanced_shape.h

Abstract:

    This file contains the definitions for the ADVANCED_SHAPE type.

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
(*PADVANCED_SHAPE_COMPUTE_NORMAL_ROUTINE)(
    _In_ PCVOID Context,
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    );

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PADVANCED_SHAPE_CHECK_BOUNDS_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    );

typedef struct _ADVANCED_SHAPE_VTABLE {
    SHAPE_VTABLE ShapeVTable;
    PADVANCED_SHAPE_COMPUTE_NORMAL_ROUTINE ComputeNormalRoutine;
    PADVANCED_SHAPE_CHECK_BOUNDS_ROUTINE CheckBoundsRoutine;
} ADVANCED_SHAPE_VTABLE, *PADVANCED_SHAPE_VTABLE;

typedef CONST ADVANCED_SHAPE_VTABLE *PCADVANCED_SHAPE_VTABLE;

typedef struct _ADVANCED_SHAPE ADVANCED_SHAPE, *PADVANCED_SHAPE;
typedef CONST ADVANCED_SHAPE *PCADVANCED_SHAPE;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
AdvancedShapeAllocate(
    _In_ PCADVANCED_SHAPE_VTABLE AdvancedShapeVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PADVANCED_SHAPE *AdvancedShape
    )
{
    PCSHAPE_VTABLE ShapeVTable;
    PSHAPE *Shape;
    ISTATUS Status;

    //
    // &AdvancedShapeVTable->ShapeVTable should be safe to do even if
    // AdvancedShapeVTable == NULL.
    //

    ShapeVTable = (PCSHAPE_VTABLE) &AdvancedShapeVTable->ShapeVTable;
    Shape = (PSHAPE *) AdvancedShape;

    Status = ShapeAllocate(ShapeVTable,
                           Data,
                           DataSizeInBytes,
                           DataAlignment,
                           Shape);

    return Status;
}

_Ret_
SFORCEINLINE
PCADVANCED_SHAPE_VTABLE
AdvancedShapeGetVTable(
    _In_ PCADVANCED_SHAPE AdvancedShape
    )
{
    PCADVANCED_SHAPE_VTABLE VTable;
    PCSHAPE Shape;
    
    Shape = (PSHAPE) AdvancedShape;
    VTable = (PCADVANCED_SHAPE_VTABLE) ShapeGetVTable(Shape);
    
    return VTable;
}

_Ret_
SFORCEINLINE
PCVOID
AdvancedShapeGetData(
    _In_ PCADVANCED_SHAPE AdvancedShape
    )
{
    PCVOID Data;
    PCSHAPE Shape;
    
    Shape = (PSHAPE) AdvancedShape;
    Data = ShapeGetData(Shape);
    
    return Data;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
AdvancedShapeCheckBounds(
    _In_ PCADVANCED_SHAPE AdvancedShape,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCADVANCED_SHAPE_VTABLE AdvancedShapeVTable;
    PCVOID Data;
    ISTATUS Status;

    if (AdvancedShape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsInsideBox == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    AdvancedShapeVTable = AdvancedShapeGetVTable(AdvancedShape);
    Data = AdvancedShapeGetData(AdvancedShape);

    Status = AdvancedShapeVTable->CheckBoundsRoutine(Data,
                                                     ModelToWorld,
                                                     WorldAlignedBoundingBox,
                                                     IsInsideBox);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
AdvancedShapeComputeNormal(
    _In_ PCADVANCED_SHAPE AdvancedShape,
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCADVANCED_SHAPE_VTABLE AdvancedShapeVTable;
    PCVOID Data;
    ISTATUS Status;

    if (AdvancedShape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (PointValidate(ModelHitPoint) != FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (SurfaceNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    AdvancedShapeVTable = AdvancedShapeGetVTable(AdvancedShape);
    Data = AdvancedShapeGetData(AdvancedShape);

    Status = AdvancedShapeVTable->ComputeNormalRoutine(Data,
                                                       ModelHitPoint,
                                                       FaceHit,
                                                       SurfaceNormal);

    return Status;
}

SFORCEINLINE
PCSHAPE
AdvancedShapeAsConstantShape(
    _In_ PCADVANCED_SHAPE AdvancedShape
    )
{
    PCSHAPE Shape;

    Shape = (PCSHAPE) AdvancedShape;

    return Shape;
}

SFORCEINLINE
PSHAPE
AdvancedShapeAsShape(
    _In_ PADVANCED_SHAPE AdvancedShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) AdvancedShape;

    return Shape;
}

SFORCEINLINE
VOID
AdvancedShapeRetain(
    _In_opt_ PADVANCED_SHAPE AdvancedShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) AdvancedShape;

    ShapeRetain(Shape);
}

SFORCEINLINE
VOID
AdvancedShapeRelease(
    _In_opt_ _Post_invalid_ PADVANCED_SHAPE AdvancedShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) AdvancedShape;

    ShapeRelease(Shape);
}

#endif // _ADVANCED_SHAPE_IRIS_ADVANCED_