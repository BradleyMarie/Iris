/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    normal_map.h

Abstract:

    Interface for an object that computes the normals used for shading.

--*/

#ifndef _IRIS_PHYSX_NORMAL_MAP_
#define _IRIS_PHYSX_NORMAL_MAP_

#include <stddef.h>

#include "iris_physx/normal_map_vtable.h"

//
// Types
//

typedef struct _NORMAL_MAP NORMAL_MAP, *PNORMAL_MAP;
typedef const NORMAL_MAP *PCNORMAL_MAP;

//
// Functions
//

ISTATUS
NormalMapAllocate(
    _In_ PCNORMAL_MAP_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PNORMAL_MAP *normal_map
    );

void
NormalMapRetain(
    _In_opt_ PNORMAL_MAP normal_map
    );

void
NormalMapRelease(
    _In_opt_ _Post_invalid_ PNORMAL_MAP normal_map
    );

#endif // _IRIS_PHYSX_NORMAL_MAP_