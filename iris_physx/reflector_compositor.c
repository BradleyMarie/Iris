/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    reflector_compositor.c

Abstract:

    Allows allocation of reflectors cheaply during shading.

--*/

#include "iris_physx/reflector_compositor.h"

#include <assert.h>
#include <string.h>

#include "iris_physx/reflector_compositor_internal.h"

//
// Static Functions
//

static
ISTATUS
AttenuatedReflectorReflect(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    )
{
    assert(context != NULL);
    assert(isfinite(wavelength));
    assert((float_t)0.0 < wavelength);
    assert(reflectance != NULL);

    PCATTENUATED_REFLECTOR attenuated_reflector =
        (PCATTENUATED_REFLECTOR)context;

    float_t output_reflectance;
    ISTATUS Status = ReflectorReflectInline(attenuated_reflector->reflector,
                                            wavelength,
                                            &output_reflectance);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *reflectance = output_reflectance * attenuated_reflector->attenuation;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AttenuatedReflectionReflectorReflect(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    )
{
    assert(context != NULL);
    assert(isfinite(wavelength));
    assert((float_t)0.0 < wavelength);
    assert(reflectance != NULL);

    PCATTENUATED_SUM_REFLECTOR reflector = (PCATTENUATED_SUM_REFLECTOR)context;

    float_t added_reflectance;
    ISTATUS status = ReflectorReflectInline(reflector->added_reflector,
                                            wavelength,
                                            &added_reflectance);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t attenuated_reflectance;
    status = ReflectorReflectInline(reflector->attenuated_reflector,
                                    wavelength,
                                    &attenuated_reflectance);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *reflectance = fma(reflector->attenuation,
                       attenuated_reflectance,
                       added_reflectance);

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

const static REFLECTOR_VTABLE attenuated_reflector_vtable = {
    AttenuatedReflectorReflect,
    NULL
};

const static REFLECTOR_VTABLE attenuated_sum_reflector_vtable = {
    AttenuatedReflectionReflectorReflect,
    NULL
};

//
// Initialization Functions
//

static
inline
ISTATUS
AttenuatedReflectorAllocate(
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _In_ PCREFLECTOR reflector,
    _In_ float_t attenuation,
    _Out_ PCREFLECTOR *attenuated_reflector
    )
{
    assert(compositor != NULL);
    assert(reflector != NULL);
    assert(isfinite(attenuation));
    assert((float_t)0.0 < attenuation);
    assert(attenuated_reflector != NULL);

    if (attenuation == (float_t)1.0)
    {
        *attenuated_reflector = reflector;
        return ISTATUS_SUCCESS;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->attenuated_reflector_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PATTENUATED_REFLECTOR allocated_reflector =
        (PATTENUATED_REFLECTOR)allocation;

    ReflectorInitialize(&allocated_reflector->header,
                        &attenuated_reflector_vtable,
                        allocated_reflector);

    allocated_reflector->reflector = reflector;
    allocated_reflector->attenuation = attenuation;

    *attenuated_reflector = &allocated_reflector->header;

    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
ReflectorCompositorAttenuateReflector(
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _In_ PCREFLECTOR reflector,
    _In_ float_t attenuation,
    _Out_ PCREFLECTOR *attenuated_reflector
    )
{
    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if(!isfinite(attenuation) || attenuation < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (attenuated_reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (reflector == NULL || attenuation == (float_t)0.0)
    {
        *attenuated_reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    if (reflector->vtable == &attenuated_reflector_vtable)
    {
        PCATTENUATED_REFLECTOR reflector0 =
            (PCATTENUATED_REFLECTOR) reflector;

        reflector = reflector0->reflector;
        attenuation *= reflector0->attenuation;
    }

    ISTATUS status = AttenuatedReflectorAllocate(compositor,
                                                 reflector,
                                                 attenuation,
                                                 attenuated_reflector);

    return status;
}

ISTATUS
ReflectorCompositorAttenuatedAddReflectors(
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _In_ PCREFLECTOR added_reflector,
    _In_ PCREFLECTOR attenuated_reflector,
    _In_ float_t attenuation,
    _Out_ PCREFLECTOR *result
    )
{
    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if(!isfinite(attenuation) || attenuation < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (added_reflector == NULL)
    {
        if (attenuated_reflector == NULL || attenuation == (float_t)0.0)
        {
            *result = NULL;
            return ISTATUS_SUCCESS;
        }

        ISTATUS status = AttenuatedReflectorAllocate(compositor,
                                                     attenuated_reflector,
                                                     attenuation,
                                                     result);

        return status;
    }

    if (attenuated_reflector == NULL || attenuation == (float_t)0.0)
    {
        *result = added_reflector;
        return ISTATUS_SUCCESS;
    }

    if (attenuated_reflector->vtable == &attenuated_reflector_vtable)
    {
        PCATTENUATED_REFLECTOR reflector0 =
            (PCATTENUATED_REFLECTOR)attenuated_reflector;

        attenuated_reflector = reflector0->reflector;
        attenuation *= reflector0->attenuation;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->attenuated_sum_reflector_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PATTENUATED_SUM_REFLECTOR allocated_reflector =
        (PATTENUATED_SUM_REFLECTOR)allocation;

    ReflectorInitialize(&allocated_reflector->header,
                        &attenuated_sum_reflector_vtable,
                        allocated_reflector);

    allocated_reflector->added_reflector = added_reflector;
    allocated_reflector->attenuated_reflector = attenuated_reflector;
    allocated_reflector->attenuation = attenuation;

    *result = &allocated_reflector->header;

    return ISTATUS_SUCCESS;
}