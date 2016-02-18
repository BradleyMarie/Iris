/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    shapereference.c

Abstract:

    This file contains the function definitions for the SHAPE_REFERENCE type.

--*/

#define _IRIS_EXPORT_SHAPE_REFERENCE_ROUTINES_
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
    PCSHAPE_VTABLE Result;
    
    Result = StaticShapeReferenceGetVTable(ShapeReference);
    
    return Result;
}

_Ret_
PCVOID
ShapeReferenceGetData(
    _In_ PCSHAPE_REFERENCE ShapeReference
    )
{
    PCVOID Result;
    
    Result = StaticShapeReferenceGetData(ShapeReference);
    
    return Result;
}