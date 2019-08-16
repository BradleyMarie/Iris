/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    ply_reader.c

Abstract:

    Reads a PLY file.

--*/

#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/safe_math.h"
#include "iris_physx_toolkit/ply_reader.h"
#include "third_party/rply/rply.h"
#include "third_party/rply/rplyfile.h"

//
// Defines
//

#define RPLY_FAILURE 0
#define RPLY_SUCCESS 1

//
// Static Functions
//

static
bool
FitsSizeT(
    _In_ long value
    )
{
    if (value < 0)
    {
        return false;
    }

#if SIZE_MAX < LONG_MAX
    if ((long)SIZE_MAX < value)
    {
        return false;
    }
#endif

    return true;
}

static
bool
AsSizeT(
    _In_ long value,
    _Out_ size_t *output
    )
{
    if (!FitsSizeT(value))
    {
        return false;
    }

    *output = (size_t)value;

    return true;
}

static
int
PlyVertexCallback(
    _Inout_ p_ply_argument argument
    )
{
    return RPLY_SUCCESS;
}

static
int
PlyNormalCallback(
    _Inout_ p_ply_argument argument
    )
{
    return RPLY_SUCCESS;
}

static
int
PlyUvCallback(
    _Inout_ p_ply_argument argument
    )
{
    return RPLY_SUCCESS;
}

static
int
PlyVertexIndiciesCallback(
    _Inout_ p_ply_argument argument
    )
{
    return RPLY_SUCCESS;
}

static
bool
InitializeUVCallbacks(
    _Inout_ p_ply ply,
    _Inout_ PPLY_DATA ply_data,
    _In_ const char *u_name,
    _In_ const char *v_name,
    _In_ size_t num_vertices,
    _Inout_ bool *found
    )
{
    long num_us = ply_set_read_cb(ply,
                                  "vertex",
                                  u_name,
                                  PlyUvCallback,
                                  ply_data,
                                  0);

    long num_vs = ply_set_read_cb(ply,
                                  "vertex",
                                  v_name,
                                  PlyUvCallback,
                                  ply_data,
                                  1);
    if (num_us != num_vs)
    {
        return false;
    }

    if (*found && (num_us != 0 || num_vs != 0))
    {
        return false;
    }
    else
    {
        if (!FitsSizeT(num_us) || !FitsSizeT(num_vs))
        {
            return false;
        }

        if (num_us != 0)
        {
            if (num_us != (long)num_vertices)
            {
                return false;
            }

            *found = true;
        }
    }

    return true;
}

//
// Functions
//

