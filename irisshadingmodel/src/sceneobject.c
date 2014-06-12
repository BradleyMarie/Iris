/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    sceneobject.c

Abstract:

    This file contains the function definitions for the SCENE_OBJECT type.

--*/

#include <irisshadingmodelp.h>

VOID 
SceneObjectFree(
    _In_opt_ _Post_invalid_ PSCENE_OBJECT SceneObject
    )
{
    PGEOMETRY Geometry;

    Geometry = (PGEOMETRY) SceneObject;

    GeometryFree(Geometry);
}