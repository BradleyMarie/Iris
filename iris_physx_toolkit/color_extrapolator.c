/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    color_extrapolator.c

Abstract:

    Interface representing a color extrapolator.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "common/safe_math.h"
#include "iris_physx_toolkit/color_extrapolator.h"
#include "third_party/smhasher/MurmurHash2.h"
#include "third_party/smhasher/MurmurHash3.h"

//
// Defines
//

#define INITIAL_LIST_SIZE 16
#define LIST_GROWTH_FACTOR 2
#define HASH_SEED 0

//
// Types
//

typedef struct _REFLECTOR_LIST_ENTRY {
    COLOR3 color;
    PREFLECTOR reflector;
} REFLECTOR_LIST_ENTRY, *PREFLECTOR_LIST_ENTRY;

typedef const REFLECTOR_LIST_ENTRY *PCREFLECTOR_LIST_ENTRY;

typedef struct _SPECTRUM_LIST_ENTRY {
    COLOR3 color;
    PSPECTRUM spectrum;
} SPECTRUM_LIST_ENTRY, *PSPECTRUM_LIST_ENTRY;

typedef const SPECTRUM_LIST_ENTRY *PCSPECTRUM_LIST_ENTRY;

struct _COLOR_EXTRAPOLATOR {
    PCCOLOR_EXTRAPOLATOR_VTABLE vtable;
    _Field_size_full_(reflector_list_capacity) PREFLECTOR_LIST_ENTRY reflector_list;
    _Field_size_full_(spectrum_list_capacity) PSPECTRUM_LIST_ENTRY spectrum_list;
    size_t reflector_list_capacity;
    size_t reflector_list_usable_capacity;
    size_t reflector_list_size;
    size_t spectrum_list_capacity;
    size_t spectrum_list_usable_capacity;
    size_t spectrum_list_size;
    void *data;
};

//
// Static Functions
//

static
inline
size_t
ColorExtrapolatorComputeUsableCapacity(
    _In_ size_t capacity
    )
{
    assert(capacity >= 4);

    return capacity - capacity / 4;
}

static
inline
bool
ColorExtrapolatorSpectrumListFull(
    _In_ PCCOLOR_EXTRAPOLATOR color_extrapolator
    )
{
    size_t size = color_extrapolator->spectrum_list_size;
    size_t capacity = color_extrapolator->spectrum_list_usable_capacity;
    return size == capacity;
}

static
inline
bool
ColorExtrapolatorReflectorListFull(
    _In_ PCCOLOR_EXTRAPOLATOR color_extrapolator
    )
{
    size_t size = color_extrapolator->reflector_list_size;
    size_t capacity = color_extrapolator->reflector_list_usable_capacity;
    return size == capacity;
}

static
inline
void
ColorExtrapolatorSetSpectrumEntryEmpty(
    _Inout_ PSPECTRUM_LIST_ENTRY entry
    )
{
    entry->color.color_space = (COLOR_SPACE)-1;
}

static
inline
void
ColorExtrapolatorSetReflectorEntryEmpty(
    _Inout_ PREFLECTOR_LIST_ENTRY entry
    )
{
    entry->color.color_space = (COLOR_SPACE)-1;
}

static
inline
bool
ColorExtrapolatorIsSpectrumEntryEmpty(
    _In_ PCSPECTRUM_LIST_ENTRY entry
    )
{
    return entry->color.color_space == (COLOR_SPACE)-1;
}

static
inline
bool
ColorExtrapolatorIsReflectorEntryEmpty(
    _In_ PCREFLECTOR_LIST_ENTRY entry
    )
{
    return entry->color.color_space == (COLOR_SPACE)-1;
}

static
inline
size_t
ColorExtrapolatorSpectrumProbeStart(
    _In_ size_t list_capacity,
    _In_ COLOR3 color
    )
{
    size_t hash;
    if (sizeof(PCSPECTRUM) == 4)
    {
        MurmurHash3_x86_32(&color, sizeof(COLOR3), HASH_SEED, &hash);
    }
    else if (sizeof(PCSPECTRUM) == 8)
    {
        hash = MurmurHash64A(&color, sizeof(COLOR3), HASH_SEED);
    }
    else
    {
        assert(false);
        hash = 0;
    }

    return hash % list_capacity;
}

