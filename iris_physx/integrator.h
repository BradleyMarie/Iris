/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    integrator.h

Abstract:

    Manages the state needed to integrate over a ray.

--*/

#ifndef _IRIS_PHYSX_INTEGRATOR_
#define _IRIS_PHYSX_INTEGRATOR_

#include "iris_physx/color_integrator.h"
#include "iris_physx/integrator_vtable.h"
#include "iris_physx/light_sampler_vtable.h"
#include "iris_physx/scene.h"

//
// Functions
//

ISTATUS
IntegratorAllocate(
    _In_ PCINTEGRATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PINTEGRATOR *integrator
    );

ISTATUS
IntegratorPrepare(
    _Inout_ PINTEGRATOR integrator,
    _In_ PSCENE scene,
    _In_ PLIGHT_SAMPLER light_sampler,
    _In_ PCOLOR_INTEGRATOR color_integrator,
    _In_ bool precompute_colors
    );

ISTATUS
IntegratorIntegrate(
    _Inout_ PINTEGRATOR integrator,
    _Inout_ PRANDOM rng,
    _In_ RAY ray,
    _In_ float_t epsilon,
    _Out_ PCOLOR3 color
    );

ISTATUS
IntegratorIntegrateSpectral(
    _Inout_ PINTEGRATOR integrator,
    _Inout_ PRANDOM rng,
    _In_ RAY ray,
    _In_ float_t epsilon,
    _Out_ PCOLOR3 color
    );

ISTATUS
IntegratorDuplicate(
    _In_ PINTEGRATOR integrator,
    _Out_ PINTEGRATOR *duplicate
    );

void
IntegratorFree(
    _In_opt_ _Post_invalid_ PINTEGRATOR integrator
    );
    
#endif // _IRIS_PHYSX_INTEGRATOR_