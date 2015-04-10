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
// Types
//

typedef struct _SCENE_OBJECT {
    PDRAWING_SHAPE Shape;
    PMATRIX ModelToWorld;
    BOOL Premultiplied;
} SCENE_OBJECT, *PSCENE_OBJECT;

typedef CONST SCENE_OBJECT *PCSCENE_OBJECT;

//
// Functions
//

_Check_return_
_Ret_maybenull_
SFORCEINLINE
PSCENE_OBJECT 
SceneObjectAllocate(
    _In_ PDRAWING_SHAPE DrawingShape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    PSCENE_OBJECT SceneObject;
    PSHAPE Shape;

    ASSERT(DrawingShape != NULL);

    SceneObject = (PSCENE_OBJECT) malloc(sizeof(SCENE_OBJECT));

    if (SceneObject == NULL)
    {
        return NULL;
    }

    Shape = (PSHAPE) DrawingShape;

    SceneObject->Shape = DrawingShape;
    SceneObject->ModelToWorld = ModelToWorld;
    SceneObject->Premultiplied = Premultiplied;

    MatrixReference(ModelToWorld);
    ShapeReference(Shape);

    return SceneObject;
}

#endif // _SCENE_IRIS_SCENE_OBJECT_INTERNAL_