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

#include "iris_physx_toolkit/ply_reader.h"
#include "third_party/rply/rply.h"
#include "third_party/rply/rplyfile.h"

//
// Types
//

typedef struct _RPLY_CONTEXT {
    _Field_size_(num_vertices) float_t *x_list;
    _Field_size_(num_vertices) float_t *y_list;
    _Field_size_(num_vertices) float_t *z_list;
    _Field_size_(num_vertices) float_t *nx_list;
    _Field_size_(num_vertices) float_t *ny_list;
    _Field_size_(num_vertices) float_t *nz_list;
    _Field_size_(num_vertices) float_t *u_list;
    _Field_size_(num_vertices) float_t *v_list;
    size_t num_vertices;
    _Field_size_(num_faces) size_t *vertex0_list;
    _Field_size_(num_faces) size_t *vertex1_list;
    _Field_size_(num_faces) size_t *vertex2_list;
    _Field_size_(num_faces) uint32_t *face_index_list;
    size_t num_faces;
} RPLY_CONTEXT, *PRPLY_CONTEXT;

//
// Static Functions
//

static
bool
AsSizeT(
    _In_ long value,
    _Out_ size_t *output
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

    *output = (size_t)value;

    return true;
}

static
void
RplyContextDestroy(
    _In_ _Post_invalid_ PRPLY_CONTEXT rply_context
    )
{
    assert(rply_context != NULL);

    free(rply_context->x_list);
    free(rply_context->y_list);
    free(rply_context->z_list);
    free(rply_context->nx_list);
    free(rply_context->ny_list);
    free(rply_context->nz_list);
    free(rply_context->u_list);
    free(rply_context->v_list);
    free(rply_context->vertex0_list);
    free(rply_context->vertex1_list);
    free(rply_context->vertex2_list);
    free(rply_context->face_index_list);
}

static
bool
RplyContextInitialize(
    _Inout_ PRPLY_CONTEXT rply_context,
    _In_ size_t num_vertices,
    _In_ size_t num_faces
    )
{
    assert(rply_context != NULL);

    memset(rply_context, 0, sizeof(RPLY_CONTEXT));

    rply_context->x_list = (float_t*)calloc(num_vertices, sizeof(float_t));

    if (rply_context->x_list == NULL)
    {
        RplyContextDestroy(rply_context);
        return false;
    }

    rply_context->y_list = (float_t*)calloc(num_vertices, sizeof(float_t));

    if (rply_context->y_list == NULL)
    {
        RplyContextDestroy(rply_context);
        return false;
    }

    rply_context->z_list = (float_t*)calloc(num_vertices, sizeof(float_t));

    if (rply_context->z_list == NULL)
    {
        RplyContextDestroy(rply_context);
        return false;
    }

    rply_context->nx_list = (float_t*)calloc(num_vertices, sizeof(float_t));

    if (rply_context->nx_list == NULL)
    {
        RplyContextDestroy(rply_context);
        return false;
    }

    rply_context->ny_list = (float_t*)calloc(num_vertices, sizeof(float_t));

    if (rply_context->ny_list == NULL)
    {
        RplyContextDestroy(rply_context);
        return false;
    }

    rply_context->nz_list = (float_t*)calloc(num_vertices, sizeof(float_t));

    if (rply_context->nz_list == NULL)
    {
        RplyContextDestroy(rply_context);
        return false;
    }

    rply_context->u_list = (float_t*)calloc(num_vertices, sizeof(float_t));

    if (rply_context->u_list == NULL)
    {
        RplyContextDestroy(rply_context);
        return false;
    }

    rply_context->v_list = (float_t*)calloc(num_vertices, sizeof(float_t));

    if (rply_context->v_list == NULL)
    {
        RplyContextDestroy(rply_context);
        return false;
    }

    rply_context->num_vertices = num_vertices;

    rply_context->vertex0_list = (size_t*)calloc(num_faces, sizeof(size_t));

    if (rply_context->vertex0_list == NULL)
    {
        RplyContextDestroy(rply_context);
        return false;
    }

    rply_context->vertex1_list = (size_t*)calloc(num_faces, sizeof(size_t));

    if (rply_context->vertex1_list == NULL)
    {
        RplyContextDestroy(rply_context);
        return false;
    }

    rply_context->vertex2_list = (size_t*)calloc(num_faces, sizeof(size_t));

    if (rply_context->vertex2_list == NULL)
    {
        RplyContextDestroy(rply_context);
        return false;
    }

    rply_context->face_index_list = 
        (uint32_t*)calloc(num_faces, sizeof(uint32_t));

    if (rply_context->face_index_list == NULL)
    {
        RplyContextDestroy(rply_context);
        return false;
    }

    rply_context->num_faces = num_faces;

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
                break;
            }
        }
        else if (strcmp(element_name, "vertex") == 0)
        {
            if (!AsSizeT(num_instances, &num_vertices))
            {
                break;
            }
        }
    }

    if (num_faces == 0 || num_vertices == 0)
    {
        ply_close(ply);
        fclose(file);
        return ISTATUS_IO_ERROR;
    }

    RPLY_CONTEXT rply_context;
    bool success = RplyContextInitialize(&rply_context,
                                         num_vertices,
                                         num_faces);

    if (!success)
    {
        ply_close(ply);
        fclose(file);
        return ISTATUS_ALLOCATION_FAILED;
    }

    // TODO

    return ISTATUS_ALLOCATION_FAILED;
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
    free(ply_data->uv);
    free(ply_data->faces);
    free(ply_data);
}