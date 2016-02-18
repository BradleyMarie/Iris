/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iris_shape.h

Abstract:

    This file contains the implementation of the internal shape functions.

--*/

#ifndef _IRIS_SHAPE_REFERENCE_INTERNAL_
#define _IRIS_SHAPE_REFERENCE_INTERNAL_

#include <irisp.h>

//
// Macros
//

#ifdef _IRIS_EXPORT_SHAPE_REFERENCE_ROUTINES_
#define ShapeReferenceGetVTable(Shape) \
        StaticShapeReferenceGetVTable(Shape)

#define ShapeReferenceGetData(Shape) \
        StaticShapeReferenceGetData(Shape)
#endif

//
// Types
//

struct _SHAPE_REFERENCE {
    PCSHAPE_VTABLE VTable;
    PVOID Data;
};

//
// Functions
//

SFORCEINLINE
VOID
ShapeReferenceInitialize(
    _In_ PCSHAPE_VTABLE VTable,
    _In_opt_ PVOID Data,
    _Out_ PSHAPE_REFERENCE ShapeReference
    )
{
    ASSERT(VTable != NULL);
    ASSERT(ShapeReference != NULL);
    
    ShapeReference->VTable = VTable;
    ShapeReference->Data = Data;
}

_Ret_
SFORCEINLINE
PCSHAPE_VTABLE
ShapeReferenceGetVTable(
    _In_ PCSHAPE_REFERENCE ShapeReference
    )
{
    if (ShapeReference == NULL)
    {
        return NULL;
    }

    return ShapeReference->VTable;
}

_Ret_
SFORCEINLINE
PCVOID
ShapeReferenceGetData(
    _In_ PCSHAPE_REFERENCE ShapeReference
    )
{
    if (ShapeReference == NULL)
    {
        return NULL;
    }

    return ShapeReference->Data;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
ShapeReferenceTrace(
    _In_ PCSHAPE_REFERENCE ShapeReference, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    ISTATUS Status;

    ASSERT(ShapeReference != NULL);
    ASSERT(HitAllocator != NULL);
    ASSERT(HitList != NULL);

    HitAllocatorSetCurrentShapeReference(HitAllocator, ShapeReference);

    Status = ShapeReference->VTable->TraceRoutine(ShapeReference->Data, 
                                                  Ray,
                                                  HitAllocator,
                                                  HitList);

    return Status;
}

SFORCEINLINE
VOID
ShapeReferenceDestroy(
    _In_ _Post_invalid_ PSHAPE_REFERENCE ShapeReference
    )
{
    PFREE_ROUTINE FreeRoutine;
    
    ASSERT(ShapeReference != NULL);
    
    FreeRoutine = ShapeReference->VTable->FreeRoutine;

    if (FreeRoutine != NULL)
    {
        FreeRoutine(ShapeReference->Data);
    }
}

#ifdef _IRIS_EXPORT_SHAPE_REFERENCE_ROUTINES_
#undef ShapeReferenceGetVTable
#undef ShapeReferenceGetData
#endif

#endif // _IRIS_SHAPE_INTERNAL_