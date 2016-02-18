/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    shapereference.c

Abstract:

    This file contains the function definitions for the SHAPE_REFERENCE type.

--*/

#include <irisp.h>

//
// Functions
//

_Ret_
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