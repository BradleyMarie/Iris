/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    list_scene.c

Abstract:

    Implements a list scene.

--*/

#include <stdalign.h>

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

typedef struct _LIST_SCENE {
    POINTER_LIST world_geometry;
    POINTER_LIST premultiplied_geometry;
    POINTER_LIST transformed_geometry;
} LIST_SCENE, *PLIST_SCENE;

typedef const LIST_SCENE *PCLIST_SCENE;

//
// Static Functions
//

static
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

static
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

static
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

//
// Scene Functions
//

static
ISTATUS
ListSceneTrace(
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

        ISTATUS status = ShapeHitTesterTestWorldShape(hit_tester, shape);

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

static
void
ListSceneFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PLIST_SCENE list_scene = (PLIST_SCENE)context;

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
}

//
// Static Data
//

static const SCENE_VTABLE list_scene_vtable = {
    ListSceneTrace,
    ListSceneFree
};

//
// Public Functions
//

ISTATUS
ListSceneAllocate(
    _In_reads_(num_shapes) const PSHAPE shapes[],
    _In_reads_(num_shapes) const PMATRIX transforms[],
    _In_reads_(num_shapes) const bool premultiplied[],
    _In_ size_t num_shapes,
    _Out_ PSCENE *scene
    )
{
    if (shapes == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (transforms == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (premultiplied == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    LIST_SCENE result;

    if (!PointerListInitialize(&result.world_geometry))
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (!PointerListInitialize(&result.premultiplied_geometry))
    {
        PointerListDestroy(&result.world_geometry);
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (!PointerListInitialize(&result.transformed_geometry))
    {
        PointerListDestroy(&result.premultiplied_geometry);
        PointerListDestroy(&result.world_geometry);
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < num_shapes; i++)
    {
        ISTATUS status;
        if (premultiplied[i])
        {
            status = ListSceneAddPremultipliedShape(&result,
                                                    shapes[i],
                                                    transforms[i]);
        }
        else
        {
            status = ListSceneAddTransformedShape(&result,
                                                  shapes[i],
                                                  transforms[i]);
        }

        if (status != ISTATUS_SUCCESS)
        {
            ListSceneFree(&result);
            return status;
        }
    }

    PLIGHT background_light;
    ISTATUS status = SceneAllocate(&list_scene_vtable,
                                   &result,
                                   sizeof(LIST_SCENE),
                                   alignof(LIST_SCENE),
                                   NULL,
                                   scene);

    if (status != ISTATUS_SUCCESS)
    {
        ListSceneFree(&result);
    }

    return status;
}