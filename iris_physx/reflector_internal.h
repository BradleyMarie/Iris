/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    reflector_internal.h

Abstract:

    The internal routines for sampling from a reflector.

--*/

#ifndef _IRIS_PHYSX_REFLECTOR_INTERNAL_
#define _IRIS_PHYSX_REFLECTOR_INTERNAL_

#include <stdatomic.h>

#include "iris_advanced/iris_advanced.h"
#include "iris_physx/reflector_vtable.h"

//
// Defines
//

#define ATTENUATED_REFLECTOR_TYPE     0
#define ATTENUATED_SUM_REFLECTOR_TYPE 1
#define PRODUCT_REFLECTOR_TYPE        2
#define PERFECT_REFLECTOR_TYPE        3
#define EXTERNAL_REFLECTOR_TYPE       4

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
    _Out_ struct _REFLECTOR *reflector,
    _In_ PCREFLECTOR_VTABLE vtable,
    _In_opt_ void *data
    )
{
    assert(reflector != NULL);
    assert(vtable != NULL);
    
    reflector->vtable = vtable;
    reflector->data = data;
    reflector->reference_count = EXTERNAL_REFLECTOR_TYPE;
}

static
inline
void
InternalReflectorInitialize(
    _Out_ struct _REFLECTOR *reflector,
    _In_ PCREFLECTOR_VTABLE vtable,
    _In_opt_ void *data,
    _In_ size_t type
    )
{
    assert(reflector != NULL);
    assert(vtable != NULL);
    assert(type < EXTERNAL_REFLECTOR_TYPE);

    reflector->vtable = vtable;
    reflector->data = data;
    reflector->reference_count = type;
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