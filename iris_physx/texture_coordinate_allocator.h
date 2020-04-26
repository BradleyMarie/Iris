/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    texture_coordinate_allocator.h

Abstract:

    The allocator for texture coordinates.

--*/

#ifndef _IRIS_PHYSX_TEXTURE_COORDINATE_ALLOCATOR_
#define _IRIS_PHYSX_TEXTURE_COORDINATE_ALLOCATOR_

#include <stddef.h>

#include "common/sal.h"
#include "common/status.h"

//
// Types
//

typedef struct _TEXTURE_COORDINATE_ALLOCATOR TEXTURE_COORDINATE_ALLOCATOR, *PTEXTURE_COORDINATE_ALLOCATOR;
typedef const TEXTURE_COORDINATE_ALLOCATOR *PCTEXTURE_COORDINATE_ALLOCATOR;

//
// Functions
//

ISTATUS
TextureCoordinateAllocatorAllocate(
    _Inout_ PTEXTURE_COORDINATE_ALLOCATOR allocator,
    _In_ size_t size,
    _In_ size_t alignment,
    _Out_ void **allocation
    );

#endif // _IRIS_PHYSX_TEXTURE_COORDINATE_ALLOCATOR_