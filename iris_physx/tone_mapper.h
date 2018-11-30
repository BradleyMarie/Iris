/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    tone_mapper.h

Abstract:

    Interface representing a tone mapper.

--*/

#ifndef _IRIS_PHYSX_TONE_MAPPER_
#define _IRIS_PHYSX_TONE_MAPPER_

#include <stddef.h>

#include "iris_physx/tone_mapper_vtable.h"

//
// Types
//

typedef struct _TONE_MAPPER TONE_MAPPER, *PTONE_MAPPER;
typedef const TONE_MAPPER *PCTONE_MAPPER;

//
// Functions
//

ISTATUS
ToneMapperAllocate(
    _In_ PCTONE_MAPPER_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PTONE_MAPPER *tone_mapper
    );

ISTATUS
ToneMapperComputeTone(
    _In_ PCTONE_MAPPER tone_mapper,
    _Out_ PCOLOR3 color
    );

ISTATUS
ToneMapperClear(
    _In_ PTONE_MAPPER tone_mapper
    );

void
ToneMapperFree(
    _In_opt_ _Post_invalid_ PTONE_MAPPER tone_mapper
    );

#endif // _IRIS_PHYSX_TONE_MAPPER_