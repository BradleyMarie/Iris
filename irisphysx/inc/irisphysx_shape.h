/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_shape.h

Abstract:

    This file contains the definitions for the PHYSX_SHAPE type.

--*/

#ifndef _PHYSX_SHAPE_IRIS_PHYSX_
#define _PHYSX_SHAPE_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PBR_SHAPE_GET_MATERIAL_ROUTINE)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCMATERIAL *Material
    );

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PBR_SHAPE_GET_LIGHT_ROUTINE)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCLIGHT *Light
    );

typedef struct _PBR_SHAPE_VTABLE {
    ADVANCED_SHAPE_VTABLE AdvancedShapeVTable;
    PBR_SHAPE_GET_MATERIAL_ROUTINE GetMaterialRoutine;
    PBR_SHAPE_GET_LIGHT_ROUTINE GetLightRoutine;
} PBR_SHAPE_VTABLE, *PPBR_SHAPE_VTABLE;

typedef CONST PBR_SHAPE_VTABLE *PCPBR_SHAPE_VTABLE;

typedef struct _PPBR_SHAPE PBR_SHAPE, *PPBR_SHAPE;
typedef CONST PBR_SHAPE *PCPBR_SHAPE;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PBRShapeAllocate(
    _In_ PCPBR_SHAPE_VTABLE PBRShapeVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_SHAPE *PBRShape
    )
{
    PCADVANCED_SHAPE_VTABLE AdvancedShapeVTable;
    PADVANCED_SHAPE *AdvancedShape;
    ISTATUS Status;

    //
    // &PBRShapeVTable->AdvancedShapeVTable should be safe to do even if
    // PBRShapeVTable == NULL.
    //

    AdvancedShapeVTable = (PCADVANCED_SHAPE_VTABLE) &PBRShapeVTable->AdvancedShapeVTable;
    AdvancedShape = (PADVANCED_SHAPE *) PBRShape;

    Status = AdvancedShapeAllocate(AdvancedShapeVTable,
                                   Data,
                                   DataSizeInBytes,
                                   DataAlignment,
                                   AdvancedShape);

    return Status;
}

_Ret_
SFORCEINLINE
PCPBR_SHAPE_VTABLE
PBRShapeGetVTable(
    _In_ PCPBR_SHAPE PBRShape
    )
{
    PCADVANCED_SHAPE AdvancedShape;
    PCPBR_SHAPE_VTABLE VTable;
    
    AdvancedShape = (PCADVANCED_SHAPE) PBRShape;
    VTable = (PCPBR_SHAPE_VTABLE) AdvancedShapeGetVTable(AdvancedShape);
    
    return VTable;
}

