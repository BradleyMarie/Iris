/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    scene.c

Abstract:

    A structured representation of the scenes in a scene.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/scene.h"
#include "iris_physx/scene_internal.h"

//
// Functions
//

ISTATUS
SceneAllocate(
    _In_ PCSCENE_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PSCENE *scene
    )
{
    if (vtable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (data_size != 0)
    {
        if (data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
        
        if (data_alignment == 0 ||
            (data_alignment & (data_alignment - 1)) != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;    
        }
        
        if (data_size % data_alignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(SCENE),
                                          alignof(SCENE),
                                          (void **)scene,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*scene)->vtable = vtable;
    (*scene)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
SceneFree(
    _In_opt_ _Post_invalid_ PSCENE scene
    )
{
    if (scene == NULL)
    {
        return;
    }

    if (scene->vtable->free_routine != NULL)
    {
        scene->vtable->free_routine(scene->data);
    }

    free(scene);
}