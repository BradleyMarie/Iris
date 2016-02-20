/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_shape.h

Abstract:

    This file contains the definitions for the 
    Iris++ shape type.

--*/

#include <irisplusplus.h>

#ifndef _SHAPE_IRIS_PLUS_PLUS_
#define _SHAPE_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class Shape final {
public:
    IRISPLUSPLUSAPI
    Shape(
        _In_ PSHAPE ShapePtr,
        _In_ bool Retain
        )
    : Data(ShapePtr)
    {
        if (Retain)
        {
            ShapeRetain(ShapePtr);
        }
    }

    Shape(
        const Shape & ToCopy
        )
    : Data(ToCopy.Data)
    {
        ShapeRetain(Data);
    }
        
    _Ret_
    PSHAPE
    AsPSHAPE(
        void
        )
    {
        return Data;
    }

    _Ret_
    PCSHAPE
    AsPCSHAPE(
        void
        ) const
    {
        return Data;
    }

    IRISPLUSPLUSAPI
    Shape & 
    operator=(
        _In_ Shape & ToCopy
        );

    virtual
    ~Shape(
        void
        )
    { 
        ShapeRelease(Data);
    }

private:
    PSHAPE Data;
};

} // namespace Iris

#endif // _SHAPE_IRIS_PLUS_PLUS_