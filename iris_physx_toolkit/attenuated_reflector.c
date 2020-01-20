/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    attenuated_reflector.c

Abstract:

    Implements a attenuated reflector.

--*/

#include <stdalign.h>

#include "iris_physx_toolkit/attenuated_reflector.h"

//
// AttenuatedAllocator3 Functions
//

ISTATUS
AttenuatedReflector3AllocateWithAllocator(
    _In_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _In_opt_ PREFLECTOR reflector0,
    _In_ float_t attenuation0,
    _In_opt_ PREFLECTOR reflector1,
    _In_ float_t attenuation1,
    _In_opt_ PREFLECTOR reflector2,
    _In_ float_t attenuation2,
    _Out_ PCREFLECTOR *reflector
    )
{
    if (reflector_compositor == NULL)
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

    PCREFLECTOR result;
    ISTATUS status = ReflectorCompositorAttenuateReflector(reflector_compositor,
                                                           reflector0,
                                                           attenuation0,
                                                           &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorCompositorAttenuatedAddReflectors(reflector_compositor,
                                                        result,
                                                        reflector1,
                                                        attenuation1,
                                                        &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorCompositorAttenuatedAddReflectors(reflector_compositor,
                                                        result,
                                                        reflector2,
                                                        attenuation2,
                                                        reflector);

    return status;
}