static
inline
size_t
ColorExtrapolatorReflectorProbeStart(
    _In_ size_t list_capacity,
    _In_ COLOR3 color
    )
{
    size_t hash;
    if (sizeof(PCREFLECTOR) == 4)
    {
        MurmurHash3_x86_32(&color, sizeof(COLOR3), HASH_SEED, &hash);
    }
    else if (sizeof(PCREFLECTOR) == 8)
    {
        hash = MurmurHash64A(&color, sizeof(COLOR3), HASH_SEED);
    }
    else
    {
        assert(false);
        hash = 0;
    }

    return hash % list_capacity;
}

static
inline
bool
ColorExtrapolatorFindSpectrum(
    _In_ PCCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ COLOR3 color,
    _Out_ size_t *index
    )
{
    *index = ColorExtrapolatorSpectrumProbeStart(
        color_extrapolator->spectrum_list_capacity, color);
    for (;;)
    {
        if (ColorExtrapolatorIsSpectrumEntryEmpty(color_extrapolator->spectrum_list + *index))
        {
            return false;
        }

        if (memcmp(&color_extrapolator->spectrum_list[*index].color,
                   &color,
                   sizeof(COLOR3)) == 0)
        {
            return true;
        }

        *index += 1;

        if (*index == color_extrapolator->spectrum_list_capacity)
        {
            *index = 0;
        }
    }

    assert(false);
    return false;
}

static
inline
bool
ColorExtrapolatorFindReflector(
    _In_ PCCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ COLOR3 color,
    _Out_ size_t *index
    )
{
    *index = ColorExtrapolatorReflectorProbeStart(
        color_extrapolator->reflector_list_capacity, color);
    for (;;)
    {
        if (ColorExtrapolatorIsReflectorEntryEmpty(color_extrapolator->reflector_list + *index))
        {
            return false;
        }

        if (memcmp(&color_extrapolator->reflector_list[*index].color,
                   &color,
                   sizeof(COLOR3)) == 0)
        {
            return true;
        }

        *index += 1;

        if (*index == color_extrapolator->reflector_list_capacity)
        {
            *index = 0;
        }
    }

    assert(false);
    return false;
}

static
inline
bool
ColorExtrapolatorFindSpectrumEntry(
    _In_ PCCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ COLOR3 color,
    _Out_ PSPECTRUM_LIST_ENTRY *entry
    )
{
    size_t index;
    bool result = ColorExtrapolatorFindSpectrum(color_extrapolator,
                                                color,
                                                &index);

    *entry = color_extrapolator->spectrum_list + index;

    return result;
}

static
inline
bool
ColorExtrapolatorFindReflectorEntry(
    _In_ PCCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ COLOR3 color,
    _Out_ PREFLECTOR_LIST_ENTRY *entry
    )
{
    size_t index;
    bool result = ColorExtrapolatorFindReflector(color_extrapolator,
                                                 color,
                                                 &index);

    *entry = color_extrapolator->reflector_list + index;

    return result;
}

static
inline
void
ColorExtrapolatorInsertSpectrum(
    _Inout_updates_(list_size) PSPECTRUM_LIST_ENTRY list,
    _In_ size_t list_size,
    _In_ COLOR3 color,
    _In_ PSPECTRUM spectrum
    )
{
    size_t index = ColorExtrapolatorSpectrumProbeStart(list_size, color);
    for (;;)
    {
        if (ColorExtrapolatorIsSpectrumEntryEmpty(list + index))
        {
            list[index].color = color;
            list[index].spectrum = spectrum;
            return;
        }

        index += 1;

        if (index == list_size)
        {
            index = 0;
        }
    }
}

static
inline
void
ColorExtrapolatorInsertReflector(
    _Inout_updates_(list_size) PREFLECTOR_LIST_ENTRY list,
    _In_ size_t list_size,
    _In_ COLOR3 color,
    _In_ PREFLECTOR reflector
    )
{
    size_t index = ColorExtrapolatorReflectorProbeStart(list_size, color);
    for (;;)
    {
        if (ColorExtrapolatorIsReflectorEntryEmpty(list + index))
        {
            list[index].color = color;
            list[index].reflector = reflector;
            return;
        }

        index += 1;

        if (index == list_size)
        {
            index = 0;
        }
    }
}

