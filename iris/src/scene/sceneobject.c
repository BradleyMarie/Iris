/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    sceneobject.c

Abstract:

    This file contains the function definitions for the SCENE_OBJECT type.

--*/

#include <irisp.h>

//
// Types
//

typedef struct _SCENE_OBJECT SCENE_OBJECT, *PSCENE_OBJECT;

//
// Functions
//

_Check_return_
_Ret_maybenull_
PSCENE_OBJECT
SceneObjectAllocate(
    _In_ PSHAPE Shape,
    _In_ PINVERTIBLE_MATRIX Transformation,
    _In_ BOOL Premultiplied
    )
{
    PSCENE_OBJECT SceneObject;

    ASSERT(Shape != NULL);

    SceneObject = (PSCENE_OBJECT) malloc(sizeof(SCENE_OBJECT));

    if (SceneObject == NULL)
    {
        return NULL;
    }

    SceneObject->Shape = Shape;
    SceneObject->Transformation = Transformation;
    SceneObject->Premultiplied = Premultiplied;

    return SceneObject;
}

VOID
SceneObjectFree(
    _Pre_maybenull_ _Post_invalid_ PSCENE_OBJECT SceneObject
    )
{
    free(SceneObject);
}