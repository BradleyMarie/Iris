/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_listscene.h

Abstract:

    This file contains the definitions for the LIST_SCENE type.

--*/

#include <iristoolkitp.h>

//
// Constants
//

#define LIST_SCENE_INTIAL_SIZE  10

//
// Types
//

typedef struct _LIST_SCENE {
    _Field_size_(ObjectsCapacity) PSCENE_OBJECT *Objects;
    SIZE_T ObjectsCapacity;
    SIZE_T ObjectsSize;
} LIST_SCENE, *PLIST_SCENE;

typedef CONST LIST_SCENE *PCLIST_SCENE;

//
// Static functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
ListSceneAddObject(
    _Inout_ PVOID Context,
    _In_ PSCENE_OBJECT SceneObject
    )
{
    PSCENE_OBJECT *Objects;
    PLIST_SCENE ListScene;
    SIZE_T NewCapacity;
    SIZE_T ListSize;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(SceneObject != NULL);

    ListScene = (PLIST_SCENE) Context;

    ListSize = ListScene->ObjectsSize;

    if (ListScene->ObjectsCapacity == ListSize)
    {
        //
        // List grows each time by a factor of 2
        //

        Status = CheckedAddSizeT(ListSize, ListSize, &NewCapacity);

        if (Status != ISTATUS_SUCCESS)
        {
            if (ListSize == SIZE_MAX)
            {
                return ISTATUS_ALLOCATION_FAILED;
            }

            NewCapacity = SIZE_MAX;
        }

        Objects = (PSCENE_OBJECT*) realloc(ListScene->Objects,
                                           NewCapacity * sizeof(PSCENE_OBJECT));

        if (Objects == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        ListScene->Objects = Objects;
        ListScene->ObjectsCapacity = NewCapacity;
    }
    else
    {
        Objects = ListScene->Objects;
    }

    Objects[ListSize] = SceneObject;
    ListScene->ObjectsSize = ListSize + 1;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS 
ListSceneTrace(
    _In_ PCVOID Context,
    _Inout_ PHIT_TESTER HitTester
    )
{
    PSCENE_OBJECT *Objects;
    PLIST_SCENE ListScene;
    SIZE_T ListSize;
    ISTATUS Status;
    SIZE_T Index;

    ASSERT(Context != NULL);
    ASSERT(HitTester != NULL);

    ListScene = (PLIST_SCENE) Context;

    ListSize = ListScene->ObjectsSize;
    Objects = ListScene->Objects;

    for (Index = 0; Index < ListSize; Index++)
    {
        Status = HitTesterTestShapeWithTransform(HitTester,
                                                 (PSHAPE) Objects[Index]->Shape,
                                                 Objects[Index]->ModelToWorld,
                                                 Objects[Index]->Premultiplied);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
    }

    return ISTATUS_SUCCESS;
}

STATIC
VOID
ListSceneFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PSCENE_OBJECT *Objects;
    PLIST_SCENE ListScene;
    SIZE_T ListSize;
    SIZE_T Index;

    ASSERT(Context != NULL);

    ListScene = (PLIST_SCENE) Context;

    ListSize = ListScene->ObjectsSize;
    Objects = ListScene->Objects;
    
    for (Index = 0; Index < ListSize; Index++)
    {
        SceneObjectFree(Objects[Index]);
    }

    free(ListScene->Objects);
}

//
// Static variables
//

STATIC COLOR_SCENE_VTABLE ListSceneVTable = {
    ListSceneTrace,
    ListSceneFree,
    ListSceneAddObject
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
PCOLOR_SCENE
ListSceneAllocate(
    VOID
    )
{
    PSCENE_OBJECT *Objects;
    LIST_SCENE ListScene;
    PCOLOR_SCENE Scene;

    Objects = (PSCENE_OBJECT*) malloc(sizeof(PSCENE_OBJECT) * LIST_SCENE_INTIAL_SIZE);

    if (Objects == NULL)
    {
        return NULL;
    }

    ListScene.Objects = Objects;
    ListScene.ObjectsSize = 0;
    ListScene.ObjectsCapacity = LIST_SCENE_INTIAL_SIZE;
    
    Scene = ColorSceneAllocate(&ListSceneVTable,
                               &ListScene,
                               sizeof(LIST_SCENE),
                               sizeof(PVOID));

    if (Scene == NULL)
    {
        free(Objects);
    }

    return Scene;
}