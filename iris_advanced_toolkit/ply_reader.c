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
#include "iris_advanced_toolkit/ply_reader.h"
#include "third_party/rply/rply.h"
#include "third_party/rply/rplyfile.h"

//
// Defines
//

#define RPLY_FAILURE 0
#define RPLY_SUCCESS 1

#define FLAG_X 0
#define FLAG_Y 1
#define FLAG_Z 2

#define FLAG_U 0
#define FLAG_V 1

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
    void *context;
    long flags;
    ply_get_argument_user_data(argument, &context, &flags);

    long index;
    ply_get_argument_element(argument, NULL, &index);

    float_t value = ply_get_argument_value(argument);

    PPLY_DATA ply_data = (PPLY_DATA)context;
    switch (flags)
    {
        case FLAG_X:
            ply_data->vertices[index].x = value;
            break;
        case FLAG_Y:
            ply_data->vertices[index].y = value;
            break;
        case FLAG_Z:
            ply_data->vertices[index].z = value;
            break;
        default:
            return RPLY_FAILURE;
    }

    return RPLY_SUCCESS;
}

static
int
PlyNormalCallback(
    _Inout_ p_ply_argument argument
    )
{
    void *context;
    long flags;
    ply_get_argument_user_data(argument, &context, &flags);

    long index;
    ply_get_argument_element(argument, NULL, &index);

    float_t value = ply_get_argument_value(argument);

    PPLY_DATA ply_data = (PPLY_DATA)context;
    switch (flags)
    {
        case FLAG_X:
            ply_data->normals[index].x = value;
            break;
        case FLAG_Y:
            ply_data->normals[index].y = value;
            break;
        case FLAG_Z:
            ply_data->normals[index].z = value;
            break;
        default:
            return RPLY_FAILURE;
    }

    return RPLY_SUCCESS;
}

static
size_t
GetUIndex(
    _In_ long index
    )
{
    return 2 * index;
}

static
size_t
GetVIndex(
    _In_ long index
    )
{
    return 2 * index + 1;
}

static
int
PlyUvCallback(
    _Inout_ p_ply_argument argument
    )
{
    void *context;
    long flags;
    ply_get_argument_user_data(argument, &context, &flags);

    long index;
    ply_get_argument_element(argument, NULL, &index);

    float_t value = ply_get_argument_value(argument);

    PPLY_DATA ply_data = (PPLY_DATA)context;
    switch (flags)
    {
        case FLAG_U:
            ply_data->uvs[GetUIndex(index)] = value;
            break;
        case FLAG_V:
            ply_data->uvs[GetVIndex(index)] = value;
            break;
        default:
            return RPLY_FAILURE;
    }

    return RPLY_SUCCESS;
}

static
int
PlyVertexIndiciesCallback(
    _Inout_ p_ply_argument argument
    )
{
    void *context;
    long flags;
    ply_get_argument_user_data(argument, &context, &flags);

    long length, index;
    ply_get_argument_property(argument, NULL, &length, &index);

    if (index == -1)
    {
        return RPLY_SUCCESS;
    }

    if (index < 0 || length < 3 || 4 < length)
    {
        return RPLY_FAILURE;
    }

    long value = (long)ply_get_argument_value(argument);

    size_t vertex_index;
    if (!AsSizeT(value, &vertex_index))
    {
        return RPLY_FAILURE;
    }

    PPLY_DATA ply_data = (PPLY_DATA)context;
    if (ply_data->num_vertices <= vertex_index)
    {
        return RPLY_FAILURE;
    }

    switch (index)
    {
        case 0:
        case 1:
        case 2:
            ply_data->faces[ply_data->num_faces++] = vertex_index;
            break;
        case 3:
            ply_data->faces[ply_data->num_faces] =
                ply_data->faces[ply_data->num_faces - 3];
            ply_data->num_faces += 1;
            ply_data->faces[ply_data->num_faces] =
                ply_data->faces[ply_data->num_faces - 2];
            ply_data->num_faces += 1;
            ply_data->faces[ply_data->num_faces++] = vertex_index;
            break;
        default:
            return RPLY_FAILURE;
    }

    return RPLY_SUCCESS;
}

static
bool
InitializeVertexCallbacks(
    _Inout_ p_ply ply,
    _Inout_ PPLY_DATA ply_data,
    _In_ size_t num_vertices
    )
{
    assert(ply != NULL);
    assert(ply_data != NULL);

    long num_x = ply_set_read_cb(ply,
                                 "vertex",
                                 "x",
                                 PlyVertexCallback,
                                 ply_data,
                                 FLAG_X);

    if (num_x != (long)num_vertices)
    {
        return false;
    }

    long num_y = ply_set_read_cb(ply,
                                 "vertex",
                                 "y",
                                 PlyVertexCallback,
                                 ply_data,
                                 FLAG_Y);

    if (num_y != (long)num_vertices)
    {
        return false;
    }

    long num_z = ply_set_read_cb(ply,
                                 "vertex",
                                 "z",
                                 PlyVertexCallback,
                                 ply_data,
                                 FLAG_Z);

    return num_z == (long)num_vertices;
}


