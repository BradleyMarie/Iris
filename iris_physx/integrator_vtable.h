/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    integrator_vtable.h

Abstract:

    The vtable for a integrator.

--*/

#ifndef _IRIS_PHYSX_INTEGRATOR_VTABLE_
#define _IRIS_PHYSX_INTEGRATOR_VTABLE_

#include "iris_advanced/iris_advanced.h"
#include "iris_physx/hit_tester.h"
#include "iris_physx/light_sampler.h"
#include "iris_physx/ray_tracer.h"
#include "iris_physx/reflector_compositor.h"
#include "iris_physx/spectrum_compositor.h"
#include "iris_physx/visibility_tester.h"

//
// Types
//

typedef struct _INTEGRATOR INTEGRATOR, *PINTEGRATOR;
typedef const INTEGRATOR *PCINTEGRATOR;

typedef
ISTATUS
(*PINTEGRATOR_INTEGRATE_ROUTINE)(
    _In_opt_ const void *context,
    _In_ PCRAY ray,
    _In_ PCLIGHT_SAMPLER light_sampler,
    _Inout_ PLIGHT_SAMPLE_LIST light_sample_list,
    _Inout_ PSHAPE_RAY_TRACER ray_tracer,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Inout_ PREFLECTOR_COMPOSITOR allocator,
    _Inout_ PRANDOM rng,
    _Out_ PCSPECTRUM *spectrum
    );

typedef
ISTATUS
(*PINTEGRATOR_DUPLICATE_ROUTINE)(
    _In_opt_ const void *context,
    _Out_ PINTEGRATOR *duplicate
    );

typedef struct _INTEGRATOR_VTABLE {
    PINTEGRATOR_INTEGRATE_ROUTINE integrate_routine;
    PINTEGRATOR_DUPLICATE_ROUTINE duplicate_routine;
    PFREE_ROUTINE free_routine;
} INTEGRATOR_VTABLE, *PINTEGRATOR_VTABLE;

typedef const INTEGRATOR_VTABLE *PCINTEGRATOR_VTABLE;

#endif // _IRIS_PHYSX_INTEGRATOR_VTABLE_