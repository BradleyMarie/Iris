/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    uniform_reflector.c

Abstract:

    Implements a uniform reflector.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/uniform_reflector.h"

//
// Types
//

typedef struct _UNIFORM_REFLECTOR {
    float_t albedo;
} UNIFORM_REFLECTOR, *PUNIFORM_REFLECTOR;

typedef const UNIFORM_REFLECTOR *PCUNIFORM_REFLECTOR;

//
// Static Functions
//

static
ISTATUS
UniformReflectorReflect(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    )
{
    PCUNIFORM_REFLECTOR uniform_reflector = (PCUNIFORM_REFLECTOR)context;

    *reflectance = uniform_reflector->albedo;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
UniformReflectorGetAlbedo(
    _In_ const void *context,
    _Out_ float_t *albedo
    )
{
    PCUNIFORM_REFLECTOR uniform_reflector = (PCUNIFORM_REFLECTOR)context;

    *albedo = uniform_reflector->albedo;

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

static const REFLECTOR_VTABLE uniform_reflector_vtable = {
    UniformReflectorReflect,
    UniformReflectorGetAlbedo,
    NULL
};

//
// Functions
//

ISTATUS
UniformReflectorAllocate(
    _In_ float_t albedo,
    _Out_ PREFLECTOR *reflector
    )
{
    if (isless(albedo, (float_t)0.0) ||
        isgreater(albedo, (float_t)1.0))
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    UNIFORM_REFLECTOR uniform_reflector;
    uniform_reflector.albedo = albedo;

    ISTATUS status = ReflectorAllocate(&uniform_reflector_vtable,
                                       &uniform_reflector,
                                       sizeof(UNIFORM_REFLECTOR),
                                       alignof(UNIFORM_REFLECTOR),
                                       reflector);

    return status;
}