/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    list_scene.c

Abstract:

    Implements a list scene.

--*/

#include "common/pointer_list.h"
#include "iris_physx_toolkit/list_scene.h"

//
// Types
//

typedef struct _SHAPE_AND_TRANSFORM {
    PSHAPE shape;
    PMATRIX model_to_world;
} SHAPE_AND_TRANSFORM, *PSHAPE_AND_TRANSFORM;

typedef const SHAPE_AND_TRANSFORM *PCSHAPE_AND_TRANSFORM;

struct _LIST_SCENE {
    POINTER_LIST world_geometry;
    POINTER_LIST premultiplied_geometry;
    POINTER_LIST transformed_geometry;
};

//
// Functions
//

ISTATUS
ListSceneAllocate(
    _Out_ PLIST_SCENE *list_scene
    )
{
    if (list_scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    PLIST_SCENE result = (PLIST_SCENE)malloc(sizeof(LIST_SCENE));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (!PointerListInitialize(&result->world_geometry))
    {
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (!PointerListInitialize(&result->premultiplied_geometry))
    {
        PointerListDestroy(&result->world_geometry);
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (!PointerListInitialize(&result->transformed_geometry))
    {
        PointerListDestroy(&result->premultiplied_geometry);
        PointerListDestroy(&result->world_geometry);
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    *list_scene = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
ListSceneAddShape(
    _Inout_ PLIST_SCENE list_scene,
    _In_ PSHAPE shape
    )
{
    if (list_scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!PointerListAddPointer(&list_scene->world_geometry, shape))
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ShapeRetain(shape);

    return ISTATUS_SUCCESS;
}

ISTATUS
ListSceneAddPremultipliedShape(
    _Inout_ PLIST_SCENE list_scene,
    _In_ PSHAPE shape,
    _In_opt_ PMATRIX model_to_world
    )
{
    if (model_to_world == NULL)
    {
        return ListSceneAddShape(list_scene, shape);
    }

    if (list_scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PSHAPE_AND_TRANSFORM entry = 
        (PSHAPE_AND_TRANSFORM)malloc(sizeof(SHAPE_AND_TRANSFORM));

    if (entry == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    entry->shape = shape;
    entry->model_to_world = model_to_world;

    if (!PointerListAddPointer(&list_scene->premultiplied_geometry, entry))
    {
        free(entry);
        return ISTATUS_ALLOCATION_FAILED;
    }

    ShapeRetain(shape);
    MatrixRetain(model_to_world);

    return ISTATUS_SUCCESS;
}

ISTATUS
ListSceneAddTransformedShape(
    _Inout_ PLIST_SCENE list_scene,
    _In_ PSHAPE shape,
    _In_opt_ PMATRIX model_to_world
    )
{
    if (model_to_world == NULL)
    {
        return ListSceneAddShape(list_scene, shape);
    }

    if (list_scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    PSHAPE_AND_TRANSFORM entry = 
        (PSHAPE_AND_TRANSFORM)malloc(sizeof(SHAPE_AND_TRANSFORM));

    if (entry == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    entry->shape = shape;
    entry->model_to_world = model_to_world;

    if (!PointerListAddPointer(&list_scene->transformed_geometry, entry))
    {
        free(entry);
        return ISTATUS_ALLOCATION_FAILED;
    }

    ShapeRetain(shape);
    MatrixRetain(model_to_world);

    return ISTATUS_SUCCESS;
}

void
ListSceneFree(
    _In_opt_ _Post_invalid_ PLIST_SCENE list_scene
    )
{
    if (list_scene == NULL)
    {
        return;
    }

    size_t list_size = PointerListGetSize(&list_scene->world_geometry);
    for (size_t i = 0; i < list_size; i++)
    {
        PSHAPE shape = 
            (PSHAPE)PointerListRetrieveAtIndex(&list_scene->world_geometry, i);
        
        ShapeRelease(shape);
    }

    PointerListDestroy(&list_scene->world_geometry);

    list_size = PointerListGetSize(&list_scene->premultiplied_geometry);
    for (size_t i = 0; i < list_size; i++)
    {
        void* raw_entry = 
            PointerListRetrieveAtIndex(&list_scene->premultiplied_geometry, i);
        PSHAPE_AND_TRANSFORM entry = (PSHAPE_AND_TRANSFORM)raw_entry;

        ShapeRelease(entry->shape);
        MatrixRelease(entry->model_to_world);
        free(entry);
    }

    PointerListDestroy(&list_scene->premultiplied_geometry);

    list_size = PointerListGetSize(&list_scene->transformed_geometry);
    for (size_t i = 0; i < list_size; i++)
    {
        void* raw_entry = 
            PointerListRetrieveAtIndex(&list_scene->transformed_geometry, i);
        PSHAPE_AND_TRANSFORM entry = (PSHAPE_AND_TRANSFORM)raw_entry;

        ShapeRelease(entry->shape);
        MatrixRelease(entry->model_to_world);
        free(entry);
    }

    PointerListDestroy(&list_scene->transformed_geometry);

    free(list_scene);
}

//
// Callback Functions
//

ISTATUS 
ListSceneTraceCallback(
    _In_opt_ const void *context, 
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    )
{
    assert(context != NULL);
    assert(hit_tester != NULL);
    assert(RayValidate(ray));

    PCLIST_SCENE list_scene = (PCLIST_SCENE)context;

    size_t list_size = PointerListGetSize(&list_scene->world_geometry);
    for (size_t i = 0; i < list_size; i++)
    {
        PCSHAPE shape = 
            (PCSHAPE)PointerListRetrieveAtIndex(&list_scene->world_geometry, i);

        ISTATUS status = ShapeHitTesterTestShape(hit_tester, shape);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    list_size = PointerListGetSize(&list_scene->premultiplied_geometry);
    for (size_t i = 0; i < list_size; i++)
    {
        const void* raw_entry = 
            PointerListRetrieveAtIndex(&list_scene->premultiplied_geometry, i);
        PCSHAPE_AND_TRANSFORM entry = (PCSHAPE_AND_TRANSFORM)raw_entry;

        ISTATUS status = 
            ShapeHitTesterTestPremultipliedShape(hit_tester,
                                                 entry->shape,
                                                 entry->model_to_world);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    list_size = PointerListGetSize(&list_scene->transformed_geometry);
    for (size_t i = 0; i < list_size; i++)
    {
        const void* raw_entry = 
            PointerListRetrieveAtIndex(&list_scene->transformed_geometry, i);
        PCSHAPE_AND_TRANSFORM entry = (PCSHAPE_AND_TRANSFORM)raw_entry;

        ISTATUS status =
            ShapeHitTesterTestTransformedShape(hit_tester,
                                               entry->shape,
                                               entry->model_to_world);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    return ISTATUS_SUCCESS;
}