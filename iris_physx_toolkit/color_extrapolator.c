/*++

Copyright (c) 2019 Brad Weinberger

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
    float_t color[3];
    PREFLECTOR reflector;
} REFLECTOR_LIST_ENTRY, *PREFLECTOR_LIST_ENTRY;

typedef const REFLECTOR_LIST_ENTRY *PCREFLECTOR_LIST_ENTRY;

typedef struct _SPECTRUM_LIST_ENTRY {
    float_t color[3];
    PSPECTRUM spectrum;
} SPECTRUM_LIST_ENTRY, *PSPECTRUM_LIST_ENTRY;

typedef const SPECTRUM_LIST_ENTRY *PCSPECTRUM_LIST_ENTRY;

struct _COLOR_EXTRAPOLATOR {
    PCCOLOR_EXTRAPOLATOR_VTABLE vtable;
    _Field_size_full_(reflector_list_capacity) PREFLECTOR_LIST_ENTRY reflector_list;
    _Field_size_full_(spectrum_list_capacity) PSPECTRUM_LIST_ENTRY spectrum_list;
    size_t reflector_list_capacity;
    size_t reflector_list_size;
    size_t spectrum_list_capacity;
    size_t spectrum_list_size;
    void *data;
};

//
// Static Functions
//

static
inline
void
ColorExtrapolatorSetSpectrumEntryEmpty(
    _Inout_ PSPECTRUM_LIST_ENTRY entry
    )
{
    entry->color[0] = (float_t)-1.0;
}

static
inline
void
ColorExtrapolatorSetReflectorEntryEmpty(
    _Inout_ PREFLECTOR_LIST_ENTRY entry
    )
{
    entry->color[0] = (float_t)-1.0;
}

static
inline
bool
ColorExtrapolatorIsSpectrumEntryEmpty(
    _In_ PCSPECTRUM_LIST_ENTRY entry
    )
{
    return entry->color[0] < (float_t)0.0;
}

static
inline
bool
ColorExtrapolatorIsReflectorEntryEmpty(
    _In_ PCREFLECTOR_LIST_ENTRY entry
    )
{
    return entry->color[0] < (float_t)0.0;
}

static
inline
size_t
ColorExtrapolatorSpectrumProbeStart(
    _In_ size_t list_capacity,
    _In_ const float_t color[3]
    )
{
    size_t hash;
    if (sizeof(PCSPECTRUM) == 4) {
        MurmurHash3_x86_32(color, sizeof(float_t) * 3, HASH_SEED, &hash);
    } else if (sizeof(PCSPECTRUM) == 8) {
        hash = MurmurHash64A(color, sizeof(float_t) * 3, HASH_SEED);
    } else {
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
    _In_ const float_t color[3]
    )
{
    size_t hash;
    if (sizeof(PCREFLECTOR) == 4) {
        MurmurHash3_x86_32(color, sizeof(float_t) * 3, HASH_SEED, &hash);
    } else if (sizeof(PCREFLECTOR) == 8) {
        hash = MurmurHash64A(color, sizeof(float_t) * 3, HASH_SEED);
    } else {
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
    _In_ const float_t color[3],
    _Out_ size_t *index
    )
{
    *index = ColorExtrapolatorSpectrumProbeStart(
        color_extrapolator->spectrum_list_capacity, color);
    for (;;) {
        if (color_extrapolator->spectrum_list[*index].color[0] == color[0] &&
            color_extrapolator->spectrum_list[*index].color[1] == color[1] &&
            color_extrapolator->spectrum_list[*index].color[2] == color[2])
        {
            return true;
        }

        if (ColorExtrapolatorIsSpectrumEntryEmpty(color_extrapolator->spectrum_list + *index))
        {
            return false;
        }

        *index += 1;

        if (*index == color_extrapolator->spectrum_list_capacity) {
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
    _In_ const float_t color[3],
    _Out_ size_t *index
    )
{
    *index = ColorExtrapolatorReflectorProbeStart(
        color_extrapolator->reflector_list_capacity, color);
    for (;;) {
        if (color_extrapolator->reflector_list[*index].color[0] == color[0] &&
            color_extrapolator->reflector_list[*index].color[1] == color[1] &&
            color_extrapolator->reflector_list[*index].color[2] == color[2])
        {
            return true;
        }

        if (ColorExtrapolatorIsReflectorEntryEmpty(color_extrapolator->reflector_list + *index))
        {
            return false;
        }

        *index += 1;

        if (*index == color_extrapolator->reflector_list_capacity) {
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
    _In_ const float_t color[3],
    _Out_ PCSPECTRUM_LIST_ENTRY *entry
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
    _In_ const float_t color[3],
    _Out_ PCREFLECTOR_LIST_ENTRY *entry
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
    _In_ const float_t color[3],
    _In_ PSPECTRUM spectrum
    )
{
    size_t index = ColorExtrapolatorSpectrumProbeStart(list_size, color);
    for (;;) {
        if (list[index].spectrum == spectrum)
        {
            return;
        }

        if (list[index].spectrum == NULL)
        {
            list[index].color[0] = color[0];
            list[index].color[1] = color[1];
            list[index].color[2] = color[2];
            list[index].spectrum = spectrum;
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
ColorExtrapolatorInsertReflector(
    _Inout_updates_(list_size) PREFLECTOR_LIST_ENTRY list,
    _In_ size_t list_size,
    _In_ const float_t color[3],
    _In_ PREFLECTOR reflector
    )
{
    size_t index = ColorExtrapolatorReflectorProbeStart(list_size, color);
    for (;;) {
        if (list[index].reflector == reflector)
        {
            return;
        }

        if (list[index].reflector == NULL)
        {
            list[index].color[0] = color[0];
            list[index].color[1] = color[1];
            list[index].color[2] = color[2];
            list[index].reflector = reflector;
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
        ColorExtrapolatorInsertSpectrum(new_list,
                                        new_capacity,
                                        color_extrapolator->spectrum_list[i].color,
                                        color_extrapolator->spectrum_list[i].spectrum);
    }

    free(color_extrapolator->spectrum_list);

    color_extrapolator->spectrum_list = new_list;
    color_extrapolator->spectrum_list_capacity = new_capacity;

    return ISTATUS_SUCCESS;
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
        ColorExtrapolatorInsertReflector(new_list,
                                         new_capacity,
                                         color_extrapolator->reflector_list[i].color,
                                         color_extrapolator->reflector_list[i].reflector);
    }

    free(color_extrapolator->reflector_list);

    color_extrapolator->reflector_list = new_list;
    color_extrapolator->reflector_list_capacity = new_capacity;

    return ISTATUS_SUCCESS;
}

static
inline
size_t
ColorExtrapolatorAvailableSpectrumSlots(
    _In_ PCCOLOR_EXTRAPOLATOR color_extrapolator
    )
{
    size_t usable_slots = color_extrapolator->spectrum_list_capacity;
    if (usable_slots < 8)
    {
        usable_slots = usable_slots - 1;
    }
    else 
    {
        usable_slots = usable_slots - usable_slots / 8;
    }

    return usable_slots - color_extrapolator->spectrum_list_size;
}

static
inline
size_t
ColorExtrapolatorAvailableReflectorSlots(
    _In_ PCCOLOR_EXTRAPOLATOR color_extrapolator
    )
{
    size_t usable_slots = color_extrapolator->reflector_list_capacity;
    if (usable_slots < 8)
    {
        usable_slots = usable_slots - 1;
    }
    else 
    {
        usable_slots = usable_slots - usable_slots / 8;
    }

    return usable_slots - color_extrapolator->reflector_list_size;
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

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(COLOR_EXTRAPOLATOR),
                                          alignof(COLOR_EXTRAPOLATOR),
                                          (void **)color_extrapolator,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*color_extrapolator)->reflector_list =
        (PREFLECTOR_LIST_ENTRY)calloc(INITIAL_LIST_SIZE, sizeof(REFLECTOR_LIST_ENTRY));
    if ((*color_extrapolator)->reflector_list == NULL)
    {
        free(*color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*color_extrapolator)->spectrum_list =
        (PSPECTRUM_LIST_ENTRY)calloc(INITIAL_LIST_SIZE, sizeof(SPECTRUM_LIST_ENTRY));
    if ((*color_extrapolator)->spectrum_list == NULL)
    {
        free((*color_extrapolator)->reflector_list);
        free(*color_extrapolator);
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*color_extrapolator)->vtable = vtable;
    (*color_extrapolator)->reflector_list_capacity = INITIAL_LIST_SIZE;
    (*color_extrapolator)->reflector_list_size = 0;
    (*color_extrapolator)->spectrum_list_capacity = INITIAL_LIST_SIZE;
    (*color_extrapolator)->spectrum_list_size = 0;
    (*color_extrapolator)->data = data_allocation;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorExtrapolatorComputeSpectrum(
    _In_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ float_t color[3],
    _Out_ PSPECTRUM *spectrum
    )
{
    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (color == NULL ||
        !isfinite(color[0]) || color[0] < (float_t)0.0 ||
        !isfinite(color[1]) || color[1] < (float_t)0.0 ||
        !isfinite(color[2]) || color[2] < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    PCSPECTRUM_LIST_ENTRY entry;
    bool found = ColorExtrapolatorFindSpectrumEntry(color_extrapolator,
                                                    color,
                                                    &entry);

    if (found)
    {
        *spectrum = entry->spectrum;
        SpectrumRelease(entry->spectrum); 
        return ISTATUS_SUCCESS;
    }

    if (ColorExtrapolatorAvailableSpectrumSlots(color_extrapolator) == 0)
    {
        ISTATUS status = ColorExtrapolatorGrowSpectrumHashTable(color_extrapolator);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    PSPECTRUM result;
    ISTATUS status =
        color_extrapolator->vtable->compute_spectrum_routine(
            color_extrapolator->data, color, &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ColorExtrapolatorInsertSpectrum(color_extrapolator->spectrum_list,
                                    color_extrapolator->spectrum_list_capacity,
                                    color,
                                    result);

    SpectrumRetain(result);
    *spectrum = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
ColorExtrapolatorComputeReflector(
    _In_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _In_ float_t color[3],
    _Out_ PREFLECTOR *reflector
    )
{
    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (color == NULL ||
        !isfinite(color[0]) || color[0] < (float_t)0.0 ||
        !isfinite(color[1]) || color[1] < (float_t)0.0 ||
        !isfinite(color[2]) || color[2] < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    PCREFLECTOR_LIST_ENTRY entry;
    bool found = ColorExtrapolatorFindReflectorEntry(color_extrapolator,
                                                     color,
                                                     &entry);

    if (found)
    {
        *reflector = entry->reflector;
        ReflectorRelease(entry->reflector); 
        return ISTATUS_SUCCESS;
    }

    if (ColorExtrapolatorAvailableReflectorSlots(color_extrapolator) == 0)
    {
        ISTATUS status = ColorExtrapolatorGrowReflectorHashTable(color_extrapolator);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    PREFLECTOR result;
    ISTATUS status =
        color_extrapolator->vtable->compute_reflector_routine(
            color_extrapolator->data, color, &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ColorExtrapolatorInsertReflector(color_extrapolator->reflector_list,
                                     color_extrapolator->reflector_list_capacity,
                                     color,
                                     result);

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