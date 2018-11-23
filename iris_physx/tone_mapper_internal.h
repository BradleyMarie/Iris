/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    tone_mapper_internal.h

Abstract:

    The internal routines for a tone mapper.

--*/

#ifndef _IRIS_PHYSX_TONE_MAPPER_INTERNAL_
#define _IRIS_PHYSX_TONE_MAPPER_INTERNAL_

#include "iris_physx/tone_mapper_vtable.h"

//
// Types
//

struct _TONE_MAPPER {
    PCTONE_MAPPER_VTABLE vtable;
    void *data;
};

//
// Functions
//

static
inline
void
ToneMapperInitialize(
    _In_ PCTONE_MAPPER_VTABLE vtable,
    _In_opt_ void *data,
    _Out_ struct _TONE_MAPPER *tone_mapper
    )
{
    assert(vtable != NULL);
    assert(tone_mapper != NULL);
    
    tone_mapper->vtable = vtable;
    tone_mapper->data = data;
}

static
inline
ISTATUS
ToneMapperAddSample(
    _In_ struct _TONE_MAPPER *tone_mapper,
    _In_opt_ PCSPECTRUM spectrum
    )
{
    assert(tone_mapper != NULL);

    ISTATUS status = tone_mapper->vtable->add_sample_routine(tone_mapper->data,
                                                             spectrum);

    return status;
}

#endif // _IRIS_PHYSX_TONE_MAPPER_INTERNAL_