static
inline
ISTATUS
ColorExtrapolatorResizeSpectrumHashTable(
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ size_t new_capacity
    )
{
    PSPECTRUM_LIST_ENTRY new_list =
        (PSPECTRUM_LIST_ENTRY)calloc(new_capacity, sizeof(SPECTRUM_LIST_ENTRY));

    if (new_list == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < new_capacity; i++)
    {
        ColorExtrapolatorSetSpectrumEntryEmpty(new_list + i);
    }

    for (size_t i = 0; i < color_extrapolator->spectrum_list_capacity; i++)
    {
        if (ColorExtrapolatorIsSpectrumEntryEmpty(color_extrapolator->spectrum_list + i))
        {
            continue;
        }

        ColorExtrapolatorInsertSpectrum(new_list,
                                        new_capacity,
                                        color_extrapolator->spectrum_list[i].color,
                                        color_extrapolator->spectrum_list[i].spectrum);
    }

    free(color_extrapolator->spectrum_list);

    color_extrapolator->spectrum_list = new_list;
    color_extrapolator->spectrum_list_capacity = new_capacity;

    color_extrapolator->spectrum_list_usable_capacity =
        ColorExtrapolatorComputeUsableCapacity(new_capacity);

    return ISTATUS_SUCCESS;
}

