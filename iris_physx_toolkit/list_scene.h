/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    list_scene.h

Abstract:

    Creates scene which tests a ray against all of the geometry it contains.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_LIST_SCENE_
#define _IRIS_PHYSX_TOOLKIT_LIST_SCENE_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef struct _LIST_SCENE LIST_SCENE, *PLIST_SCENE;
typedef const LIST_SCENE *PCLIST_SCENE;

//
// Functions
//

ISTATUS
ListSceneAllocate(
    _Out_ PLIST_SCENE *list_scene
    );

ISTATUS
ListSceneAddShape(
    _Inout_ PLIST_SCENE list_scene,
    _In_ PSHAPE shape
    );

ISTATUS
ListSceneAddPremultipliedShape(
    _Inout_ PLIST_SCENE list_scene,
    _In_ PSHAPE shape,
    _In_opt_ PMATRIX model_to_world
    );

ISTATUS
ListSceneAddTransformedShape(
    _Inout_ PLIST_SCENE list_scene,
    _In_ PSHAPE shape,
    _In_opt_ PMATRIX model_to_world
    );

void
ListSceneFree(
    _In_opt_ _Post_invalid_ PLIST_SCENE list_scene
    );

//
// Callback Functions
//

ISTATUS 
ListSceneTraceCallback(
    _In_opt_ const void *context, 
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_LIST_SCENE_