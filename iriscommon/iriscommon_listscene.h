/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iriscommon_listscene.h

Abstract:

    This file contains the definitions for the LIST_SCENE type.

--*/

#ifndef _IRIS_COMMON_LIST_SCENE_
#define _IRIS_COMMON_LIST_SCENE_

#include <iris.h>

//
// Constants
//

#define LIST_SCENE_INTIAL_SIZE  10

//
// Types
//

typedef struct _LIST_SCENE_OBJECT {
    PSHAPE Shape;
    PMATRIX ModelToWorld;
    BOOL Premultiplied;
} LIST_SCENE_OBJECT, *PLIST_SCENE_OBJECT;

typedef struct _LIST_SCENE {
    _Field_size_(ObjectsCapacity) PLIST_SCENE_OBJECT Objects;
    SIZE_T ObjectsCapacity;
    SIZE_T ObjectsSize;
} LIST_SCENE, *PLIST_SCENE;

typedef CONST LIST_SCENE *PCLIST_SCENE;

//
// Definitions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
ListSceneInitialize(
    _Out_ PLIST_SCENE Output
    )
{
    PLIST_SCENE_OBJECT Objects;

    ASSERT(Output != NULL);

    Objects = (PLIST_SCENE_OBJECT) malloc(sizeof(LIST_SCENE_OBJECT) * LIST_SCENE_INTIAL_SIZE);

    if (Objects == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Output->Objects = Objects;
    Output->ObjectsSize = 0;
    Output->ObjectsCapacity = LIST_SCENE_INTIAL_SIZE;

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
ListSceneDestroy(
    _Inout_ PLIST_SCENE ListScene
    )
{
    PLIST_SCENE_OBJECT Objects;
    SIZE_T ListSize;
    SIZE_T Index;

    ASSERT(ListScene != NULL);

    ListSize = ListScene->ObjectsSize;
    Objects = ListScene->Objects;
    
    for (Index = 0; Index < ListSize; Index++)
    {
        ShapeDereference(Objects[Index].Shape);
        MatrixRelease(Objects[Index].ModelToWorld);
    }

    free(Objects);

    ListScene->Objects = NULL;
    ListScene->ObjectsSize = 0;
    ListScene->ObjectsCapacity = 0;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
ListSceneAddObject(
    _Inout_ PLIST_SCENE ListScene,
    _In_ PSHAPE Shape,
    _In_opt_ PMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    PLIST_SCENE_OBJECT Objects;
    SIZE_T NewCapacity;
    SIZE_T ListSize;
    ISTATUS Status;

    ASSERT(ListScene != NULL);
    ASSERT(Shape != NULL);

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

        Objects = (PLIST_SCENE_OBJECT) realloc(ListScene->Objects,
                                               NewCapacity * sizeof(LIST_SCENE_OBJECT));

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

    ListScene->ObjectsSize = ListSize + 1;

    Objects[ListSize].Shape = Shape;
    Objects[ListSize].ModelToWorld = ModelToWorld;
    Objects[ListSize].Premultiplied = Premultiplied;

    MatrixRetain(ModelToWorld);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS 
ListSceneTrace(
    _Inout_ PCLIST_SCENE ListScene,
    _Inout_ PRAYTRACER RayTracer
    )
{
    PLIST_SCENE_OBJECT Objects;
    SIZE_T ListSize;
    ISTATUS Status;
    SIZE_T Index;

    ASSERT(ListScene != NULL);
    ASSERT(RayTracer != NULL);

    ListSize = ListScene->ObjectsSize;
    Objects = ListScene->Objects;

    for (Index = 0; Index < ListSize; Index++)
    {
        Status = RayTracerTraceShapeWithTransform(RayTracer, 
                                                  Objects[Index].Shape,
                                                  Objects[Index].ModelToWorld,
                                                  Objects[Index].Premultiplied);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
    }

    return ISTATUS_SUCCESS;
}

#undef LIST_SCENE_INTIAL_SIZE

#endif // _IRIS_COMMON_LIST_SCENE_