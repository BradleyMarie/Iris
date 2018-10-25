/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    integrator_vtable.h

Abstract:

    The vtable for a integrator.

--*/

#ifndef _IRIS_PHYSX_INTEGRATOR_VTABLE_
#define _IRIS_PHYSX_INTEGRATOR_VTABLE_

#include "iris_advanced/iris_advanced.h"
#include "iris_physx/hit_tester.h"
#include "iris_physx/ray_tracer.h"
#include "iris_physx/reflector_allocator.h"
#include "iris_physx/spectrum_compositor.h"
#include "iris_physx/visibility_tester.h"

//
// Types
//

typedef
ISTATUS
(*PINTEGRATOR_INTEGRATE_ROUTINE)(
    _In_opt_ const void *context,
    _In_ PCRAY ray,
    _Inout_ PSHAPE_RAY_TRACER ray_tracer,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Inout_ PREFLECTOR_ALLOCATOR allocator,
    _Inout_ PRANDOM rng,
    _Out_ PCSPECTRUM *spectrum
    );

typedef struct _INTEGRATOR_VTABLE {
    PINTEGRATOR_INTEGRATE_ROUTINE integrate_routine;
    PFREE_ROUTINE free_routine;
} INTEGRATOR_VTABLE, *PINTEGRATOR_VTABLE;

typedef const INTEGRATOR_VTABLE *PCINTEGRATOR_VTABLE;

#endif // _IRIS_PHYSX_INTEGRATOR_VTABLE_