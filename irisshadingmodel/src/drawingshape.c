/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    drawingshape.c

Abstract:

    This file contains the function definitions for the DRAWING_SHAPE type.

--*/

#include <irisshadingmodelp.h>

//
// Types
//

struct _DRAWING_SHAPE {
    PCDRAWING_SHAPE_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_opt_
PDRAWING_SHAPE
DrawingShapeAllocate(
    _In_ PCDRAWING_SHAPE_VTABLE DrawingShapeVTable,
    _In_reads_bytes_opt_(DataSizeInBytes) PCVOID Data,
    _In_ _When_(Data == NULL, _Reserved_) SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PDRAWING_SHAPE AllocatedShape;

    if (DrawingShapeVTable == NULL)
    {
        return NULL;
    }

    if (DataSizeInBytes != 0)
    {
        if (Data == NULL)
        {
            return NULL;
        }

        if (DataAlignment == 0 ||
            DataAlignment & DataAlignment - 1)
        {
            return NULL;
        }

        if (DataSizeInBytes % DataAlignment != 0)
        {
            return NULL;
        }
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(DRAWING_SHAPE),
                                                      _Alignof(DRAWING_SHAPE),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return NULL;
    }

    AllocatedShape = (PDRAWING_SHAPE) HeaderAllocation;

    AllocatedShape->VTable = DrawingShapeVTable;
    AllocatedShape->Data = DataAllocation;
    AllocatedShape->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return AllocatedShape;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
DrawingShapeTrace(
    _In_opt_ PCDRAWING_SHAPE Context,
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    return Context->VTable->TestGeometryRoutine(Context, Context->Data, Ray, HitAllocator, HitList);
}

_Ret_opt_
PCTEXTURE
DrawingShapeGetTexture(
    _In_ PCDRAWING_SHAPE DrawingShape,
    _In_ UINT32 FaceHit
    )
{
    ASSERT(DrawingShape != NULL);

    return DrawingShape->VTable->GetTextureRoutine(DrawingShape->Data, FaceHit);
}

_Ret_opt_
PCNORMAL
DrawingShapeGetNormal(
    _In_ PCDRAWING_SHAPE DrawingShape,
    _In_ UINT32 FaceHit
    )
{
    ASSERT(DrawingShape != NULL);

    return DrawingShape->VTable->GetNormalRoutine(DrawingShape->Data, FaceHit);
}

VOID
DrawingShapeReference(
    _In_opt_ PDRAWING_SHAPE DrawingShape
    )
{
    if (DrawingShape == NULL)
    {
        return;
    }

    DrawingShape->ReferenceCount += 1;
}

VOID
DrawingShapeDereference(
    _In_opt_ _Post_invalid_ PDRAWING_SHAPE DrawingShape
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (DrawingShape == NULL)
    {
        return;
    }

    DrawingShape->ReferenceCount -= 1;

    if (DrawingShape->ReferenceCount == 0)
    {
        FreeRoutine = DrawingShape->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(DrawingShape->Data);
        }

        IrisAlignedFree(DrawingShape);
    }
}