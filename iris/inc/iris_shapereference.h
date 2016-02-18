/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shapereference.h

Abstract:

    This file contains the implementation of the base shape 
    reference object.

--*/

#include <iris.h>

#ifndef _IRIS_SHAPE_REFERENCE_
#define _IRIS_SHAPE_REFERENCE_

//
// Functions
//

_Ret_
IRISAPI
PCSHAPE_VTABLE
ShapeReferenceGetVTable(
    _In_ PCSHAPE_REFERENCE ShapeReference
    );

_Ret_
IRISAPI
PCVOID
ShapeReferenceGetData(
    _In_ PCSHAPE_REFERENCE ShapeReference
    );

#endif // _IRIS_SHAPE_REFERENCE_