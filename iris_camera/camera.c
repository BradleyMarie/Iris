/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    camera.c

Abstract:

    Generates the ray to be traced from UV coordinates on the framebuffer and
    lens. The generated ray is not guaranteed to be normalized.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_camera/camera_internal.h"
#include "iris_camera/camera.h"

//
// Functions
//

ISTATUS
CameraAllocate(
    _In_ PCCAMERA_VTABLE vtable,
    _In_ float_t pixel_min_u,
    _In_ float_t pixel_max_u,
    _In_ float_t pixel_min_v,
    _In_ float_t pixel_max_v,
    _In_ float_t lens_min_u,
    _In_ float_t lens_max_u,
    _In_ float_t lens_min_v,
    _In_ float_t lens_max_v,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCAMERA *camera
    )
{
    if (vtable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(pixel_min_u))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(pixel_max_u))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (!isfinite(pixel_min_v))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(pixel_max_v))
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (!isfinite(lens_min_u))
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (!isfinite(lens_max_u))
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (!isfinite(lens_min_v))
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (!isfinite(lens_max_v))
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    if (pixel_max_u < pixel_min_u)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    if (pixel_max_v < pixel_min_v)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
    }

    if (lens_max_u < lens_min_u)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
    }

    if (lens_max_v < lens_min_v)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_03;
    }

    if (data_size != 0)
    {
        if (data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_04;
        }
        
        if (data_alignment == 0 ||
            (data_alignment & (data_alignment - 1)) != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_05;    
        }
        
        if (data_size % data_alignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_06;
        }
    }

    if (camera == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_12;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(CAMERA),
                                          alignof(CAMERA),
                                          (void **)camera,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*camera)->vtable = vtable;
    (*camera)->data = data_allocation;
    (*camera)->pixel_min_u = pixel_min_u;
    (*camera)->pixel_max_u = pixel_max_u;
    (*camera)->pixel_min_v = pixel_min_v;
    (*camera)->pixel_max_v = pixel_max_v;
    (*camera)->lens_min_u = lens_min_u;
    (*camera)->lens_max_u = lens_max_u;
    (*camera)->lens_min_v = lens_min_v;
    (*camera)->lens_max_v = lens_max_v;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
CameraFree(
    _In_opt_ _Post_invalid_ PCAMERA camera
    )
{
    if (camera == NULL)
    {
        return;
    }

    camera->vtable->free_routine(camera->data);
    free(camera);
}