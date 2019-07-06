/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    ply_reader.h

Abstract:

    Reads a PLY file.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_PLY_READER_
#define _IRIS_PHYSX_TOOLKIT_PLY_READER_

#include "iris_physx/iris_physx.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

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
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_PLY_READER_