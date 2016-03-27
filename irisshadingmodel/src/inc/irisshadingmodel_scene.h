/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_scene.h

Abstract:

    This file contains the internal definitions for the COLOR_SCENE type.

--*/

#ifndef _COLOR_SCENE_IRIS_SHADING_MODEL_INTERNAL_
#define _COLOR_SCENE_IRIS_SHADING_MODEL_INTERNAL_

#include <irisshadingmodelp.h>

//
// Types
//

struct _COLOR_SCENE {
    PCCOLOR_SCENE_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

#endif // _COLOR_SCENE_IRIS_SHADING_MODEL_INTERNAL_