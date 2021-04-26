/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    environmental_light_vtable.h

Abstract:

    The vtable for an envionmental light.

--*/

#ifndef _IRIS_PHYSX_ENVIRONMENTAL_LIGHT_VTABLE_
#define _IRIS_PHYSX_ENVIRONMENTAL_LIGHT_VTABLE_

#include "iris_advanced/iris_advanced.h"
#include "iris_physx/spectrum_compositor.h"

//
// Types
//

typedef
ISTATUS
(*PENVIRONMENTAL_LIGHT_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 surface_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ PVECTOR3 to_light,
    _Out_ float_t *pdf
    );

typedef
ISTATUS
(*PENVIRONMENTAL_LIGHT_COMPUTE_EMISSIVE_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    );

typedef
ISTATUS
(*PENVIRONMENTAL_LIGHT_COMPUTE_EMISSIVE_WITH_PDF_ROUTINE)(
    _In_ const void *context,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    );

typedef struct _ENVIRONMENTAL_LIGHT_VTABLE {
    PENVIRONMENTAL_LIGHT_SAMPLE_ROUTINE sample_routine;
    PENVIRONMENTAL_LIGHT_COMPUTE_EMISSIVE_ROUTINE compute_emissive_routine;
    PENVIRONMENTAL_LIGHT_COMPUTE_EMISSIVE_WITH_PDF_ROUTINE compute_emissive_with_pdf_routine;
    PFREE_ROUTINE free_routine;
} ENVIRONMENTAL_LIGHT_VTABLE, *PENVIRONMENTAL_LIGHT_VTABLE;

typedef const ENVIRONMENTAL_LIGHT_VTABLE *PCENVIRONMENTAL_LIGHT_VTABLE;

#endif // _IRIS_PHYSX_ENVIRONMENTAL_LIGHT_VTABLE_