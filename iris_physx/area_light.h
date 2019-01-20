/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    area_light.h

Abstract:

    Allocates an area light associated with shape.

--*/

#ifndef _IRIS_AREA_LIGHT_
#define _IRIS_AREA_LIGHT_

#include "iris_physx/light.h"
#include "iris_physx/shape.h"

//
// Functions
//

ISTATUS
AreaLightAllocate(
    _In_ PSHAPE shape,
    _In_ uint32_t face,
    _In_opt_ PMATRIX model_to_world,
    _Out_ PLIGHT *light
    );

#endif // _IRIS_AREA_LIGHT_