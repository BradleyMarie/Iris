/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_listscene.h

Abstract:

    This file contains the prototypes for the LIST_SCENE type.

--*/

#ifndef _LIST_SCENE_IRIS_TOOLKIT_
#define _LIST_SCENE_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PSCENE
ListSceneAllocate(
    VOID
    );

#endif // _LIST_SCENE_IRIS_TOOLKIT_