/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    color_integrator.c

Abstract:

    Interface representing a color matcher.

--*/

#include <stdlib.h>

#include "common/safe_math.h"
#include "iris_physx/color_integrator.h"

//
// Defines
//

#define INITIAL_LIST_SIZE 16
#define LIST_GROWTH_FACTOR 2

//
// Types
//

typedef struct _REFLECTOR_LIST_ENTRY {
    PREFLECTOR reflector;
    COLOR3 color;
} REFLECTOR_LIST_ENTRY, *PREFLECTOR_LIST_ENTRY;

typedef const REFLECTOR_LIST_ENTRY *PCREFLECTOR_LIST_ENTRY;

typedef struct _SPECTRUM_LIST_ENTRY {
    PSPECTRUM spectrum;
    COLOR3 color;
} SPECTRUM_LIST_ENTRY, *PSPECTRUM_LIST_ENTRY;

typedef const SPECTRUM_LIST_ENTRY *PCSPECTRUM_LIST_ENTRY;

struct _COLOR_INTEGRATOR {
    PCOLOR_INTEGRATOR_COMPUTE_SPECTRUM_COLOR_ROUTINE compute_spectrum_color_routine;
    PCOLOR_INTEGRATOR_COMPUTE_REFLECTOR_COLOR_ROUTINE compute_reflector_color_routine;
    _Field_size_(reflector_list_capacity) PREFLECTOR_LIST_ENTRY reflector_list;
    _Field_size_(spectrum_list_capacity) PSPECTRUM_LIST_ENTRY spectrum_list;
    size_t reflector_list_capacity;
    size_t reflector_list_size;
    size_t spectrum_list_capacity;
    size_t spectrum_list_size;
};

//
// Static Functions
//

static
int
ColorIntegratorCompareSpectrumEntry(
    _In_ const void* left,
    _In_ const void* right
    )
{
    PCSPECTRUM_LIST_ENTRY left_entry = (PCSPECTRUM_LIST_ENTRY)left;
    PCSPECTRUM_LIST_ENTRY right_entry = (PCSPECTRUM_LIST_ENTRY)right;

    if (left_entry->spectrum < right_entry->spectrum)
    {
        return -1;
    }

    if (left_entry->spectrum > right_entry->spectrum)
    {
        return 1;
    }

    return 0;
}

static
int
ColorIntegratorCompareReflectorEntry(
    _In_ const void* left,
    _In_ const void* right
    )
{
    PCREFLECTOR_LIST_ENTRY left_entry = (PCREFLECTOR_LIST_ENTRY)left;
    PCREFLECTOR_LIST_ENTRY right_entry = (PCREFLECTOR_LIST_ENTRY)right;

    if (left_entry->reflector < right_entry->reflector)
    {
        return -1;
    }

    if (left_entry->reflector > right_entry->reflector)
    {
        return 1;
    }

    return 0;
}

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