static
inline
ISTATUS
ColorExtrapolatorResizeReflectorHashTable(
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ size_t new_capacity
    )
{
    PREFLECTOR_LIST_ENTRY new_list =
        (PREFLECTOR_LIST_ENTRY)calloc(new_capacity, sizeof(REFLECTOR_LIST_ENTRY));

    if (new_list == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < new_capacity; i++)
    {
        ColorExtrapolatorSetReflectorEntryEmpty(new_list + i);
    }

    for (size_t i = 0; i < color_extrapolator->reflector_list_capacity; i++)
    {
        if (ColorExtrapolatorIsReflectorEntryEmpty(color_extrapolator->reflector_list + i))
        {
            continue;
        }

        ColorExtrapolatorInsertReflector(new_list,
                                         new_capacity,
                                         color_extrapolator->reflector_list[i].color,
                                         color_extrapolator->reflector_list[i].reflector);
    }

    free(color_extrapolator->reflector_list);

    color_extrapolator->reflector_list = new_list;
    color_extrapolator->reflector_list_capacity = new_capacity;

    color_extrapolator->reflector_list_usable_capacity =
        ColorExtrapolatorComputeUsableCapacity(new_capacity);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
ColorExtrapolatorGrowSpectrumHashTable(
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator
    )
{
    size_t new_capacity;
    bool success = CheckedMultiplySizeT(color_extrapolator->spectrum_list_capacity,
                                        LIST_GROWTH_FACTOR,
                                        &new_capacity);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ISTATUS status = ColorExtrapolatorResizeSpectrumHashTable(color_extrapolator,
                                                              new_capacity);

    return status;
}

static
ISTATUS
ColorExtrapolatorGrowReflectorHashTable(
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator
    )
{
    size_t new_capacity;
    bool success = CheckedMultiplySizeT(color_extrapolator->reflector_list_capacity,
                                        LIST_GROWTH_FACTOR,
                                        &new_capacity);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ISTATUS status = ColorExtrapolatorResizeReflectorHashTable(color_extrapolator,
                                                               new_capacity);

    return status;
}

//
// Functions
//

ISTATUS
ColorExtrapolatorAllocate(
    _In_ PCCOLOR_EXTRAPOLATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCOLOR_EXTRAPOLATOR *color_extrapolator
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

    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    PCOLOR_EXTRAPOLATOR result;
    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(COLOR_EXTRAPOLATOR),
                                          alignof(COLOR_EXTRAPOLATOR),
                                          (void **)&result,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
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

    for (size_t i = 0; i < INITIAL_LIST_SIZE; i++)
    {
        ColorExtrapolatorSetSpectrumEntryEmpty(result->spectrum_list + i);
        ColorExtrapolatorSetReflectorEntryEmpty(result->reflector_list + i);
    }

    size_t usable = ColorExtrapolatorComputeUsableCapacity(INITIAL_LIST_SIZE);

    result->vtable = vtable;
    result->reflector_list_capacity = INITIAL_LIST_SIZE;
    result->reflector_list_usable_capacity = usable;
    result->reflector_list_size = 0;
    result->spectrum_list_capacity = INITIAL_LIST_SIZE;
    result->spectrum_list_usable_capacity = usable;
    result->spectrum_list_size = 0;
    result->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    *color_extrapolator = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorExtrapolatorPrepareToComputeSpectra(
    _In_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ size_t num_spectra
    )
{
    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    size_t usable_size = color_extrapolator->spectrum_list_usable_capacity;

    if (num_spectra < usable_size)
    {
        return ISTATUS_SUCCESS;
    }

    size_t new_capacity = color_extrapolator->spectrum_list_capacity;
    while (usable_size < num_spectra)
    {
        bool success = CheckedMultiplySizeT(new_capacity,
                                            LIST_GROWTH_FACTOR,
                                            &new_capacity);

        if (!success)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        usable_size = ColorExtrapolatorComputeUsableCapacity(new_capacity);
    }

    ISTATUS status =
        ColorExtrapolatorResizeSpectrumHashTable(color_extrapolator,
                                                 new_capacity);

    return status;
}

ISTATUS
ColorExtrapolatorPrepareToComputeReflectors(
    _In_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ size_t num_reflectors
    )
{
    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    size_t usable_size = color_extrapolator->reflector_list_usable_capacity;

    if (num_reflectors < usable_size)
    {
        return ISTATUS_SUCCESS;
    }

    size_t new_capacity = color_extrapolator->reflector_list_capacity;
    while (usable_size < num_reflectors)
    {
        bool success = CheckedMultiplySizeT(new_capacity,
                                            LIST_GROWTH_FACTOR,
                                            &new_capacity);

        if (!success)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        usable_size = ColorExtrapolatorComputeUsableCapacity(new_capacity);
    }

    ISTATUS status =
        ColorExtrapolatorResizeReflectorHashTable(color_extrapolator,
                                                  new_capacity);

    return status;
}

ISTATUS
ColorExtrapolatorComputeSpectrum(
    _In_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ COLOR3 color,
    _Out_ PSPECTRUM *spectrum
    )
{
    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!ColorValidate(color))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (ColorIsBlack(color))
    {
        *spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    PSPECTRUM_LIST_ENTRY entry;
    bool found = ColorExtrapolatorFindSpectrumEntry(color_extrapolator,
                                                    color,
                                                    &entry);

    if (found)
    {
        *spectrum = entry->spectrum;
        SpectrumRetain(entry->spectrum);
        return ISTATUS_SUCCESS;
    }

    PSPECTRUM result;
    ISTATUS status =
        color_extrapolator->vtable->compute_spectrum_routine(
            color_extrapolator->data, color, &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    color_extrapolator->spectrum_list_size += 1;
    entry->spectrum = result;
    entry->color = color;

    if (ColorExtrapolatorSpectrumListFull(color_extrapolator))
    {
        ISTATUS status =
            ColorExtrapolatorGrowSpectrumHashTable(color_extrapolator);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    SpectrumRetain(result);
    *spectrum = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorExtrapolatorComputeReflector(
    _In_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ COLOR3 color,
    _Out_ PREFLECTOR *reflector
    )
{
    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!ColorValidate(color))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (ColorIsBlack(color))
    {
        *reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    PREFLECTOR_LIST_ENTRY entry;
    bool found = ColorExtrapolatorFindReflectorEntry(color_extrapolator,
                                                     color,
                                                     &entry);

    if (found)
    {
        *reflector = entry->reflector;
        ReflectorRetain(entry->reflector);
        return ISTATUS_SUCCESS;
    }

    PREFLECTOR result;
    ISTATUS status =
        color_extrapolator->vtable->compute_reflector_routine(
            color_extrapolator->data, color, &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    color_extrapolator->reflector_list_size += 1;
    entry->reflector = result;
    entry->color = color;

    if (ColorExtrapolatorReflectorListFull(color_extrapolator))
    {
        ISTATUS status =
            ColorExtrapolatorGrowReflectorHashTable(color_extrapolator);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    ReflectorRetain(result);
    *reflector = result;

    return ISTATUS_SUCCESS;
}

void
ColorExtrapolatorFree(
    _In_opt_ _Post_invalid_ PCOLOR_EXTRAPOLATOR color_extrapolator
    )
{
    if (color_extrapolator == NULL)
    {
        return;
    }

    for (size_t i = 0; i < color_extrapolator->reflector_list_capacity; i++)
    {
        ReflectorRelease(color_extrapolator->reflector_list[i].reflector);
    }

    free(color_extrapolator->reflector_list);

    for (size_t i = 0; i < color_extrapolator->spectrum_list_capacity; i++)
    {
        SpectrumRelease(color_extrapolator->spectrum_list[i].spectrum);
    }

    free(color_extrapolator->spectrum_list);

    if (color_extrapolator->vtable->free_routine)
    {
        color_extrapolator->vtable->free_routine(color_extrapolator->data);
    }

    free(color_extrapolator);
}