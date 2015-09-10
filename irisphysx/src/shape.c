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
_Ret_opt_
PSPECTRUM_SHAPE
SpectrumShapeAllocate(
    _In_ PCSPECTRUM_SHAPE_VTABLE SpectrumShapeVTable,
    _In_reads_bytes_opt_(DataSizeInBytes) PCVOID Data,
    _In_ _When_(Data == NULL, _Reserved_) SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PSHAPE Shape;

    Shape = ShapeAllocate(&SpectrumShapeVTable->AdvancedShapeVTable.ShapeVTable,
                          Data,
                          DataSizeInBytes,
                          DataAlignment);

    return (PSPECTRUM_SHAPE) Shape;
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
    PCADVANCED_SHAPE AdvancedShape;
    ISTATUS Status;

    AdvancedShape = (PCADVANCED_SHAPE) SpectrumShape;

    Status = AdvancedShapeCheckBounds(AdvancedShape,
                                      ModelToWorld,
                                      WorldAlignedBoundingBox,
                                      IsInsideBox);

    return Status;
}

_Ret_opt_
PCBRDF
SpectrumShapeGetBRDF(
    _In_ PCSPECTRUM_SHAPE SpectrumShape,
    _In_ UINT32 FaceHit
    )
{
    PCSPECTRUM_SHAPE_VTABLE SpectrumShapeVTable;
    PCSHAPE_VTABLE ShapeVTable;
    PCSHAPE Shape;
    PCVOID Data;
    PCBRDF Brdf;

    if (SpectrumShape == NULL)
    {
        return NULL;
    }

    Shape = (PCSHAPE) SpectrumShape;

    Data = ShapeGetData(Shape);

    ShapeVTable = ShapeGetVTable(Shape);

    SpectrumShapeVTable = (PCSPECTRUM_SHAPE_VTABLE) ShapeVTable;

    Brdf = SpectrumShapeVTable->GetBRDFRoutine(Data, FaceHit);

    return Brdf;
}

_Ret_opt_
IRISPHYSXAPI
PCLIGHT
SpectrumShapeGetLight(
    _In_ PCSPECTRUM_SHAPE SpectrumShape,
    _In_ UINT32 FaceHit
    )
{
    PCSPECTRUM_SHAPE_VTABLE SpectrumShapeVTable;
    PCSHAPE_VTABLE ShapeVTable;
    PCSHAPE Shape;
    PCLIGHT Light;
    PCVOID Data;

    if (SpectrumShape == NULL)
    {
        return NULL;
    }

    Shape = (PCSHAPE) SpectrumShape;

    Data = ShapeGetData(Shape);

    ShapeVTable = ShapeGetVTable(Shape);

    SpectrumShapeVTable = (PCSPECTRUM_SHAPE_VTABLE) ShapeVTable;

    Light = SpectrumShapeVTable->GetLightRoutine(Data, FaceHit);

    return Light;   
}

VOID
SpectrumShapeReference(
    _In_opt_ PCSPECTRUM_SHAPE SpectrumShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) SpectrumShape;

    ShapeReference(Shape);
}

VOID
SpectrumShapeDereference(
    _In_opt_ _Post_invalid_ PCSPECTRUM_SHAPE SpectrumShape
    )
{
    PSHAPE Shape;

    Shape = (PSHAPE) SpectrumShape;

    ShapeDereference(Shape);
}