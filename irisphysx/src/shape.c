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
	_When_(DataSizeInBytes != 0, _Pre_satisfies_(DataSizeInBytes % DataAlignment == 0)) SIZE_T DataSizeInBytes,
	_When_(DataSizeInBytes != 0, _Pre_satisfies_((DataAlignment & (DataAlignment - 1)) == 0)) SIZE_T DataAlignment,
	_Out_ PSPECTRUM_SHAPE *SpectrumShape
	)
{
    PADVANCED_SHAPE *AdvancedShape;
	ISTATUS Status;

    AdvancedShape = (PADVANCED_SHAPE *) SpectrumShape;

	Status = AdvancedShapeAllocate(&SpectrumShapeVTable->AdvancedShapeVTable,
                                   Data,
                                   DataSizeInBytes,
                                   DataAlignment,
                                   AdvancedShape);

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
PCMATERIAL
SpectrumShapeGetMaterial(
    _In_ PCSPECTRUM_SHAPE SpectrumShape,
    _In_ UINT32 FaceHit
    )
{
    PCSPECTRUM_SHAPE_VTABLE SpectrumShapeVTable;
    PCSHAPE_VTABLE ShapeVTable;
    PCSHAPE Shape;
    PCVOID Data;
    PCMATERIAL Material;

    if (SpectrumShape == NULL)
    {
        return NULL;
    }

    Shape = (PCSHAPE) SpectrumShape;

    Data = ShapeGetData(Shape);

    ShapeVTable = ShapeGetVTable(Shape);

    SpectrumShapeVTable = (PCSPECTRUM_SHAPE_VTABLE) ShapeVTable;

    Material = SpectrumShapeVTable->GetMaterialRoutine(Data, FaceHit);

    return Material;
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