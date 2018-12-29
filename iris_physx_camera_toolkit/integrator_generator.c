/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    integrator_generator.c

Abstract:

    Interface for integrator generators.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx_camera_toolkit/integrator_generator.h"
#include "iris_physx_camera_toolkit/integrator_generator_internal.h"

//
// Functions
//

ISTATUS
IntegratorGeneratorAllocate(
    _In_ PCINTEGRATOR_GENERATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PINTEGRATOR_GENERATOR *integrator_generator
    )
{
    if (vtable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (data_size != 0)
    {
        if (data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
        
        if (data_alignment == 0 ||
            (data_alignment & (data_alignment - 1)) != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;    
        }
        
        if (data_size % data_alignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (integrator_generator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(INTEGRATOR_GENERATOR),
                                          alignof(INTEGRATOR_GENERATOR),
                                          (void **)integrator_generator,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*integrator_generator)->vtable = vtable;
    (*integrator_generator)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

void
IntegratorGeneratorFree(
    _In_opt_ _Post_invalid_ PINTEGRATOR_GENERATOR integrator_generator
    )
{
    if (integrator_generator == NULL)
    {
        return;
    }

    if (integrator_generator->vtable->free_routine)
    {
        integrator_generator->vtable->free_routine(
            integrator_generator->data);
    }

    free(integrator_generator);
}