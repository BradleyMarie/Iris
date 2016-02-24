/*++

Copyright (c) 2015 Brad Weinberger

Module Name:

    shape.c

Abstract:

    This file contains the function definitions for the SPECTRUM_SHAPE type.

--*/

#include <irisphysx.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumShapeAllocate(
    _In_ PCSPECTRUM_SHAPE_VTABLE SpectrumShapeVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSPECTRUM_SHAPE *SpectrumShape
    )
{
    PCSHAPE_VTABLE ShapeVTable;
    PSHAPE *Shape;
    ISTATUS Status;

    ShapeVTable = (PCSHAPE_VTABLE) SpectrumShapeVTable;
    Shape = (PSHAPE *) SpectrumShape;

    Status = ShapeAllocate(ShapeVTable,
                           Data,
                           DataSizeInBytes,
                           DataAlignment,
                           Shape);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
SpectrumShapeCheckBounds(
    _In_ PCSPECTRUM_SHAPE SpectrumShape,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    )
{
    PCSPECTRUM_SHAPE_VTABLE SpectrumShapeVTable;
    PCSHAPE_VTABLE ShapeVTable;
    ISTATUS Status;
    PCSHAPE Shape;
    PCVOID Data;

    if (SpectrumShape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (IsInsideBox == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Shape = (PCSHAPE) SpectrumShape;

    Data = ShapeGetData(Shape);

    ShapeVTable = ShapeGetVTable(Shape);

    SpectrumShapeVTable = (PCSPECTRUM_SHAPE_VTABLE) ShapeVTable;

    Status = SpectrumShapeVTable->CheckBoundsRoutine(Data,
                                                     ModelToWorld,
                                                     WorldAlignedBoundingBox,
                                                     IsInsideBox);

    return Status;
}


_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumShapeGetMaterial(
    _In_ PCSPECTRUM_SHAPE SpectrumShape,
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCMATERIAL *Material
    )
{
    PCSPECTRUM_SHAPE_VTABLE SpectrumShapeVTable;
    PCSHAPE_VTABLE ShapeVTable;
    ISTATUS Status;
    PCSHAPE Shape;
    PCVOID Data;

    if (SpectrumShape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Shape = (PCSHAPE) SpectrumShape;

    Data = ShapeGetData(Shape);

    ShapeVTable = ShapeGetVTable(Shape);

    SpectrumShapeVTable = (PCSPECTRUM_SHAPE_VTABLE) ShapeVTable;

    Status = SpectrumShapeVTable->GetMaterialRoutine(Data, 
                                                     FaceHit, 
                                                     Material);

    return Status;
}

_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumShapeGetLight(
    _In_ PCSPECTRUM_SHAPE SpectrumShape,
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCLIGHT *Light
    )
{
    PSPECTRUM_SHAPE_GET_LIGHT_ROUTINE GetLightRoutine;
    PCSPECTRUM_SHAPE_VTABLE SpectrumShapeVTable;
    PCSHAPE_VTABLE ShapeVTable;
    ISTATUS Status;
    PCSHAPE Shape;
    PCVOID Data;

    if (SpectrumShape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    Shape = (PCSHAPE) SpectrumShape;

    Data = ShapeGetData(Shape);

    ShapeVTable = ShapeGetVTable(Shape);

    SpectrumShapeVTable = (PCSPECTRUM_SHAPE_VTABLE) ShapeVTable;

    GetLightRoutine = SpectrumShapeVTable->GetLightRoutine;

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

VOID
SpectrumShapeReference(
    _In_opt_ PCSPECTRUM_SHAPE SpectrumShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) SpectrumShape;

    ShapeRetain(Shape);
}

VOID
SpectrumShapeDereference(
    _In_opt_ _Post_invalid_ PCSPECTRUM_SHAPE SpectrumShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) SpectrumShape;

    ShapeRelease(Shape);
}