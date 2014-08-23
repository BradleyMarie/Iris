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
    _Inout_ PSHAPE_HIT_ALLOCATOR IrisHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    )
{
    assert(Context != NULL);
    assert(IrisHitAllocator != NULL);
    assert(ShapeHitList != NULL);

    ShapeHitAllocator HitAllocator(IrisHitAllocator);

    const Shape *ShapePointer = static_cast<const Shape*>(Context);
    *ShapeHitList = ShapePointer->Trace(Ray, HitAllocator);
    return ISTATUS_SUCCESS;
}

_Check_return_ 
_Ret_opt_
static
PCTEXTURE
ShapeGetTexture(
    _In_opt_ PCVOID Context,
    _In_ UINT32 FaceHit
    )
{
    assert(Context != NULL);

    const Shape *ShapePointer = static_cast<const Shape*>(Context);
    return ShapePointer->GetTexture(FaceHit);
}

_Check_return_
_Ret_opt_
static
PCNORMAL
ShapeGetNormal(
    _In_opt_ PCVOID Context,
    _In_ UINT32 FaceHit
    )
{
    assert(Context != NULL);

    const Shape *ShapePointer = static_cast<const Shape*>(Context);
    return ShapePointer->GetNormal(FaceHit);
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

const static DRAWING_SHAPE_VTABLE InteropVTable = {
    { NULL, ShapeFree },
    ShapeGetTexture,
    ShapeGetNormal
};

//
// Functions
//

Shape::Shape(
    void
    )
{
    PDRAWING_SHAPE DrawingShape;
    Shape *ShapePointer;

    ShapePointer = this;

    DrawingShape = DrawingShapeAllocate(&InteropVTable,
                                        &ShapePointer,
                                        sizeof(Shape*),
                                        sizeof(Shape*));

    if (DrawingShape == NULL)
    {
        throw std::bad_alloc();
    }

    Data = DrawingShape;
}

Shape::Shape(
    _In_ PDRAWING_SHAPE DrawingShape
    )
: Data(DrawingShape)
{ }

void
Shape::Reference(
    void
    )
{
    DrawingShapeReference(Data);
}

void 
Shape::Dereference(
    void
    )
{
    DrawingShapeDereference(Data);
}

CShape::CShape(
    _In_ PDRAWING_SHAPE DrawingShape
    )
: Shape(DrawingShape)
{ 
    if (DrawingShape == NULL)
    {
        throw std::invalid_argument("DrawingShape");
    }
}

_Ret_
ShapeHitList *
CShape::Trace(
    _In_ Ray ModelRay,
    _Inout_ ShapeHitAllocator & HitAllocator
    ) const
{
    throw std::logic_error("Impossible Path");
}

_Check_return_
_Ret_opt_
PCTEXTURE
CShape::GetTexture(
    _In_ UINT32 FaceHit
    ) const
{
    return DrawingShapeGetTexture(Data, FaceHit);
}

_Check_return_
_Ret_opt_
PCNORMAL
CShape::GetNormal(
    _In_ UINT32 FaceHit
    ) const
{
    return DrawingShapeGetNormal(Data, FaceHit);
}

IrisPointer<Shape>
CShape::Create(
    _In_ PDRAWING_SHAPE DrawingShape
    )
{
    CShape* Allocated = new CShape(DrawingShape);
    return IrisPointer<Shape>(Allocated);
}

} // namespace Iris