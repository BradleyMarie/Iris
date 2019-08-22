/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    ply_reader.h

Abstract:

    Reads a PLY file.

--*/

#ifndef _IRIS_ADVANCED_TOOLKIT_PLY_READER_
#define _IRIS_ADVANCED_TOOLKIT_PLY_READER_

#include "iris_advanced/iris_advanced.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef struct _PLY_DATA {
    _Field_size_(num_vertices) POINT3 *vertices;
    _Field_size_opt_(num_vertices) VECTOR3 *normals;
    _Field_size_opt_(2 * num_vertices) float_t *uvs; // float uvs[][2]
    size_t num_vertices;
    _Field_size_(3 * num_faces) size_t *faces; // size_t faces[][3]
    size_t num_faces;
} PLY_DATA, *PPLY_DATA;

typedef const PLY_DATA *PCPLY_DATA;

//
// Functions
//

ISTATUS
ReadFromPlyFile(
    _In_ const char* filename,
    _Out_ PPLY_DATA *ply_data
    );

void
FreePlyData(
    _In_opt_ _Post_invalid_ PPLY_DATA ply_data
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_ADVANCED_TOOLKIT_PLY_READER_