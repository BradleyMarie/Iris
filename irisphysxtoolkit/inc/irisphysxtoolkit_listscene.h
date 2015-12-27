/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysxtoolkit_listscene.h

Abstract:

    This file contains the allocation functions for a list scene.

--*/

#ifndef _LIST_SCENE_IRIS_PHYSX_TOOLKIT_
#define _LIST_SCENE_IRIS_PHYSX_TOOLKIT_

#include <irisphysxtoolkit.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXTOOLKITAPI
ISTATUS
PhysxListSceneAllocate(
    _Out_ PSPECTRUM_SCENE *Scene
    );

#endif // _LIST_SCENE_IRIS_PHYSX_TOOLKIT_