static
bool
InitializeNormalCallbacks(
    _Inout_ p_ply ply,
    _Inout_ PPLY_DATA ply_data,
    _In_ size_t num_vertices,
    _Out_ bool *found
    )
{
    assert(ply != NULL);
    assert(ply_data != NULL);
    assert(found != NULL);

    long num_nx = ply_set_read_cb(ply,
                                  "vertex",
                                  "nx",
                                  PlyNormalCallback,
                                  ply_data,
                                  FLAG_X);

    long expected_normals;
    if (num_nx != 0)
    {
        if (num_nx != (long)num_vertices)
        {
            return false;
        }

        expected_normals = num_vertices;
        *found = true;
    }
    else
    {
        expected_normals = 0;
        *found = false;
    }

    long num_ny = ply_set_read_cb(ply,
                                  "vertex",
                                  "ny",
                                  PlyNormalCallback,
                                  ply_data,
                                  FLAG_Y);

    if (num_ny != expected_normals)
    {
        return false;
    }

    long num_nz = ply_set_read_cb(ply,
                                  "vertex",
                                  "nz",
                                  PlyNormalCallback,
                                  ply_data,
                                  FLAG_Z);

    return num_nz == expected_normals;
}

static
bool
InitializeUVCallback(
    _Inout_ p_ply ply,
    _Inout_ PPLY_DATA ply_data,
    _In_ const char *u_name,
    _In_ const char *v_name,
    _In_ size_t num_vertices,
    _Inout_ bool *found
    )
{
    assert(ply != NULL);
    assert(ply_data != NULL);
    assert(u_name != NULL);
    assert(v_name != NULL);
    assert(found != NULL);

    long num_us = ply_set_read_cb(ply,
                                  "vertex",
                                  u_name,
                                  PlyUvCallback,
                                  ply_data,
                                  FLAG_U);

    long expected_vs;
    if (*found)
    {
        if (num_us != 0)
        {
            return false;
        }

        expected_vs = 0;
    }
    else
    {
        if (num_us != 0)
        {
            if (num_us != (long)num_vertices)
            {
                return false;
            }

            *found = true;
            expected_vs = (long)num_vertices;
        }
        else
        {
            expected_vs = 0;
        }
    }

    long num_vs = ply_set_read_cb(ply,
                                  "vertex",
                                  v_name,
                                  PlyUvCallback,
                                  ply_data,
                                  FLAG_V);

    return num_vs == expected_vs;
}

static
bool
InitializeUVCallbacks(
    _Inout_ p_ply ply,
    _Inout_ PPLY_DATA ply_data,
    _In_ size_t num_vertices,
    _Out_ bool *found
    )
{
    assert(ply != NULL);
    assert(ply_data != NULL);
    assert(found != NULL);

    *found = false;
    bool success = InitializeUVCallback(ply,
                                        ply_data,
                                        "u",
                                        "v",
                                        num_vertices,
                                        found);
    if (!success) {
        return false;
    }

    success = InitializeUVCallback(ply,
                                   ply_data,
                                   "s",
                                   "t",
                                   num_vertices,
                                   found);
    if (!success) {
        return false;
    }

    success = InitializeUVCallback(ply,
                                   ply_data,
                                   "texture_u",
                                   "texture_v",
                                   num_vertices,
                                   found);
    if (!success) {
        return false;
    }

    success = InitializeUVCallback(ply,
                                   ply_data,
                                   "texture_s",
                                   "texture_t",
                                   num_vertices,
                                   found);
    return success;
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

    if (!InitializeVertexCallbacks(ply, context, num_vertices))
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

    bool found;
    if (!InitializeNormalCallbacks(ply, context, num_vertices, &found))
    {
        FreePlyData(context);
        ply_close(ply);
        fclose(file);
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!found)
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

    if (!InitializeUVCallbacks(ply, context, num_vertices, &found))
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
                                          context,
                                          0);

    if (vertex_indices != 0)
    {
        if (num_vertices == 0 || vertex_indices != (long)num_faces)
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
    }
    else
    {
        context->faces = NULL;
    }

    context->num_faces = 0;

    int read_status = ply_read(ply);

    ply_close(ply);
    fclose(file);

    if (read_status != RPLY_SUCCESS)
    {
        FreePlyData(context);
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (context->num_faces % 3 != 0)
    {
        FreePlyData(context);
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (context->num_faces != 0)
    {
        size_t *resized = (size_t*)realloc(context->faces,
                                           sizeof(size_t) * context->num_faces);
        if (resized != NULL)
        {
            context->faces = resized;
        }

        context->num_faces /= 3;
    }

    for (size_t i = 0; i < context->num_vertices; i++)
    {
        if (!PointValidate(context->vertices[i]))
        {
            FreePlyData(context);
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        if (context->normals && !VectorValidate(context->normals[i]))
        {
            FreePlyData(context);
            return ISTATUS_INVALID_ARGUMENT_00;
        }
    }

    for (size_t i = 0; i < num_faces; i++)
    {
        size_t face_base = 3 * i;
        size_t face0 = context->faces[face_base];
        size_t face1 = context->faces[face_base + 1];
        size_t face2 = context->faces[face_base + 2];

        if (face0 == face1 || face1 == face2 || face0 == face2)
        {
            FreePlyData(context);
            return ISTATUS_INVALID_ARGUMENT_00;
        }
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