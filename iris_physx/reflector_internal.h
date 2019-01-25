/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    reflector_internal.h

Abstract:

    The internal routines for sampling from a reflector.

--*/

#ifndef _IRIS_PHYSX_REFLECTOR_INTERNAL_
#define _IRIS_PHYSX_REFLECTOR_INTERNAL_

#include <stdatomic.h>

#include "iris_physx/reflector_vtable.h"

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
    _Out_ float_t *reflectance
    )
{
    assert(reflector != NULL);
    assert(isfinite(wavelength));
    assert((float_t)0.0 < wavelength);
    assert(reflectance != NULL);

    ISTATUS status = reflector->vtable->reflect_routine(reflector->data,
                                                        wavelength,
                                                        reflectance);

    return status;
}

static
inline
ISTATUS
ReflectorGetAlbedoInline(
    _In_ const struct _REFLECTOR *reflector,
    _Out_ float_t *albedo
    )
{
    assert(reflector != NULL);
    assert(albedo != NULL);

    ISTATUS status = reflector->vtable->get_albedo_routine(reflector->data,
                                                           albedo);

    return status;
}

#endif // _IRIS_PHYSX_REFLECTOR_INTERNAL_