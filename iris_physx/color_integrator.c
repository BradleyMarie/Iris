/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    color_integrator.c

Abstract:

    Interface representing a color matcher.

--*/

#include "common/safe_math.h"
#include "iris_physx/color_integrator.h"
#include "iris_physx/color_integrator_internal.h"

//
// Defines
//

#define INITIAL_LIST_SIZE 16
#define LIST_GROWTH_FACTOR 2

//
// Functions
//

ISTATUS
ColorIntegratorAllocate(
    _In_ PCOLOR_INTEGRATOR_COMPUTE_SPECTRUM_COLOR_ROUTINE compute_spectrum_color_routine,
    _In_ PCOLOR_INTEGRATOR_COMPUTE_REFLECTOR_COLOR_ROUTINE compute_reflector_color_routine,
    _Out_ PCOLOR_INTEGRATOR *color_integrator
    )
{
    if (compute_spectrum_color_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (compute_reflector_color_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    PCOLOR_INTEGRATOR result =
        (PCOLOR_INTEGRATOR)malloc(sizeof(COLOR_INTEGRATOR));
    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    result->reflector_list = 
        (PREFLECTOR_LIST_ENTRY)calloc(INITIAL_LIST_SIZE, sizeof(REFLECTOR_LIST_ENTRY));
    if (result->reflector_list == NULL)
    {
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    result->spectrum_list = 
        (PSPECTRUM_LIST_ENTRY)calloc(INITIAL_LIST_SIZE, sizeof(SPECTRUM_LIST_ENTRY));
    if (result->spectrum_list == NULL)
    {
        free(result->reflector_list);
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    result->compute_reflector_color_routine = compute_reflector_color_routine;
    result->compute_spectrum_color_routine = compute_spectrum_color_routine;
    result->reflector_list_capacity = INITIAL_LIST_SIZE;
    result->reflector_list_size = 0;
    result->spectrum_list_capacity = INITIAL_LIST_SIZE;
    result->spectrum_list_size = 0;

    *color_integrator = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorIntegratorPrecomputeSpectrumColor(
    _In_ PCOLOR_INTEGRATOR color_integrator,
    _In_opt_ PSPECTRUM spectrum
    )
{
    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    PCSPECTRUM_LIST_ENTRY old_entry =
        ColorIntegratorFindSpectrumEntry(color_integrator, spectrum);
    if (old_entry != NULL)
    {
        return ISTATUS_SUCCESS;
    }

    if (color_integrator->spectrum_list_size == color_integrator->spectrum_list_capacity)
    {
        size_t new_capacity;
        bool success = CheckedMultiplySizeT(color_integrator->spectrum_list_capacity,
                                            LIST_GROWTH_FACTOR,
                                            &new_capacity);
        
        if (!success)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        size_t new_capacity_in_bytes;
        success = CheckedMultiplySizeT(new_capacity,
                                       sizeof(SPECTRUM_LIST_ENTRY),
                                       &new_capacity_in_bytes);
        
        if (!success)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        void *new_list = realloc((void *)color_integrator->spectrum_list,
                                 new_capacity_in_bytes);

        if (new_list == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        color_integrator->spectrum_list = (PSPECTRUM_LIST_ENTRY)new_list;
        color_integrator->spectrum_list_capacity = new_capacity;
    }

    PSPECTRUM_LIST_ENTRY entry =
        color_integrator->spectrum_list + color_integrator->spectrum_list_size;

    ISTATUS status =
        color_integrator->compute_spectrum_color_routine(spectrum, &entry->color);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    entry->spectrum = spectrum;
    color_integrator->spectrum_list_size += 1;

    qsort(color_integrator->spectrum_list,
          color_integrator->spectrum_list_size,
          sizeof(SPECTRUM_LIST_ENTRY),
          ColorIntegratorCompareSpectrumEntry);

    SpectrumRetain(spectrum);

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorIntegratorPrecomputeReflectorColor(
    _In_ PCOLOR_INTEGRATOR color_integrator,
    _In_opt_ PREFLECTOR reflector
    )
{
    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflector == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    PCREFLECTOR_LIST_ENTRY old_entry =
        ColorIntegratorFindReflectorEntry(color_integrator, reflector);
    if (old_entry != NULL)
    {
        return ISTATUS_SUCCESS;
    }

    if (color_integrator->reflector_list_size == color_integrator->reflector_list_capacity)
    {
        size_t new_capacity;
        bool success = CheckedMultiplySizeT(color_integrator->reflector_list_capacity,
                                            LIST_GROWTH_FACTOR,
                                            &new_capacity);
        
        if (!success)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        size_t new_capacity_in_bytes;
        success = CheckedMultiplySizeT(new_capacity,
                                       sizeof(REFLECTOR_LIST_ENTRY),
                                       &new_capacity_in_bytes);
        
        if (!success)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        void *new_list = realloc((void *)color_integrator->reflector_list,
                                 new_capacity_in_bytes);

        if (new_list == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        color_integrator->reflector_list = (PREFLECTOR_LIST_ENTRY)new_list;
        color_integrator->reflector_list_capacity = new_capacity;
    }

    PREFLECTOR_LIST_ENTRY entry =
        color_integrator->reflector_list + color_integrator->reflector_list_size;

    ISTATUS status =
        color_integrator->compute_reflector_color_routine(reflector, &entry->color);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    entry->reflector = reflector;
    color_integrator->reflector_list_size += 1;

    qsort(color_integrator->reflector_list,
          color_integrator->reflector_list_size,
          sizeof(REFLECTOR_LIST_ENTRY),
          ColorIntegratorCompareReflectorEntry);

    ReflectorRetain(reflector);

    return ISTATUS_SUCCESS;
}

void
ColorIntegratorFree(
    _In_opt_ _Post_invalid_ PCOLOR_INTEGRATOR color_integrator
    )
{
    if (color_integrator == NULL)
    {
        return;
    }

    for (size_t i = 0; i < color_integrator->reflector_list_size; i++)
    {
        ReflectorRelease(color_integrator->reflector_list[i].reflector);
    }

    free(color_integrator->reflector_list);

    for (size_t i = 0; i < color_integrator->spectrum_list_size; i++)
    {
        SpectrumRelease(color_integrator->spectrum_list[i].spectrum);
    }

    free(color_integrator->spectrum_list);
    free(color_integrator);
}