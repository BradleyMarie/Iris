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

class Shape {
protected:
    IRISPLUSPLUSAPI
    Shape(
        void
        );

public:
    PDRAWING_SHAPE
    AsPDRAWING_SHAPE(
        void
        ) const
    {
        return Data;
    }

    _Check_return_
    _Ret_opt_
    PCTEXTURE
    virtual
    GetTexture(
        _In_ UINT32 FaceHit
        ) const = 0;

    _Check_return_
    _Ret_opt_
    PCNORMAL
    virtual
    GetNormal(
        _In_ UINT32 FaceHit
        ) const = 0;

    IRISPLUSPLUSAPI
    virtual
    ~Shape(
        void
        );

private:
    PDRAWING_SHAPE Data;

    IRISPLUSPLUSAPI
    Shape(
        _In_ PDRAWING_SHAPE DrawingShape
        );

    _Check_return_ 
    _Ret_opt_
    friend
    PCTEXTURE
    ShapeGetTexture(
        _In_opt_ PCVOID Context,
        _In_ UINT32 FaceHit
        );

    _Check_return_
    _Ret_opt_
    friend
    PCNORMAL
    ShapeGetNormal(
        _In_opt_ PCVOID Context,
        _In_ UINT32 FaceHit
        );

    friend class CShape;
};

class CShape : public Shape {
public:
    CShape(
        _In_ PDRAWING_SHAPE DrawingShape
        );

    _Check_return_
    _Ret_opt_
    IRISPLUSPLUSAPI
    PCTEXTURE
    virtual
    GetTexture(
        _In_ UINT32 FaceHit
        ) const;

    _Check_return_
    _Ret_opt_
    IRISPLUSPLUSAPI
    PCNORMAL
    virtual
    GetNormal(
        _In_ UINT32 FaceHit
        ) const;
};

} // namespace Iris

#endif // _SHAPE_IRIS_PLUS_PLUS_