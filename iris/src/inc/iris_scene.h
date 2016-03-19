/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iris_scene.h

Abstract:

    This file contains the implementation of the internal scene functions.

--*/

#ifndef _IRIS_SCENE_INTERNAL_
#define _IRIS_SCENE_INTERNAL_

#include <irisp.h>

//
// Types
//

struct _SCENE {
    PCSCENE_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
SceneTrace(
    _In_ PCSCENE Scene,
    _Inout_ PHIT_TESTER HitTester
    )
{
    ISTATUS Status;

    ASSERT(Scene != NULL);
    ASSERT(HitTester != NULL);

    Status = Scene->VTable->TraceRoutine(Scene->Data, HitTester);

    return Status;
}

#endif // _IRIS_SCENE_INTERNAL_