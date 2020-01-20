/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    light_sample_list.c

Abstract:

    Implements a light sample_list.

--*/

#include "iris_physx/light_sample_list.h"
#include "iris_physx/light_sample_list_internal.h"

//
// Functions
//

ISTATUS
LightSampleListGetSize(
    _In_ PCLIGHT_SAMPLE_LIST light_sample_list,
    _Out_ size_t *size
    )
{
    if (light_sample_list == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (size == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    *size =
        LightSampleCollectorGetSampleCount(&light_sample_list->collector);

    return ISTATUS_SUCCESS;
}


ISTATUS
LightSampleListGetSample(
    _In_ PCLIGHT_SAMPLE_LIST light_sample_list,
    _In_ size_t index,
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
    )
{
    if (light_sample_list == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    size_t num_samples =
        LightSampleCollectorGetSampleCount(&light_sample_list->collector);

    if (num_samples <= index)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    LightSampleCollectorGetSample(&light_sample_list->collector,
                                  index,
                                  light,
                                  pdf);

    return ISTATUS_SUCCESS;
}