_Ret_
SFORCEINLINE
PCVOID
PBRShapeGetData(
    _In_ PCPBR_SHAPE PBRShape
    )
{
    PCADVANCED_SHAPE AdvancedShape;
    PCVOID Data;
    
    AdvancedShape = (PCADVANCED_SHAPE) PBRShape;
    Data = AdvancedShapeGetData(AdvancedShape);
    
    return Data;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
PBRShapeCheckBounds(
    _In_ PCPBR_SHAPE PBRShape,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCADVANCED_SHAPE AdvancedShape;
    ISTATUS Status;
    
    AdvancedShape = (PCADVANCED_SHAPE) PBRShape;
    
    Status = AdvancedShapeCheckBounds(AdvancedShape,
                                      ModelToWorld,
                                      WorldAlignedBoundingBox,
                                      IsInsideBox);
    
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PBRShapeComputeNormal(
    _In_ PCPBR_SHAPE PBRShape,
    _In_ POINT3 ModelHitPoint,
    _In_ UINT32 FaceHit,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCADVANCED_SHAPE AdvancedShape;
    ISTATUS Status;
    
    AdvancedShape = (PCADVANCED_SHAPE) PBRShape;
    
    Status = AdvancedShapeComputeNormal(AdvancedShape,
                                        ModelHitPoint,
                                        FaceHit,
                                        SurfaceNormal);
    
    return Status;
}

_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PBRShapeGetMaterial(
    _In_ PCPBR_SHAPE PBRShape,
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCMATERIAL *Material
    )
{
    PCPBR_SHAPE_VTABLE PBRShapeVTable;
    ISTATUS Status;
    PCVOID Data;

    if (PBRShape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Data = PBRShapeGetData(PBRShape);
    PBRShapeVTable = PBRShapeGetVTable(PBRShape);

    Status = PBRShapeVTable->GetMaterialRoutine(Data, 
                                                FaceHit, 
                                                Material);

    return Status;
}

_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PBRShapeGetLight(
    _In_ PCPBR_SHAPE PBRShape,
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCLIGHT *Light
    )
{
    PBR_SHAPE_GET_LIGHT_ROUTINE GetLightRoutine;
    PCPBR_SHAPE_VTABLE PBRShapeVTable;
    ISTATUS Status;
    PCVOID Data;

    if (PBRShape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Data = PBRShapeGetData(PBRShape);
    PBRShapeVTable = PBRShapeGetVTable(PBRShape);

    GetLightRoutine = PBRShapeVTable->GetLightRoutine;

    if (GetLightRoutine == NULL)
    {
        *Light = NULL;
        return ISTATUS_SUCCESS;
    }

    Status = GetLightRoutine(Data, 
                             FaceHit,
                             Light);

    return Status;
}

SFORCEINLINE
PCADVANCED_SHAPE
PBRShapeAsConstantAdvancedShape(
    _In_ PCPBR_SHAPE PBRShape
    )
{
    PCADVANCED_SHAPE AdvancedShape;
    
    AdvancedShape = (PCADVANCED_SHAPE) PBRShape;

    return AdvancedShape;
}

SFORCEINLINE
PADVANCED_SHAPE
PBRShapeAsAdvancedShape(
    _In_ PPBR_SHAPE PBRShape
    )
{
    PADVANCED_SHAPE AdvancedShape;
    
    AdvancedShape = (PADVANCED_SHAPE) PBRShape;

    return AdvancedShape;
}

SFORCEINLINE
PCSHAPE
PBRShapeAsConstantShape(
    _In_ PCPBR_SHAPE PBRShape
    )
{
    PCADVANCED_SHAPE AdvancedShape;
    PCSHAPE Shape;
    
    AdvancedShape = (PADVANCED_SHAPE) PBRShape;
    Shape = AdvancedShapeAsConstantShape(AdvancedShape);

    return Shape;
}

SFORCEINLINE
PSHAPE
PBRShapeAsShape(
    _In_ PPBR_SHAPE PBRShape
    )
{
    PADVANCED_SHAPE AdvancedShape;
    PSHAPE Shape;
    
    AdvancedShape = (PADVANCED_SHAPE) PBRShape;
    Shape = AdvancedShapeAsShape(AdvancedShape);

    return Shape;
}

SFORCEINLINE
PCPBR_SHAPE
PBRShapeFromConstantAdvancedShape(
    _In_ PCADVANCED_SHAPE AdvancedShape
    )
{
    PCPBR_SHAPE PBRShape;

    PBRShape = (PCPBR_SHAPE) AdvancedShape;

    return PBRShape;
}

SFORCEINLINE
PPBR_SHAPE
PBRShapeFromAdvancedShape(
    _In_ PADVANCED_SHAPE AdvancedShape
    )
{
    PPBR_SHAPE PBRShape;

    PBRShape = (PPBR_SHAPE) AdvancedShape;

    return PBRShape;
}

SFORCEINLINE
PCPBR_SHAPE
PBRShapeFromConstantShape(
    _In_ PCSHAPE Shape
    )
{
    PCADVANCED_SHAPE AdvancedShape;
    PCPBR_SHAPE PBRShape;

    AdvancedShape = AdvancedShapeFromConstantShape(Shape);
    PBRShape = (PCPBR_SHAPE) AdvancedShape;

    return PBRShape;
}

SFORCEINLINE
PPBR_SHAPE
PBRShapeFromShape(
    _In_ PSHAPE Shape
    )
{
    PADVANCED_SHAPE AdvancedShape;
    PPBR_SHAPE PBRShape;

    AdvancedShape = AdvancedShapeFromShape(Shape);
    PBRShape = (PPBR_SHAPE) AdvancedShape;

    return PBRShape;
}

SFORCEINLINE
VOID
PBRShapeRetain(
    _In_opt_ PCPBR_SHAPE PBRShape
    )
{
    PADVANCED_SHAPE AdvancedShape;

    AdvancedShape = (PADVANCED_SHAPE) PBRShape;

    AdvancedShapeRetain(AdvancedShape);
}

SFORCEINLINE
VOID
PBRShapeRelease(
    _In_opt_ _Post_invalid_ PCPBR_SHAPE PBRShape
    )
{
    PADVANCED_SHAPE AdvancedShape;

    AdvancedShape = (PADVANCED_SHAPE) PBRShape;

    AdvancedShapeRelease(AdvancedShape);
}

#endif // _PPBR_SHAPE_IRIS_PHYSX_