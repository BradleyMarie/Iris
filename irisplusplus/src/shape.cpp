/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    shape.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ shape type.

--*/

#include <irisplusplus.h>

namespace Iris {

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
ShapeTrace(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR IrisHitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    assert(Context != NULL);
    assert(IrisHitAllocator != NULL);
    assert(HitList != NULL);

    ShapeHitAllocator HitAllocator(IrisHitAllocator);

    const Shape *ShapePointer = static_cast<const Shape*>(Context);
    *HitList = ShapePointer->Trace(Ray, HitAllocator);
    return ISTATUS_SUCCESS;
}

static
VOID
ShapeFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != NULL);

    const Shape *ShapePointer = static_cast<const Shape*>(Context);
    delete ShapePointer;
}

//
// Static Variables
//

const static SHAPE_VTABLE InteropVTable = {
    ShapeTrace, ShapeFree
};

//
// Functions
//

Shape::Shape(
    void
    )
{
    Shape *ShapePointer = this;

    ISTATUS Success = ShapeAllocate(&InteropVTable,
                                    &ShapePointer,
                                    sizeof(Shape*),
                                    alignof(Shape*),
                                    &Data);

    if (Success != ISTATUS_SUCCESS)
    {
        throw std::bad_alloc();
    }
}

Shape::Shape(
    _In_ PSHAPE ShapePtr
    )
: Data(ShapePtr)
{ }

void
Shape::Reference(
    void
    )
{
    ShapeRetain(Data);
}

void 
Shape::Dereference(
    void
    )
{
    ShapeRelease(Data);
}

CShape::CShape(
    _In_ PSHAPE ShapePtr
    )
: Shape(ShapePtr), References(1)
{ }

_Ret_
PHIT_LIST
CShape::Trace(
    _In_ Ray ModelRay,
    _Inout_ ShapeHitAllocator & HitAllocator
    ) const
{
    throw std::logic_error("Impossible Path");
}

IrisPointer<Shape>
CShape::Create(
    _In_ PSHAPE ShapePtr
    )
{
    if (ShapePtr == NULL)
    {
        throw std::invalid_argument("ShapePtr");
    }

    CShape* Allocated = new CShape(ShapePtr);
    return IrisPointer<Shape>(Allocated);
}

void
CShape::Reference(
    void
    )
{
    Shape::Reference();
    References += 1;
}

void 
CShape::Dereference(
    void
    )
{
    Shape::Dereference();
    References -= 1;

    if (References == 0)
    {
        delete this;
    }
}

} // namespace Iris