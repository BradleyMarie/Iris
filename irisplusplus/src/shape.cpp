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
_Ret_opt_
PCTEXTURE
static
ShapeGetTexture(
    _In_opt_ PCVOID Context,
    _In_ UINT32 FaceHit
    )
{
    const Shape *ShapePointer = static_cast<const Shape*>(Context);
    return ShapePointer->GetTexture(FaceHit);
}

_Check_return_
_Ret_opt_
PCNORMAL
static
ShapeGetNormal(
    _In_opt_ PCVOID Context,
    _In_ UINT32 FaceHit
    )
{
    const Shape *ShapePointer = static_cast<const Shape*>(Context);
    return ShapePointer->GetNormal(FaceHit);
}

//
// Static Variables
//

const static DRAWING_SHAPE_VTABLE InteropVTable = {
    { NULL, NULL },
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

Shape::~Shape(
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

} // namespace Iris