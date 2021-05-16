/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    reflector_compositor.c

Abstract:

    Allows composition of reflectors cheaply during shading.

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
AttenuatedReflectorGetAlbedo(
    _In_ const void *context,
    _Out_ float_t *reflectance
    )
{
    PCATTENUATED_REFLECTOR attenuated_reflector =
        (PCATTENUATED_REFLECTOR)context;

    ISTATUS Status = ReflectorGetAlbedoInline(attenuated_reflector->reflector,
                                              reflectance);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *reflectance *= attenuated_reflector->attenuation;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AttenuatedSumReflectorReflect(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    )
{
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

    *reflectance =
        added_reflectance + attenuated_reflectance * reflector->attenuation;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AttenuatedSumReflectorGetAlbedo(
    _In_ const void *context,
    _Out_ float_t *reflectance
    )
{
    PCATTENUATED_SUM_REFLECTOR reflector = (PCATTENUATED_SUM_REFLECTOR)context;

    float_t added_albedo;
    ISTATUS status = ReflectorGetAlbedoInline(reflector->added_reflector,
                                              &added_albedo);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorGetAlbedoInline(reflector->attenuated_reflector,
                                      reflectance);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *reflectance =
        added_albedo + *reflectance * reflector->attenuation;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
ProductReflectorReflect(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    )
{
    PCPRODUCT_REFLECTOR reflector = (PCPRODUCT_REFLECTOR)context;

    float_t multiplicand;
    ISTATUS status = ReflectorReflectInline(reflector->multiplicand0,
                                            wavelength,
                                            &multiplicand);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorReflectInline(reflector->multiplicand1,
                                    wavelength,
                                    reflectance);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *reflectance *= multiplicand;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
ProductReflectorGetAlbedo(
    _In_ const void *context,
    _Out_ float_t *reflectance
    )
{
    PCPRODUCT_REFLECTOR reflector = (PCPRODUCT_REFLECTOR)context;

    float_t multiplicand;
    ISTATUS status = ReflectorGetAlbedoInline(reflector->multiplicand0,
                                              &multiplicand);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorGetAlbedoInline(reflector->multiplicand1,
                                      reflectance);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *reflectance *= multiplicand;

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

const static REFLECTOR_VTABLE attenuated_reflector_vtable = {
    AttenuatedReflectorReflect,
    AttenuatedReflectorGetAlbedo,
    NULL
};

const static REFLECTOR_VTABLE attenuated_sum_reflector_vtable = {
    AttenuatedSumReflectorReflect,
    AttenuatedSumReflectorGetAlbedo,
    NULL
};

const static REFLECTOR_VTABLE product_reflector_vtable = {
    ProductReflectorReflect,
    ProductReflectorGetAlbedo,
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
    assert((float_t)0.0 <= attenuation);
    assert(attenuated_reflector != NULL);

    if (attenuation == (float_t)0.0)
    {
        *attenuated_reflector = NULL;
        return ISTATUS_SUCCESS;
    }

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

    InternalReflectorInitialize(&allocated_reflector->header,
                                &attenuated_reflector_vtable,
                                allocated_reflector,
                                ATTENUATED_REFLECTOR_TYPE);

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

    if (reflector == NULL)
    {
        *attenuated_reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    if (reflector->vtable == (const void*)&attenuated_reflector_vtable)
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
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (added_reflector == NULL)
    {
        if (attenuated_reflector == NULL)
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

    if (attenuated_reflector == NULL)
    {
        *result = added_reflector;
        return ISTATUS_SUCCESS;
    }

    if (attenuated_reflector->vtable == (const void*)&attenuated_reflector_vtable)
    {
        PCATTENUATED_REFLECTOR reflector0 =
            (PCATTENUATED_REFLECTOR)attenuated_reflector;

        attenuated_reflector = reflector0->reflector;
        attenuation *= reflector0->attenuation;
    }

    if (attenuation == (float_t)0.0)
    {
        *result = added_reflector;
        return ISTATUS_SUCCESS;
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

    InternalReflectorInitialize(&allocated_reflector->header,
                                &attenuated_sum_reflector_vtable,
                                allocated_reflector,
                                ATTENUATED_SUM_REFLECTOR_TYPE);

    allocated_reflector->added_reflector = added_reflector;
    allocated_reflector->attenuated_reflector = attenuated_reflector;
    allocated_reflector->attenuation = attenuation;

    *result = &allocated_reflector->header;

    return ISTATUS_SUCCESS;
}

ISTATUS
ReflectorCompositorMultiplyReflectors(
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _In_ PCREFLECTOR multiplicand0,
    _In_ PCREFLECTOR multiplicand1,
    _Out_ PCREFLECTOR *product
    )
{
    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (product == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (multiplicand0 == NULL || multiplicand1 == NULL)
    {
        *product = NULL;
        return ISTATUS_SUCCESS;
    }

    float_t attenuation;
    if (multiplicand0->vtable == (const void*)&attenuated_reflector_vtable &&
        multiplicand1->vtable == (const void*)&attenuated_reflector_vtable)
    {
        PCATTENUATED_REFLECTOR reflector0 =
            (PCATTENUATED_REFLECTOR) multiplicand0;
        PCATTENUATED_REFLECTOR reflector1 =
            (PCATTENUATED_REFLECTOR) multiplicand1;

        attenuation = reflector0->attenuation * reflector1->attenuation;
        multiplicand0 = reflector0->reflector;
        multiplicand1 = reflector1->reflector;
    }
    else if (multiplicand0->vtable == (const void*)&attenuated_reflector_vtable)
    {
        PCATTENUATED_REFLECTOR reflector =
            (PCATTENUATED_REFLECTOR) multiplicand0;

        attenuation = reflector->attenuation;
        multiplicand0 = reflector->reflector;
    }
    else if (multiplicand1->vtable == (const void*)&attenuated_reflector_vtable)
    {
        PCATTENUATED_REFLECTOR reflector =
            (PCATTENUATED_REFLECTOR) multiplicand1;

        attenuation = reflector->attenuation;
        multiplicand1 = reflector->reflector;
    }
    else
    {
        attenuation = (float_t)1.0;
    }

    void *allocation;
    bool success = StaticMemoryAllocatorAllocate(
        &compositor->product_reflector_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PPRODUCT_REFLECTOR allocated_product =
        (PPRODUCT_REFLECTOR)allocation;

    InternalReflectorInitialize(&allocated_product->header,
                                &product_reflector_vtable,
                                allocated_product,
                                PRODUCT_REFLECTOR_TYPE);

    allocated_product->multiplicand0 = multiplicand0;
    allocated_product->multiplicand1 = multiplicand1;

    if (attenuation == (float_t)1.0)
    {
        *product = &allocated_product->header;
        return ISTATUS_SUCCESS;
    }

    success = StaticMemoryAllocatorAllocate(
        &compositor->attenuated_reflector_allocator, &allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PATTENUATED_REFLECTOR allocated_attenuated_reflector =
        (PATTENUATED_REFLECTOR)allocation;

    InternalReflectorInitialize(&allocated_attenuated_reflector->header,
                                &attenuated_reflector_vtable,
                                allocated_attenuated_reflector,
                                ATTENUATED_REFLECTOR_TYPE);

    allocated_attenuated_reflector->reflector = &allocated_product->header;
    allocated_attenuated_reflector->attenuation = attenuation;

    *product = &allocated_attenuated_reflector->header;

    return ISTATUS_SUCCESS;
}

ISTATUS
ReflectorCompositorAddReflectors(
    _Inout_ PREFLECTOR_COMPOSITOR compositor,
    _In_ PCREFLECTOR addend0,
    _In_ PCREFLECTOR addend1,
    _Out_ PCREFLECTOR *sum
    )
{
    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (sum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    // TODO: Implement an actual sum reflector if this ends up being hot.
    ISTATUS status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                                addend0,
                                                                addend1,
                                                                (float_t)1.0,
                                                                sum);

    return status;
}