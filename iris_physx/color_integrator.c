/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    color_integrator.c

Abstract:

    Interface representing a color matcher.

--*/

#include <stdalign.h>

#include "common/alloc.h"
#include "common/safe_math.h"
#include "iris_physx/color_integrator.h"
#include "iris_physx/color_integrator_internal.h"

//
// Defines
//

#define INITIAL_LIST_SIZE 16
#define LIST_GROWTH_FACTOR 2

//
// Static Functions
//

static
inline
void
ColorIntegratorInsertSpectrum(
    _Inout_updates_(list_size) PSPECTRUM_LIST_ENTRY list,
    _In_ size_t list_size,
    _In_ PSPECTRUM spectrum,
    _In_ COLOR3 color
    )
{
    if (spectrum == NULL)
    {
        return;
    }

    size_t index = ColorIntegratorSpectrumProbeStart(list_size, spectrum);
    for (;;) {
        if (list[index].spectrum == spectrum)
        {
            return;
        }

        if (list[index].spectrum == NULL)
        {
            list[index].spectrum = spectrum;
            list[index].color = color;
            return;
        }

        index += 1;

        if (index == list_size) {
            index = 0;
        }
    }
}

static
inline
void
ColorIntegratorInsertReflector(
    _Inout_updates_(list_size) PREFLECTOR_LIST_ENTRY list,
    _In_ size_t list_size,
    _In_ PREFLECTOR reflector,
    _In_ COLOR3 color
    )
{
    if (reflector == NULL)
    {
        return;
    }

    size_t index = ColorIntegratorReflectorProbeStart(list_size, reflector);
    for (;;) {
        if (list[index].reflector == reflector)
        {
            return;
        }

        if (list[index].reflector == NULL)
        {
            list[index].reflector = reflector;
            list[index].color = color;
            return;
        }

        index += 1;

        if (index == list_size) {
            index = 0;
        }
    }
}

