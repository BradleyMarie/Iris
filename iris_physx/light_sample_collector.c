/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    light_sample_collector.c

Abstract:

    A collector for lighting samples.

--*/

#include "iris_physx/light_sample_collector.h"
#include "iris_physx/light_sample_collector_internal.h"

//
// Defines
//

#define LIGHT_SAMPLE_COLLECTOR_GROWTH_FACTOR 2

//
// Functions
//

ISTATUS
LightSampleCollectorAddSample(
    _Inout_ PLIGHT_SAMPLE_COLLECTOR collector,
    _In_ PCLIGHT light,
    _In_ float_t pdf
    )
{
    if (collector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(pdf))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (collector->size == collector->capacity)
    {
        size_t new_capacity;
        bool success = CheckedMultiplySizeT(collector->capacity,
                                            LIGHT_SAMPLE_COLLECTOR_GROWTH_FACTOR,
                                            &new_capacity);
        
        if (!success)
        {
            return false;
        }

        size_t new_capacity_in_bytes;
        success = CheckedMultiplySizeT(new_capacity,
                                       sizeof(LIGHT_AND_PDF),
                                       &new_capacity_in_bytes);
        
        if (!success)
        {
            return false;
        }

        void *new_list = realloc((void *)collector->samples,
                                 new_capacity_in_bytes);

        if (new_list == NULL)
        {
            return false;
        }

        collector->samples = (PLIGHT_AND_PDF)new_list;
        collector->capacity = new_capacity;
    }

    collector->samples[collector->size].light = light;
    collector->samples[collector->size].pdf = pdf;
    collector->size += 1;

    return ISTATUS_SUCCESS;
}