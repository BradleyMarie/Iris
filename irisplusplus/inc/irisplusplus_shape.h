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

    virtual
    ~Shape(
        void
        )
    { }

private:
    PDRAWING_SHAPE Data;

    IRISPLUSPLUSAPI
    Shape(
        _In_ PDRAWING_SHAPE DrawingShape
        );

    IRISPLUSPLUSAPI
    void
    Reference(
        void
        );

    IRISPLUSPLUSAPI
    void 
    Dereference(
        void
        );

    friend class IrisPointer<Shape>;
    friend class CShape;
};

class CShape : public Shape {
protected:
    CShape(
        _In_ PDRAWING_SHAPE DrawingShape
        );

public:

    _Check_return_
    _Ret_opt_
    IRISPLUSPLUSAPI
    PCTEXTURE
    final
    virtual
    GetTexture(
        _In_ UINT32 FaceHit
        ) const;

    _Check_return_
    _Ret_opt_
    IRISPLUSPLUSAPI
    PCNORMAL
    final
    virtual
    GetNormal(
        _In_ UINT32 FaceHit
        ) const;
};

} // namespace Iris

#endif // _SHAPE_IRIS_PLUS_PLUS_