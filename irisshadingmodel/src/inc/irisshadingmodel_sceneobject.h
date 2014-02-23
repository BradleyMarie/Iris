/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_sceneobject.h

Abstract:

    This file contains the internal definitions for the SCENE_OBJECT base type.

--*/

#ifndef _SCENE_IRIS_SCENE_OBJECT_INTERNAL_
#define _SCENE_IRIS_SCENE_OBJECT_INTERNAL_

#include <irisshadingmodelp.h>

//
// Functions
//

_Check_return_
_Ret_maybenull_
SFORCEINLINE
PSCENE_OBJECT 
SceneObjectAllocate(
    _In_ PCDRAWING_SHAPE DrawingShape,
    _In_ PCINVERTIBLE_MATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    PGEOMETRY Geometry;
    PSHAPE Shape;

    ASSERT(DrawingShape != NULL);
    ASSERT(ModelToWorld != NULL);

    Shape = (PSHAPE) DrawingShape;

    Geometry = GeometryAllocate(Shape,
                                ModelToWorld,
                                Premultiplied);

    return (PSCENE_OBJECT) Geometry;
}

#endif // _SCENE_IRIS_SCENE_OBJECT_INTERNAL_