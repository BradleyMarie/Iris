/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    random_generator_internal.h

Abstract:

    The internal routines for a random generator.

--*/

#ifndef _IRIS_CAMERA_RANDOM_GENERATOR_INTERNAL_
#define _IRIS_CAMERA_RANDOM_GENERATOR_INTERNAL_

#include "iris_camera/random_generator_vtable.h"

//
// Types
//

struct _RANDOM_GENERATOR {
    PCRANDOM_GENERATOR_VTABLE vtable;
    void *data;
};

//
// Functions
//

ISTATUS
RandomGeneratorGenerate(
    _In_ struct _RANDOM_GENERATOR *random_generator,
    _Out_ PRANDOM *random
    )
{
    assert(random_generator != NULL);
    assert(random != NULL);

    PRANDOM result;
    ISTATUS status = random_generator->vtable->generate_routine(
            random_generator->data, &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *random = result;

    return ISTATUS_SUCCESS;
}

#endif // _IRIS_CAMERA_RANDOM_GENERATOR_INTERNAL_