/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    triangle.c

Abstract:

    Implements a triangle.

--*/

#include "iris_physx_toolkit/shapes/triangle.h"
#include "iris_physx_toolkit/shapes/triangle_mesh.h"

//
// Functions
//

ISTATUS
TriangleAllocate(
    _In_ POINT3 v0,
    _In_ POINT3 v1,
    _In_ POINT3 v2,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _Out_ PSHAPE *shape
    )
{
    if (!PointValidate(v0))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!PointValidate(v1))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!PointValidate(v2))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    POINT3 vertices[] = { v0, v1, v2 };
    size_t vertex_indices[][3] = { { 0, 1, 2 } };

    size_t triangles_allocated;
    ISTATUS status = TriangleMeshAllocate(vertices,
                                          3,
                                          vertex_indices,
                                          1,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          front_material,
                                          back_material,
                                          NULL,
                                          NULL,
                                          shape,
                                          &triangles_allocated);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (triangles_allocated != 1)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
EmissiveTriangleAllocate(
    _In_ POINT3 v0,
    _In_ POINT3 v1,
    _In_ POINT3 v2,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _In_opt_ PEMISSIVE_MATERIAL front_emissive_material,
    _In_opt_ PEMISSIVE_MATERIAL back_emissive_material,
    _Out_ PSHAPE *shape
    )
{
    if (!PointValidate(v0))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!PointValidate(v1))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!PointValidate(v2))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    POINT3 vertices[] = { v0, v1, v2 };
    size_t vertex_indices[][3] = { { 0, 1, 2 } };

    size_t triangles_allocated;
    ISTATUS status = TriangleMeshAllocate(vertices,
                                          3,
                                          vertex_indices,
                                          1,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          front_material,
                                          back_material,
                                          front_emissive_material,
                                          back_emissive_material,
                                          shape,
                                          &triangles_allocated);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (triangles_allocated != 1)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    return ISTATUS_SUCCESS;
}