/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_sceneobject.h

Abstract:

    This file contains the definitions for the SCENE_OBJECT base type.

--*/

#ifndef _SCENE_IRIS_SCENE_OBJECT_
#define _SCENE_IRIS_SCENE_OBJECT_

#include <irisshadingmodel.h>

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

IRISSHADINGMODELAPI
VOID 
SceneObjectFree(
    _In_opt_ _Post_invalid_ PSCENE_OBJECT SceneObject
    );

#endif // _SCENE_IRIS_SCENE_OBJECT_