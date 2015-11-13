/*++

Copyright (c) 2015 Brad Weinberger

Module Name:

    irisphysx_scene.h

Abstract:

    This file contains the definitions for the SPECTRUM_SCENE type.

--*/

#ifndef _SPECTRUM_SCENE_IRIS_PHYSX_INTERNAL_
#define _SPECTRUM_SCENE_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _SPECTRUM_SCENE {
    PSCENE Scene;
    POINTER_LIST Lights;
    SIZE_T ReferenceCount;
};

#endif // _SPECTRUM_SCENE_IRIS_PHYSX_INTERNAL_