/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    light_sample_collector_internal.h

Abstract:

    Internal header for light sample collector.

--*/

#ifndef _IRIS_PHYSX_LIGHT_SAMPLE_COLLECTOR_INTERNAL_
#define _IRIS_PHYSX_LIGHT_SAMPLE_COLLECTOR_INTERNAL_

#include "common/const_pointer_list.h"
#include "iris_physx/light.h"

//
// Defines
//

#define LIGHT_SAMPLE_COLLECTOR_INITIAL_CAPACITY 16

//
// Types
//

typedef struct _LIGHT_AND_PDF {
    PCLIGHT light;
    float_t pdf;
} LIGHT_AND_PDF, *PLIGHT_AND_PDF;

typedef const LIGHT_AND_PDF *PCLIGHT_AND_PDF;

struct _LIGHT_SAMPLE_COLLECTOR {
    _Field_size_(capacity) PLIGHT_AND_PDF samples;
    size_t capacity;
    size_t size;
};

//
// Functions
//

static
inline
bool
LightSampleCollectorInitialize(
    _Out_ struct _LIGHT_SAMPLE_COLLECTOR *collector
    )
{
    assert(collector != NULL);

    collector->samples = 
        (PLIGHT_AND_PDF)calloc(LIGHT_SAMPLE_COLLECTOR_INITIAL_CAPACITY,
                               sizeof(LIGHT_AND_PDF));

    if (collector->samples == NULL)
    {
        return false;
    }

    collector->capacity = LIGHT_SAMPLE_COLLECTOR_INITIAL_CAPACITY;
    collector->size = 0;

    return true;
}

static
inline
size_t
LightSampleCollectorGetSampleCount(
    _In_ const struct _LIGHT_SAMPLE_COLLECTOR *collector
    )
{
    assert(collector != NULL);

    return collector->size;
}

static
inline
void
LightSampleCollectorGetSample(
    _In_ const struct _LIGHT_SAMPLE_COLLECTOR *collector,
    _In_ size_t index,
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
    )
{
    assert(collector != NULL);
    assert(index < collector->size);
    assert(light != NULL);
    assert(pdf != NULL);

    *light = collector->samples[index].light;
    *pdf = collector->samples[index].pdf;
}

static
inline
void
LightSampleCollectorClear(
    _Inout_ struct _LIGHT_SAMPLE_COLLECTOR *collector
    )
{
    assert(collector != NULL);

    collector->size = 0;
}

static
inline
void
LightSampleCollectorDestroy(
    _Inout_ struct _LIGHT_SAMPLE_COLLECTOR *collector
    )
{
    assert(collector != NULL);

    free(collector->samples);
}

#undef LIGHT_SAMPLE_COLLECTOR_INITIAL_CAPACITY

#endif // _IRIS_PHYSX_LIGHT_SAMPLE_COLLECTOR_INTERNAL_