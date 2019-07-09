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
#include "iris_physx_toolkit/triangle.h"
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

//
// Functions
//

ISTATUS
ReadFromPlyFile(
    _In_ const char* filename,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _In_opt_ PEMISSIVE_MATERIAL front_emissive_material,
    _In_opt_ PEMISSIVE_MATERIAL back_emissive_material,
    _Outptr_result_buffer_(*num_shapes) PSHAPE *shapes,
    _Out_ size_t *num_shapes
    )
{
    if (filename == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (shapes == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (num_shapes == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
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

    // TODO

    return ISTATUS_ALLOCATION_FAILED;
}