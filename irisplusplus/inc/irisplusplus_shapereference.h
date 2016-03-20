/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_shapereference.h

Abstract:

    This file contains the definitions for the 
    Iris++ shape reference type.

--*/

#include <irisplusplus.h>

#ifndef _SHAPE_REFERENCE_IRIS_PLUS_PLUS_
#define _SHAPE_REFERENCE_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class ShapeReference final {
public:
    ShapeReference(
        _In_ PCSHAPE ShapeRef
        )
    : Data(ShapeRef)
    { }
    
    _Ret_
    PCSHAPE
    AsPCSHAPE(
        void
        ) const
    {
        return Data;
    }

    ShapeReference(
        _In_ const ShapeReference & ToCopy
        )
    : Data(ToCopy.Data)
    { }

    ShapeReference & 
    operator=(
        _In_ const ShapeReference & ToCopy
        )
    {
        Data = ToCopy.Data;
        return *this;
    }

private:
    PCSHAPE Data;
};

} // namespace Iris

#endif // _SHAPE_REFERENCE_IRIS_PLUS_PLUS_