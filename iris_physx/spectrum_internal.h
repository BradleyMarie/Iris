/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    spectrum_internal.h

Abstract:

    The internal routines for sampling from a spectum.

--*/

#ifndef _IRIS_PHYSX_SPECTRUM_INTERNAL_
#define _IRIS_PHYSX_SPECTRUM_INTERNAL_

#include <stdatomic.h>

#include "iris_advanced/iris_advanced.h"
#include "iris_physx/spectrum_vtable.h"

//
// Forward Declarations
//

struct _COLOR_CACHE;

//
// Types
//

struct _SPECTRUM {
    PCSPECTRUM_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

typedef
ISTATUS
(*PSPECTRUM_COMPUTE_COLOR_ROUTINE)(
    _In_opt_ const void *context,
    _In_ const struct _COLOR_CACHE *color_cache,
    _Out_ PCOLOR3 color
    );

typedef struct _INTERNAL_SPECTRUM_VTABLE {
    SPECTRUM_VTABLE spectrum_vtable;
    PSPECTRUM_COMPUTE_COLOR_ROUTINE compute_color_routine;
} INTERNAL_SPECTRUM_VTABLE, *PINTERNAL_SPECTRUM_VTABLE;

typedef const INTERNAL_SPECTRUM_VTABLE *PCINTERNAL_SPECTRUM_VTABLE;

//
// Functions
//

static
inline
void
SpectrumInitialize(
    _Out_ struct _SPECTRUM *spectrum,
    _In_ PCSPECTRUM_VTABLE vtable,
    _In_opt_ void *data
    )
{
    assert(spectrum != NULL);
    assert(vtable != NULL);
    
    spectrum->vtable = vtable;
    spectrum->data = data;
    spectrum->reference_count = 1;
}

static
inline
void
InternalSpectrumInitialize(
    _Out_ struct _SPECTRUM *spectrum,
    _In_ PCINTERNAL_SPECTRUM_VTABLE vtable,
    _In_opt_ void *data
    )
{
    assert(spectrum != NULL);
    assert(vtable != NULL);

    spectrum->vtable = &vtable->spectrum_vtable;
    spectrum->data = data;
    spectrum->reference_count = 0;
}

static
inline
ISTATUS
SpectrumSampleInline(
    _In_ const struct _SPECTRUM *spectrum,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    assert(spectrum != NULL);
    assert(isfinite(wavelength));
    assert((float_t)0.0 < wavelength);
    assert(intensity != NULL);

    ISTATUS status = spectrum->vtable->sample_routine(spectrum->data,
                                                      wavelength,
                                                      intensity);

    return status;
}

#endif // _IRIS_PHYSX_SPECTRUM_INTERNAL_