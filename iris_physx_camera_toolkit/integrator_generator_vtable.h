/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    integrator_generator_vtable.h

Abstract:

    The vtable for an integrator generator.

--*/

#ifndef _IRIS_PHYSX_CAMERA_INTEGRATOR_GENERATOR_VTABLE_
#define _IRIS_PHYSX_CAMERA_INTEGRATOR_GENERATOR_VTABLE_

#include "iris_physx/iris_physx.h"
#include "iris_physx_camera_toolkit/integrator_generator.h"

typedef
ISTATUS
(*PINTEGRATOR_GENERATOR_GENERATE_ROUTINE)(
    _In_ const void *context,
    _Out_ PINTEGRATOR *integrator
    );

typedef struct _INTEGRATOR_GENERATOR_VTABLE {
    PINTEGRATOR_GENERATOR_GENERATE_ROUTINE generate_routine;
    PFREE_ROUTINE free_routine;
} INTEGRATOR_GENERATOR_VTABLE, *PINTEGRATOR_GENERATOR_VTABLE;

typedef const INTEGRATOR_GENERATOR_VTABLE *PCINTEGRATOR_GENERATOR_VTABLE;

#endif // _IRIS_PHYSX_CAMERA_INTEGRATOR_GENERATOR_VTABLE_