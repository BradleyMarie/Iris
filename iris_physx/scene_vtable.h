/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    scene_vtable.h

Abstract:

    The vtable for a scene.

--*/

#ifndef _IRIS_PHYSX_SCENE_VTABLE_
#define _IRIS_PHYSX_SCENE_VTABLE_

#include "iris_physx/hit_tester.h"
#include "iris_physx/shape.h"

//
// Types
//

typedef
ISTATUS 
(*PSCENE_TRACE_ROUTINE)(
    _In_opt_ const void *context,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    );

typedef struct _SCENE_VTABLE {
    PSCENE_TRACE_ROUTINE trace_routine;
    PFREE_ROUTINE free_routine;
} SCENE_VTABLE, *PSCENE_VTABLE;

typedef const SCENE_VTABLE *PCSCENE_VTABLE;

#endif // _IRIS_PHYSX_SCENE_VTABLE_