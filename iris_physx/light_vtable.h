/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    light_vtable.h

Abstract:

    The vtable for a light.

--*/

#ifndef _IRIS_PHYSX_LIGHT_VTABLE_
#define _IRIS_PHYSX_LIGHT_VTABLE_

#include "iris_advanced/iris_advanced.h"
#include "iris_physx/color_cache.h"
#include "iris_physx/spectrum_compositor.h"
#include "iris_physx/visibility_tester.h"

//
// Types
//

typedef
ISTATUS
(*LIGHT_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 surface_normal,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PRANDOM rng,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ PVECTOR3 to_light,
    _Out_ float_t *pdf
    );

typedef
ISTATUS
(*PLIGHT_COMPUTE_EMISSIVE_ROUTINE)(
    _In_ const void *context,
    _In_ PCRAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    );

typedef
ISTATUS
(*PLIGHT_COMPUTE_EMISSIVE_WITH_PDF_ROUTINE)(
    _In_ const void *context,
    _In_ PCRAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    );

typedef struct _LIGHT_VTABLE {
    LIGHT_SAMPLE_ROUTINE sample_routine;
    PLIGHT_COMPUTE_EMISSIVE_ROUTINE compute_emissive_routine;
    PLIGHT_COMPUTE_EMISSIVE_WITH_PDF_ROUTINE compute_emissive_with_pdf_routine;
    PCACHE_COLORS_ROUTINE cache_colors_routine;
    PFREE_ROUTINE free_routine;
} LIGHT_VTABLE, *PLIGHT_VTABLE;

typedef const LIGHT_VTABLE *PCLIGHT_VTABLE;

#endif // _IRIS_PHYSX_LIGHT_VTABLE_