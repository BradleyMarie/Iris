/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    light_sample_list.h

Abstract:

    A list of light samples.

--*/

#ifndef _IRIS_PHYSX_LIGHT_SAMPLE_LIST_
#define _IRIS_PHYSX_LIGHT_SAMPLE_LIST_

#include "iris_physx/light.h"

//
// Types
//

typedef struct _LIGHT_SAMPLE_LIST LIGHT_SAMPLE_LIST, *PLIGHT_SAMPLE_LIST;
typedef const LIGHT_SAMPLE_LIST *PCLIGHT_SAMPLE_LIST;

//
// Functions
//

ISTATUS
LightSampleListGetSize(
    _In_ PCLIGHT_SAMPLE_LIST light_sample_list,
    _Out_ size_t *size
    );

ISTATUS
LightSampleListGetSample(
    _In_ PCLIGHT_SAMPLE_LIST light_sample_list,
    _In_ size_t index,
    _Out_ PCLIGHT *light,
    _Out_ float_t *pdf
    );

#endif // _IRIS_PHYSX_LIGHT_SAMPLE_LIST_