static
ISTATUS
ColorIntegratorGrowSpectrumHashTable(
    _Inout_ PCOLOR_INTEGRATOR color_integrator
    )
{
    size_t new_capacity;
    bool success = CheckedMultiplySizeT(color_integrator->spectrum_list_capacity,
                                        LIST_GROWTH_FACTOR,
                                        &new_capacity);
    
    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PSPECTRUM_LIST_ENTRY new_list =
        (PSPECTRUM_LIST_ENTRY)calloc(new_capacity, sizeof(SPECTRUM_LIST_ENTRY));

    if (new_list == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < color_integrator->spectrum_list_capacity; i++)
    {
        ColorIntegratorInsertSpectrum(new_list,
                                      new_capacity,
                                      color_integrator->spectrum_list[i].spectrum,
                                      color_integrator->spectrum_list[i].color);
    }

    free(color_integrator->spectrum_list);

    color_integrator->spectrum_list = new_list;
    color_integrator->spectrum_list_capacity = new_capacity;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
ColorIntegratorGrowReflectorHashTable(
    _Inout_ PCOLOR_INTEGRATOR color_integrator
    )
{
    size_t new_capacity;
    bool success = CheckedMultiplySizeT(color_integrator->reflector_list_capacity,
                                        LIST_GROWTH_FACTOR,
                                        &new_capacity);
    
    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PREFLECTOR_LIST_ENTRY new_list =
        (PREFLECTOR_LIST_ENTRY)calloc(new_capacity, sizeof(REFLECTOR_LIST_ENTRY));

    if (new_list == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < color_integrator->reflector_list_capacity; i++)
    {
        ColorIntegratorInsertReflector(new_list,
                                       new_capacity,
                                       color_integrator->reflector_list[i].reflector,
                                       color_integrator->reflector_list[i].color);
    }

    free(color_integrator->reflector_list);

    color_integrator->reflector_list = new_list;
    color_integrator->reflector_list_capacity = new_capacity;

    return ISTATUS_SUCCESS;
}

static
inline
size_t
ColorIntegratorAvailableSpectrumSlots(
    _In_ PCCOLOR_INTEGRATOR color_integrator
    )
{
    size_t usable_slots = color_integrator->spectrum_list_capacity;
    if (usable_slots < 8)
    {
        usable_slots = usable_slots - 1;
    }
    else 
    {
        usable_slots = usable_slots - usable_slots / 8;
    }

    return usable_slots - color_integrator->spectrum_list_size;
}

static
inline
size_t
ColorIntegratorAvailableReflectorSlots(
    _In_ PCCOLOR_INTEGRATOR color_integrator
    )
{
    size_t usable_slots = color_integrator->reflector_list_capacity;
    if (usable_slots < 8)
    {
        usable_slots = usable_slots - 1;
    }
    else 
    {
        usable_slots = usable_slots - usable_slots / 8;
    }

    return usable_slots - color_integrator->reflector_list_size;
}

//
// Functions
//

ISTATUS
ColorIntegratorAllocate(
    _In_ PCCOLOR_INTEGRATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCOLOR_INTEGRATOR *color_integrator
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

    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(COLOR_INTEGRATOR),
                                          alignof(COLOR_INTEGRATOR),
                                          (void **)color_integrator,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*color_integrator)->reflector_list =
        (PREFLECTOR_LIST_ENTRY)calloc(INITIAL_LIST_SIZE, sizeof(REFLECTOR_LIST_ENTRY));
    if ((*color_integrator)->reflector_list == NULL)
    {
        free(*color_integrator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*color_integrator)->spectrum_list =
        (PSPECTRUM_LIST_ENTRY)calloc(INITIAL_LIST_SIZE, sizeof(SPECTRUM_LIST_ENTRY));
    if ((*color_integrator)->spectrum_list == NULL)
    {
        free((*color_integrator)->reflector_list);
        free(*color_integrator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*color_integrator)->vtable = vtable;
    (*color_integrator)->reflector_list_capacity = INITIAL_LIST_SIZE;
    (*color_integrator)->reflector_list_size = 0;
    (*color_integrator)->spectrum_list_capacity = INITIAL_LIST_SIZE;
    (*color_integrator)->spectrum_list_size = 0;
    (*color_integrator)->data = data_allocation;

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorIntegratorCacheSpectrum(
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

    PCSPECTRUM_LIST_ENTRY entry;
    bool found = ColorIntegratorFindSpectrumEntry(color_integrator,
                                                  spectrum,
                                                  &entry);

    if (found)
    {
        return ISTATUS_SUCCESS;
    }

    if (ColorIntegratorAvailableSpectrumSlots(color_integrator) == 0)
    {
        ISTATUS status = ColorIntegratorGrowSpectrumHashTable(color_integrator);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    COLOR3 color;
    ISTATUS status =
        color_integrator->vtable->compute_spectrum_color_routine(
            color_integrator->data, spectrum, &color);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ColorIntegratorInsertSpectrum(color_integrator->spectrum_list,
                                  color_integrator->spectrum_list_capacity,
                                  spectrum,
                                  color);

    SpectrumRetain(spectrum);

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorIntegratorCacheReflector(
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

    PCREFLECTOR_LIST_ENTRY entry;
    bool found = ColorIntegratorFindReflectorEntry(color_integrator,
                                                   reflector,
                                                   &entry);

    if (found)
    {
        return ISTATUS_SUCCESS;
    }

    if (ColorIntegratorAvailableReflectorSlots(color_integrator) == 0)
    {
        ISTATUS status = ColorIntegratorGrowReflectorHashTable(color_integrator);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    COLOR3 color;
    ISTATUS status =
        color_integrator->vtable->compute_reflector_color_routine(
            color_integrator->data, reflector, &color);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ColorIntegratorInsertReflector(color_integrator->reflector_list,
                                   color_integrator->reflector_list_capacity,
                                   reflector,
                                   color);

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

    for (size_t i = 0; i < color_integrator->reflector_list_capacity; i++)
    {
        ReflectorRelease(color_integrator->reflector_list[i].reflector);
    }

    free(color_integrator->reflector_list);

    for (size_t i = 0; i < color_integrator->spectrum_list_capacity; i++)
    {
        SpectrumRelease(color_integrator->spectrum_list[i].spectrum);
    }

    free(color_integrator->spectrum_list);

    if (color_integrator->vtable->free_routine)
    {
        color_integrator->vtable->free_routine(color_integrator->data);
    }

    free(color_integrator);
}