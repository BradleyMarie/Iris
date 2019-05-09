/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    light_sample_list_internal.h

Abstract:

    Internal definitions for light sample list.

--*/

#ifndef _IRIS_PHYSX_LIGHT_SAMPLE_LIST_INTERNAL_
#define _IRIS_PHYSX_LIGHT_SAMPLE_LIST_INTERNAL_

#include <assert.h>

#include "iris_physx/light_sample_collector_internal.h"
#include "iris_physx/light_sample_collector.h"

//
// Types
//

struct _LIGHT_SAMPLE_LIST {
    LIGHT_SAMPLE_COLLECTOR collector;
};

//
// Functions
//

static
inline
bool
LightSampleListInitialize(
    _Out_ struct _LIGHT_SAMPLE_LIST *list
    )
{
    assert(list != NULL);

    return LightSampleCollectorInitialize(&list->collector);
}

static
inline
void
LightSampleListDestroy(
    _Inout_ struct _LIGHT_SAMPLE_LIST *list
    )
{
    assert(list != NULL);

    LightSampleCollectorDestroy(&list->collector);
}

#endif // _IRIS_PHYSX_LIGHT_SAMPLE_LIST_INTERNAL_