/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    triangle_mesh.c

Abstract:

    Implements a triangle mesh.

--*/

#include <stdlib.h>
#include <string.h>

#include "iris_physx_toolkit/triangle.h"

//
// Static Functions
//

static
ISTATUS
TriangleMeshValidateVertices(
    _In_reads_(num_vertices) const POINT3 vertices[],
    _In_ size_t num_vertices,
    _In_reads_(num_triangles) size_t vertex_indices[][3],
    _In_ size_t num_triangles
    )
{
    if (vertices == NULL && num_vertices != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (vertex_indices == NULL && num_triangles != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    for (size_t i = 0; i < num_triangles; i++)
    {
        if (vertex_indices[i][0] < num_vertices)
        {
            return ISTATUS_INVALID_ARGUMENT_02;
        }

        if (!PointValidate(vertices[vertex_indices[i][0]]))
        {
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        if (vertex_indices[i][1] < num_vertices)
        {
            return ISTATUS_INVALID_ARGUMENT_02;
        }
        
        if(!PointValidate(vertices[vertex_indices[i][1]]))
        {
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        if (vertex_indices[i][2] < num_vertices)
        {
            return ISTATUS_INVALID_ARGUMENT_02;
        }

        if (!PointValidate(vertices[vertex_indices[i][2]]))
        {
            return ISTATUS_INVALID_ARGUMENT_02;
        }

        if (vertex_indices[i][0] == vertex_indices[i][1] ||
            vertex_indices[i][0] == vertex_indices[i][2] ||
            vertex_indices[i][1] == vertex_indices[i][2])
        {
            return ISTATUS_INVALID_ARGUMENT_02;
        }
    }

    return ISTATUS_SUCCESS;
}

static
void
TriangleMeshFree(
    _Inout_updates_to_(num_to_free, 0) PSHAPE shapes[],
    _In_ size_t num_to_free
    )
{
    for (size_t i = 0; i < num_to_free; i++)
    {
        ShapeRelease(shapes[i]);
    }
}

//
// Functions
//

ISTATUS
TriangleMeshAllocate(
    _In_reads_(num_vertices) const POINT3 vertices[],
    _In_ size_t num_vertices,
    _In_reads_(num_triangles) size_t vertex_indices[][3],
    _In_ size_t num_triangles,
    _In_reads_(num_triangles) PMATERIAL front_materials[],
    _In_reads_(num_triangles) PMATERIAL back_materials[],
    _Out_writes_(num_triangles) PSHAPE shapes[]
    )
{
    ISTATUS status = TriangleMeshValidateVertices(vertices,
                                                  num_vertices,
                                                  vertex_indices,
                                                  num_triangles);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (num_triangles != 0)
    {
        if (front_materials == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_04;
        }

        if (back_materials == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_05;
        }

        if (shapes == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_06;
        }
    }

    PSHAPE *shapes_tmp = (PSHAPE*)calloc(num_triangles, sizeof(PSHAPE));

    if (shapes_tmp == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < num_triangles; i++)
    {
        ISTATUS status = TriangleAllocate(vertices[vertex_indices[i][0]],
                                          vertices[vertex_indices[i][1]],
                                          vertices[vertex_indices[i][2]],
                                          front_materials[i],
                                          back_materials[i],
                                          shapes_tmp + i);

        if (status != ISTATUS_SUCCESS)
        {
            assert(status == ISTATUS_ALLOCATION_FAILED ||
                   status == ISTATUS_INVALID_ARGUMENT_COMBINATION_00);
            TriangleMeshFree(shapes_tmp, i);
            free(shapes_tmp);
            return status;
        }
    }

    memcpy(shapes, shapes_tmp, sizeof(PSHAPE) * num_triangles);
    free(shapes_tmp);

    return ISTATUS_SUCCESS;
}

ISTATUS
EmissiveTriangleMeshAllocate(
    _In_reads_(num_vertices) const POINT3 vertices[],
    _In_ size_t num_vertices,
    _In_reads_(num_triangles) size_t vertex_indices[][3],
    _In_ size_t num_triangles,
    _In_reads_(num_triangles) PMATERIAL front_materials[],
    _In_reads_(num_triangles) PMATERIAL back_materials[],
    _In_reads_(num_triangles) PEMISSIVE_MATERIAL front_emissive_materials[],
    _In_reads_(num_triangles) PEMISSIVE_MATERIAL back_emissive_materials[],
    _Out_writes_(num_triangles) PSHAPE shapes[]
    )
{
    ISTATUS status = TriangleMeshValidateVertices(vertices,
                                                  num_vertices,
                                                  vertex_indices,
                                                  num_triangles);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (num_triangles != 0)
    {
        if (front_materials == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_04;
        }

        if (back_materials == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_05;
        }

        if (front_emissive_materials == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_06;
        }

        if (back_emissive_materials == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_07;
        }

        if (shapes == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_08;
        }
    }

    PSHAPE *shapes_tmp = (PSHAPE*)calloc(num_triangles, sizeof(PSHAPE));

    if (shapes_tmp == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < num_triangles; i++)
    {
        ISTATUS status = 
            EmissiveTriangleAllocate(vertices[vertex_indices[i][0]],
                                     vertices[vertex_indices[i][1]],
                                     vertices[vertex_indices[i][2]],
                                     front_materials[i],
                                     back_materials[i],
                                     front_emissive_materials[i],
                                     back_emissive_materials[i],
                                     shapes_tmp + i);

        if (status != ISTATUS_SUCCESS)
        {
            assert(status == ISTATUS_ALLOCATION_FAILED ||
                   status == ISTATUS_INVALID_ARGUMENT_COMBINATION_00);
            TriangleMeshFree(shapes_tmp, i);
            free(shapes_tmp);
            return status;
        }
    }

    memcpy(shapes, shapes_tmp, sizeof(PSHAPE) * num_triangles);
    free(shapes_tmp);

    return ISTATUS_SUCCESS;
}