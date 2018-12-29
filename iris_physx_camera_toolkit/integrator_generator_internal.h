/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    integrator_generator_internal.h

Abstract:

    The internal routines for an integrator generator.

--*/

#ifndef _IRIS_PHYSX_CAMERA_INTEGRATOR_GENERATOR_INTERNAL_
#define _IRIS_PHYSX_CAMERA_INTEGRATOR_GENERATOR_INTERNAL_

#include "iris_physx_camera_toolkit/integrator_generator_vtable.h"

//
// Types
//

struct _INTEGRATOR_GENERATOR {
    PCINTEGRATOR_GENERATOR_VTABLE vtable;
    void *data;
};

//
// Functions
//

ISTATUS
IntegratorGeneratorGenerate(
    _In_ const struct _INTEGRATOR_GENERATOR *integrator_generator,
    _Out_ PINTEGRATOR *integrator
    )
{
    assert(integrator_generator != NULL);
    assert(integrator != NULL);

    PINTEGRATOR result;
    ISTATUS status = integrator_generator->vtable->generate_routine(
        integrator_generator->data, &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *integrator = result;

    return ISTATUS_SUCCESS;
}

#endif // _IRIS_PHYSX_CAMERA_INTEGRATOR_GENERATOR_INTERNAL_