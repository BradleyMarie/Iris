/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    attenuated_reflector.c

Abstract:

    Implements a attenuated reflector.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/attenuated_reflector.h"

//
// AttenuatedAllocator Types
//

typedef struct _ATTENUATED_REFLECTOR {
    PCREFLECTOR reflector;
    float_t attenuation;
} ATTENUATED_REFLECTOR, *PATTENUATED_REFLECTOR;

typedef const ATTENUATED_REFLECTOR *PCATTENUATED_REFLECTOR;

//
// AttenuatedAllocator3 Static Functions
//

static
ISTATUS
AttenuatedReflectorReflect(
    _In_ const void *context,
    _In_ float_t wavelength,
    _In_ float_t incoming_intensity,
    _Out_ float_t *outgoing_intensity
    )
{
    PCATTENUATED_REFLECTOR attenuated_reflector =
        (PCATTENUATED_REFLECTOR)context;

    ISTATUS status = ReflectorReflect(attenuated_reflector->reflector,
                                      wavelength,
                                      incoming_intensity,
                                      outgoing_intensity);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *outgoing_intensity *= attenuated_reflector->attenuation;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AttenuatedReflectorGetAlbedo(
    _In_ const void *context,
    _Out_ float_t *albedo
    )
{
    PCATTENUATED_REFLECTOR attenuated_reflector =
        (PCATTENUATED_REFLECTOR)context;

    ISTATUS status = ReflectorGetAlbedo(attenuated_reflector->reflector,
                                        albedo);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *albedo *= attenuated_reflector->attenuation;

    return ISTATUS_SUCCESS;
}

//
// AttenuatedAllocator3 Static Variables
//

static const REFLECTOR_VTABLE attenuated_reflector_vtable = {
    AttenuatedReflectorReflect,
    AttenuatedReflectorGetAlbedo,
    NULL
};

//
// AttenuatedAllocator3 Functions
//

ISTATUS
AttenuatedReflectorAllocateWithAllocator(
    _In_ PREFLECTOR_ALLOCATOR reflector_allocator,
    _In_ PCREFLECTOR reflector_to_attenuate,
    _In_ float_t attenuation,
    _Out_ PCREFLECTOR *reflector
    )
{
    if (reflector_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflector_to_attenuate == NULL)
    {
        *reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    if (isless(attenuation, (float_t)0.0) || !isfinite(attenuation))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    ATTENUATED_REFLECTOR attenuated_reflector;
    attenuated_reflector.reflector = reflector_to_attenuate;
    attenuated_reflector.attenuation = attenuation;

    ISTATUS status =
        ReflectorAllocatorAllocate(reflector_allocator,
                                   &attenuated_reflector_vtable,
                                   &attenuated_reflector,
                                   sizeof(ATTENUATED_REFLECTOR),
                                   alignof(ATTENUATED_REFLECTOR),
                                   reflector);

    return status;
}

//
// AttenuatedAllocator3 Types
//

typedef struct _ATTENUATED_REFLECTOR3 {
    PCREFLECTOR reflectors[3];
    float_t attenuations[3];
} ATTENUATED_REFLECTOR3, *PATTENUATED_REFLECTOR3;

typedef const ATTENUATED_REFLECTOR3 *PCATTENUATED_REFLECTOR3;

//
// AttenuatedAllocator3 Static Functions
//

static
ISTATUS
AttenuatedReflector3Reflect(
    _In_ const void *context,
    _In_ float_t wavelength,
    _In_ float_t incoming_intensity,
    _Out_ float_t *outgoing_intensity
    )
{
    PCATTENUATED_REFLECTOR3 attenuated_reflector =
        (PCATTENUATED_REFLECTOR3)context;

    float_t local_outgoing_intensity;
    ISTATUS status = ReflectorReflect(attenuated_reflector->reflectors[0],
                                      wavelength,
                                      incoming_intensity,
                                      &local_outgoing_intensity);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t result = local_outgoing_intensity *
                     attenuated_reflector->attenuations[0];

    status = ReflectorReflect(attenuated_reflector->reflectors[1],
                              wavelength,
                              incoming_intensity,
                              &local_outgoing_intensity);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    result = fmaf(local_outgoing_intensity,
                  attenuated_reflector->attenuations[1],
                  result);

    status = ReflectorReflect(attenuated_reflector->reflectors[2],
                              wavelength,
                              incoming_intensity,
                              &local_outgoing_intensity);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *outgoing_intensity = fmaf(local_outgoing_intensity,
                               attenuated_reflector->attenuations[2],
                               result);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AttenuatedReflector3GetAlbedo(
    _In_ const void *context,
    _Out_ float_t *albedo
    )
{
    PCATTENUATED_REFLECTOR3 attenuated_reflector =
        (PCATTENUATED_REFLECTOR3)context;

    float_t local_albedo;
    ISTATUS status = ReflectorGetAlbedo(attenuated_reflector->reflectors[0],
                                        &local_albedo);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t result = local_albedo * attenuated_reflector->attenuations[0];

    status = ReflectorGetAlbedo(attenuated_reflector->reflectors[1],
                                &local_albedo);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    result = fmaf(local_albedo,
                  attenuated_reflector->attenuations[1],
                  result);

    status = ReflectorGetAlbedo(attenuated_reflector->reflectors[2],
                                &local_albedo);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *albedo = fmaf(local_albedo,
                   attenuated_reflector->attenuations[2],
                   result);

    return ISTATUS_SUCCESS;
}

//
// AttenuatedAllocator3 Static Variables
//

static const REFLECTOR_VTABLE attenuated_reflector3_vtable = {
    AttenuatedReflector3Reflect,
    AttenuatedReflector3GetAlbedo,
    NULL
};

//
// AttenuatedAllocator3 Functions
//

ISTATUS
AttenuatedReflector3AllocateWithAllocator(
    _In_ PREFLECTOR_ALLOCATOR reflector_allocator,
    _In_opt_ PREFLECTOR reflector0,
    _In_ float_t attenuation0,
    _In_opt_ PREFLECTOR reflector1,
    _In_ float_t attenuation1,
    _In_opt_ PREFLECTOR reflector2,
    _In_ float_t attenuation2,
    _Out_ PCREFLECTOR *reflector
    )
{
    if (reflector_allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (isless(attenuation0, (float_t)0.0) ||
        isgreater(attenuation0, (float_t)1.0))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (isless(attenuation1, (float_t)0.0) ||
        isgreater(attenuation1, (float_t)1.0))
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (isless(attenuation2, (float_t)0.0) ||
        isgreater(attenuation2, (float_t)1.0))
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    float_t attenuation_sum = attenuation0 + attenuation1 + attenuation2;

    if (isless(attenuation_sum, (float_t)0.0) ||
        isgreater(attenuation_sum, (float_t)1.0))
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    ATTENUATED_REFLECTOR3 attenuated_reflector;
    attenuated_reflector.reflectors[0] = reflector0;
    attenuated_reflector.reflectors[1] = reflector1;
    attenuated_reflector.reflectors[2] = reflector2;
    attenuated_reflector.attenuations[0] = attenuation0;
    attenuated_reflector.attenuations[1] = attenuation1;
    attenuated_reflector.attenuations[2] = attenuation2;

    ISTATUS status = 
        ReflectorAllocatorAllocate(reflector_allocator,
                                   &attenuated_reflector3_vtable,
                                   &attenuated_reflector,
                                   sizeof(ATTENUATED_REFLECTOR3),
                                   alignof(ATTENUATED_REFLECTOR3),
                                   reflector);

    return status;
}