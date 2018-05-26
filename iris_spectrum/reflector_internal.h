/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    reflector_internal.h

Abstract:

    The internal routines for sampling from a reflector.

--*/

#ifndef _IRIS_SPECTRUM_REFLECTOR_INTERNAL_
#define _IRIS_SPECTRUM_REFLECTOR_INTERNAL_

#include "iris_spectrum/reflector_vtable.h"

//
// Types
//

struct _REFLECTOR {
    PCREFLECTOR_VTABLE vtable;
    void *data;
    atomic_uintmax_t reference_count;
};

//
// Functions
//

static
inline
void
ReflectorInitialize(
    _In_ PCREFLECTOR_VTABLE vtable,
    _In_opt_ void *data,
    _Out_ struct _REFLECTOR *reflector
    )
{
    assert(vtable != NULL);
    assert(reflector != NULL);
    
    reflector->vtable = vtable;
    reflector->data = data;
    reflector->reference_count = 1;
}

static
inline
ISTATUS
ReflectorReflectInline(
    _In_ const struct _REFLECTOR *reflector,
    _In_ float_t wavelength,
    _In_ float_t incoming_intensity,
    _Out_ float_t *outgoing_intensity
    )
{
    assert(reflector != NULL);
    assert(isfinite(wavelength));
    assert((float_t)0.0 < wavelength);
    assert(outgoing_intensity != NULL);

    ISTATUS status = reflector->vtable->reflect_routine(reflector->data,
                                                        wavelength,
                                                        incoming_intensity,
                                                        outgoing_intensity);

    return status;
}

#endif // _IRIS_SPECTRUM_REFLECTOR_INTERNAL_