ISTATUS
ReadFromPlyFile(
    _In_ const char* filename,
    _Out_ PPLY_DATA *ply_data
    )
{
    if (filename == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (ply_data == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        return ISTATUS_IO_ERROR;
    }

    p_ply ply = ply_open_from_file(file, NULL, 0, NULL);

    if (ply == NULL)
    {
        fclose(file);
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (ply_read_header(ply) == 0)
    {
        ply_close(ply);
        fclose(file);
        return ISTATUS_IO_ERROR;
    }

    size_t num_faces = 0;
    size_t num_vertices = 0;

    for (p_ply_element element = ply_get_next_element(ply, NULL);
         element != NULL;
         element = ply_get_next_element(ply, element))
    {
        const char *element_name;
        long num_instances;

        ply_get_element_info(element, &element_name, &num_instances);

        if (strcmp(element_name, "face") == 0)
        {
            if (!AsSizeT(num_instances, &num_faces))
            {
                ply_close(ply);
                fclose(file);
                return ISTATUS_INVALID_ARGUMENT_00;
            }
        }
        else if (strcmp(element_name, "vertex") == 0)
        {
            if (!AsSizeT(num_instances, &num_vertices))
            {
                ply_close(ply);
                fclose(file);
                return ISTATUS_INVALID_ARGUMENT_00;
            }
        }
    }

    PPLY_DATA context = (PPLY_DATA)calloc(1, sizeof(PLY_DATA));

    if (context == NULL)
    {
        ply_close(ply);
        fclose(file);
        return ISTATUS_ALLOCATION_FAILED;
    }

    //
    // Vertices
    //

    if (ply_set_read_cb(ply, "vertex", "x", PlyVertexCallback, context, 0) != (long)num_vertices ||
        ply_set_read_cb(ply, "vertex", "y", PlyVertexCallback, context, 1) != (long)num_vertices ||
        ply_set_read_cb(ply, "vertex", "z", PlyVertexCallback, context, 2) != (long)num_vertices)
    {
        FreePlyData(context);
        ply_close(ply);
        fclose(file);
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (num_vertices == 0)
    {
        context->vertices = NULL;
    }
    else
    {
        context->vertices = (POINT3*)calloc(num_vertices, sizeof(POINT3));

        if (context->vertices == NULL)
        {
            FreePlyData(context);
            ply_close(ply);
            fclose(file);
            return ISTATUS_ALLOCATION_FAILED;
        }
    }

    //
    // Normals
    //

    long num_normals = ply_set_read_cb(ply, "vertex", "nx", PlyNormalCallback, context, 0);
    if (!FitsSizeT(num_normals))
    {
        FreePlyData(context);
        ply_close(ply);
        fclose(file);
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (num_normals != 0 && num_normals != (long)num_vertices)
    {
        FreePlyData(context);
        ply_close(ply);
        fclose(file);
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (num_normals != ply_set_read_cb(ply, "vertex", "ny", PlyNormalCallback, context, 1))
    {
        FreePlyData(context);
        ply_close(ply);
        fclose(file);
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (num_normals != ply_set_read_cb(ply, "vertex", "nz", PlyNormalCallback, context, 2))
    {
        FreePlyData(context);
        ply_close(ply);
        fclose(file);
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (num_normals == 0)
    {
        context->normals = NULL;
    }
    else
    {
        context->normals = (VECTOR3*)calloc(num_vertices, sizeof(VECTOR3));

        if (context->normals == NULL)
        {
            FreePlyData(context);
            ply_close(ply);
            fclose(file);
            return ISTATUS_ALLOCATION_FAILED;
        }
    }

    //
    // UVs
    //

    bool found = false;
    if (!InitializeUVCallbacks(ply, context, "u", "v", num_vertices, &found) ||
        !InitializeUVCallbacks(ply, context, "s", "t", num_vertices, &found) ||
        !InitializeUVCallbacks(ply, context, "texture_u", "texture_v", num_vertices, &found) ||
        !InitializeUVCallbacks(ply, context, "texture_s", "texture_t", num_vertices, &found))
    {
        FreePlyData(context);
        ply_close(ply);
        fclose(file);
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (!found)
    {
        context->uvs = NULL;
    }
    else
    {
        size_t num_elements;
        bool success = CheckedMultiplySizeT(num_vertices, 2, &num_elements);

        if (!success)
        {
            FreePlyData(context);
            ply_close(ply);
            fclose(file);
            return ISTATUS_ALLOCATION_FAILED;
        }

        context->uvs = (float_t*)calloc(num_elements, sizeof(float_t));

        if (context->uvs == NULL)
        {
            FreePlyData(context);
            ply_close(ply);
            fclose(file);
            return ISTATUS_ALLOCATION_FAILED;
        }
    }

    context->num_vertices = num_vertices;

    //
    // Vertex Indices
    //

    long vertex_indices = ply_set_read_cb(ply,
                                          "face",
                                          "vertex_indices",
                                          PlyVertexIndiciesCallback,
                                          &context,
                                          0);

    if (vertex_indices != (long)num_faces ||
        (num_vertices == 0 && num_faces != 0))
    {
        FreePlyData(context);
        ply_close(ply);
        fclose(file);
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    size_t num_elements;
    bool success = CheckedMultiplySizeT(num_faces, 6, &num_elements);

    if (!success)
    {
        FreePlyData(context);
        ply_close(ply);
        fclose(file);
        return ISTATUS_ALLOCATION_FAILED;
    }

    context->faces = (size_t*)calloc(num_elements, sizeof(size_t));

    if (context->faces == NULL)
    {
        FreePlyData(context);
        ply_close(ply);
        fclose(file);
        return ISTATUS_ALLOCATION_FAILED;
    }

    int read_status = ply_read(ply);

    ply_close(ply);
    fclose(file);

    if (read_status != RPLY_SUCCESS)
    {
        FreePlyData(context);
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    *ply_data = context;

    return ISTATUS_SUCCESS;
}

void
FreePlyData(
    _In_opt_ _Post_invalid_ PPLY_DATA ply_data
    )
{
    if (ply_data == NULL)
    {
        return;
    }

    free(ply_data->vertices);
    free(ply_data->normals);
    free(ply_data->uvs);
    free(ply_data->faces);
    free(ply_data);
}