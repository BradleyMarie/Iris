/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    shape.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ shape type.

--*/

#include <irisplusplusp.h>

namespace Iris {

Shape & 
Shape::operator=(
    _In_ const Shape & ToCopy
    )
{
    if (this != &ToCopy)
    {
        ShapeRelease(Data);
        Data = ToCopy.Data;
        ShapeRetain(Data);
    }

    return *this;
}

} // namespace Iris