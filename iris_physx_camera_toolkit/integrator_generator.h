/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    integrator_generator.h

Abstract:

    Generates an integrator generator.

--*/

#ifndef _IRIS_PHYSX_CAMERA_INTEGRATOR_GENERATOR_
#define _IRIS_PHYSX_CAMERA_INTEGRATOR_GENERATOR_

#include "iris_physx_camera_toolkit/integrator_generator_vtable.h"

//
// Types
//

typedef struct _INTEGRATOR_GENERATOR INTEGRATOR_GENERATOR;
typedef INTEGRATOR_GENERATOR *PINTEGRATOR_GENERATOR;
typedef const INTEGRATOR_GENERATOR *PCINTEGRATOR_GENERATOR;

//
// Functions
//

ISTATUS
IntegratorGeneratorAllocate(
    _In_ PCINTEGRATOR_GENERATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PINTEGRATOR_GENERATOR *integrator_generator
    );

void
IntegratorGeneratorFree(
    _In_opt_ _Post_invalid_ PINTEGRATOR_GENERATOR integrator_generator
    );

#endif // _IRIS_PHYSX_CAMERA_INTEGRATOR_GENERATOR_