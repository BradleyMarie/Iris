/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_sceneobject.h

Abstract:

    This file contains the definitions for the SCENE_OBJECT type.

--*/

#ifndef _SCENE_OBJECT_IRIS_
#define _SCENE_OBJECT_IRIS_

#include <iris.h>

//
// Types
//

typedef struct _SCENE_OBJECT SCENE_OBJECT, *PSCENE_OBJECT;
typedef CONST SCENE_OBJECT *PCSCENE_OBJECT;

//
// Functions
//

_Check_return_
_Ret_maybenull_
PSCENE_OBJECT
SceneObjectAllocate(
    _In_ PCSHAPE Shape,
    _In_ PCINVERTIBLE_MATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

VOID
SceneObjectFree(
    _Pre_maybenull_ _Post_invalid_ PSCENE_OBJECT SceneObject
    );

#endif // _SCENE_OBJECT_IRIS_