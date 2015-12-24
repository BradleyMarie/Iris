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
(*PSPECTRUM_SHAPE_GET_MATERIAL_ROUTINE)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCMATERIAL *Material
    );

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PSPECTRUM_SHAPE_GET_LIGHT_ROUTINE)(
    _In_opt_ PCVOID Context, 
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCLIGHT *Light
    );

typedef struct _SPECTRUM_SHAPE_VTABLE {
    ADVANCED_SHAPE_VTABLE AdvancedShapeVTable;
    PSPECTRUM_SHAPE_GET_MATERIAL_ROUTINE GetMaterialRoutine;
    PSPECTRUM_SHAPE_GET_LIGHT_ROUTINE GetLightRoutine;
} SPECTRUM_SHAPE_VTABLE, *PSPECTRUM_SHAPE_VTABLE;

typedef CONST SPECTRUM_SHAPE_VTABLE *PCSPECTRUM_SHAPE_VTABLE;

typedef struct _SPECTRUM_SHAPE SPECTRUM_SHAPE, *PSPECTRUM_SHAPE;
typedef CONST SPECTRUM_SHAPE *PCSPECTRUM_SHAPE;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumShapeAllocate(
    _In_ PCSPECTRUM_SHAPE_VTABLE SpectrumShapeVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PSPECTRUM_SHAPE *Shape
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS 
SpectrumShapeCheckBounds(
    _In_ PCSPECTRUM_SHAPE SpectrumShape,
    _In_ PCMATRIX ModelToWorld,
    _In_ BOUNDING_BOX WorldAlignedBoundingBox,
    _Out_ PBOOL IsInsideBox
    );

_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumShapeGetMaterial(
    _In_ PCSPECTRUM_SHAPE SpectrumShape,
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCMATERIAL *Material
    );

_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
SpectrumShapeGetLight(
    _In_ PCSPECTRUM_SHAPE SpectrumShape,
    _In_ UINT32 FaceHit,
    _Outptr_result_maybenull_ PCLIGHT *Light
    );

IRISPHYSXAPI
VOID
SpectrumShapeReference(
    _In_opt_ PCSPECTRUM_SHAPE SpectrumShape
    );

IRISPHYSXAPI
VOID
SpectrumShapeDereference(
    _In_opt_ _Post_invalid_ PCSPECTRUM_SHAPE SpectrumShape
    );

#endif // _SPECTRUM_SHAPE_IRIS_PHYSX_