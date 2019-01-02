/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    light_sample_collector.h

Abstract:

    A collector for lighting samples.

--*/

#ifndef _IRIS_PHYSX_LIGHT_SAMPLE_COLLECTOR_
#define _IRIS_PHYSX_LIGHT_SAMPLE_COLLECTOR_

#include "iris_physx/light.h"

//
// Types
//

typedef struct _LIGHT_SAMPLE_COLLECTOR LIGHT_SAMPLE_COLLECTOR;
typedef LIGHT_SAMPLE_COLLECTOR *PLIGHT_SAMPLE_COLLECTOR;
typedef const LIGHT_SAMPLE_COLLECTOR *PCLIGHT_SAMPLE_COLLECTOR;

//
// Functions
//

ISTATUS
LightSampleCollectorAddSample(
    _Inout_ PLIGHT_SAMPLE_COLLECTOR collector,
    _In_ PCLIGHT light,
    _In_ float_t pdf
    );

#endif // _IRIS_PHYSX_LIGHT_SAMPLE_COLLECTOR_