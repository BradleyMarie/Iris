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

    _Ret_
    virtual
    PHIT_LIST
    Trace(
        _In_ Ray ModelRay,
        _Inout_ ShapeHitAllocator & HitAllocator
        ) const = 0;

    virtual
    ~Shape(
        void
        )
    { }

private:
    PSHAPE Data;

    IRISPLUSPLUSAPI
    Shape(
        _In_ PSHAPE ShapePtr
        );

    IRISPLUSPLUSAPI
    virtual
    void
    Reference(
        void
        );

    IRISPLUSPLUSAPI
    virtual
    void 
    Dereference(
        void
        );

    friend class IrisPointer<Shape>;
    friend class CShape;
};

class CShape final : public Shape {
public:
    IRISPLUSPLUSAPI
    static
    IrisPointer<Shape>
    Create(
        _In_ PSHAPE ShapePtr
        );

    _Ret_
    IRISPLUSPLUSAPI
    virtual
    PHIT_LIST
    Trace(
        _In_ Ray ModelRay,
        _Inout_ ShapeHitAllocator & HitAllocator
        ) const;

    IRISPLUSPLUSAPI
    virtual
    void
    Reference(
        void
        );

    IRISPLUSPLUSAPI
    virtual
    void 
    Dereference(
        void
        );

private:
    CShape(
        _In_ PSHAPE ShapePtr
        );

    std::atomic_size_t References;
};

} // namespace Iris

#endif // _SHAPE_IRIS_PLUS_PLUS_