/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    color_integrator_internal.h

Abstract:

    The internal routines for a color integrator.

--*/

#ifndef _IRIS_PHYSX_COLOR_INTEGRATOR_INTERNAL_
#define _IRIS_PHYSX_COLOR_INTEGRATOR_INTERNAL_

#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

#include "iris_physx/color_integrator_vtable.h"
#include "iris_physx/reflector_internal.h"
#include "iris_physx/spectrum_internal.h"
#include "third_party/smhasher/MurmurHash2.h"
#include "third_party/smhasher/MurmurHash3.h"

//
// Defines
//

#define HASH_SEED 0

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
    PCCOLOR_INTEGRATOR_VTABLE vtable;
    _Field_size_full_(reflector_list_capacity) PREFLECTOR_LIST_ENTRY reflector_list;
    _Field_size_full_(spectrum_list_capacity) PSPECTRUM_LIST_ENTRY spectrum_list;
    size_t reflector_list_capacity;
    size_t reflector_list_size;
    size_t spectrum_list_capacity;
    size_t spectrum_list_size;
    void *data;
    atomic_uintptr_t reference_count;
};

//
// Static Functions
//

static
inline
size_t
ColorIntegratorSpectrumProbeStart(
    _In_ size_t list_capacity,
    _In_ PCSPECTRUM spectrum
    )
{
    assert(list_capacity != 0);
    assert(spectrum != NULL);

    size_t hash;
    if (sizeof(PCSPECTRUM) == 4) {
        MurmurHash3_x86_32(&spectrum, sizeof(PCSPECTRUM), HASH_SEED, &hash);
    } else if (sizeof(PCSPECTRUM) == 8) {
        hash = MurmurHash64A(&spectrum, sizeof(PCSPECTRUM), HASH_SEED);
    } else {
        assert(false);
        hash = 0;
    }

    return hash % list_capacity;
}

static
inline
size_t
ColorIntegratorReflectorProbeStart(
    _In_ size_t list_capacity,
    _In_ PCREFLECTOR reflector
    )
{
    assert(list_capacity != 0);
    assert(reflector != NULL);

    size_t hash;
    if (sizeof(PCREFLECTOR) == 4) {
        MurmurHash3_x86_32(&reflector, sizeof(PCSPECTRUM), HASH_SEED, &hash);
    } else if (sizeof(PCREFLECTOR) == 8) {
        hash = MurmurHash64A(&reflector, sizeof(PCREFLECTOR), HASH_SEED);
    } else {
        assert(false);
        hash = 0;
    }

    return hash % list_capacity;
}

static
inline
bool
ColorIntegratorFindSpectrum(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_ PCSPECTRUM spectrum,
    _Out_ size_t *index
    )
{
    assert(color_integrator != NULL);
    assert(spectrum != NULL);
    assert(index != NULL);

    *index = ColorIntegratorSpectrumProbeStart(
        color_integrator->spectrum_list_capacity, spectrum);
    for (;;) {
        if (color_integrator->spectrum_list[*index].spectrum == spectrum)
        {
            return true;
        }

        if (color_integrator->spectrum_list[*index].spectrum == NULL)
        {
            return false;
        }

        *index += 1;

        if (*index == color_integrator->spectrum_list_capacity) {
            *index = 0;
        }
    }

    assert(false);
    return false;
}

static
inline
bool
ColorIntegratorFindReflector(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_ PCREFLECTOR reflector,
    _Out_ size_t *index
    )
{
    assert(color_integrator != NULL);
    assert(reflector != NULL);
    assert(index != NULL);

    *index = ColorIntegratorReflectorProbeStart(
        color_integrator->reflector_list_capacity, reflector);
    for (;;) {
        if (color_integrator->reflector_list[*index].reflector == reflector)
        {
            return true;
        }

        if (color_integrator->reflector_list[*index].reflector == NULL)
        {
            return false;
        }

        *index += 1;

        if (*index == color_integrator->reflector_list_capacity) {
            *index = 0;
        }
    }

    assert(false);
    return false;
}

static
inline
bool
ColorIntegratorFindSpectrumEntry(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_ PCSPECTRUM spectrum,
    _Out_ PCSPECTRUM_LIST_ENTRY *entry
    )
{
    assert(color_integrator != NULL);
    assert(spectrum != NULL);
    assert(entry != NULL);

    size_t index;
    bool result = ColorIntegratorFindSpectrum(color_integrator,
                                              spectrum,
                                              &index);

    *entry = color_integrator->spectrum_list + index;

    return result;
}

static
inline
bool
ColorIntegratorFindReflectorEntry(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_ PCREFLECTOR reflector,
    _Out_ PCREFLECTOR_LIST_ENTRY *entry
    )
{
    assert(color_integrator != NULL);
    assert(reflector != NULL);
    assert(entry != NULL);

    size_t index;
    bool result = ColorIntegratorFindReflector(color_integrator,
                                               reflector,
                                               &index);

    *entry = color_integrator->reflector_list + index;

    return result;
}

static
inline
ISTATUS
ColorIntegratorComputeSpectrumColor(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_opt_ PCSPECTRUM spectrum,
    _Out_ PCOLOR3 color
    )
{
    assert(color_integrator != NULL);
    assert(color != NULL);

    if (spectrum == NULL)
    {
        *color = ColorCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
        return ISTATUS_SUCCESS;
    }

    PCSPECTRUM_LIST_ENTRY entry;
    bool found = ColorIntegratorFindSpectrumEntry(color_integrator,
                                                  spectrum,
                                                  &entry);

    if (found)
    {
        *color = entry->color;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status =
        color_integrator->vtable->compute_spectrum_color_routine(
            color_integrator->data, spectrum, color);

    return status;
}

static
inline
ISTATUS
ColorIntegratorComputeReflectorColor(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_opt_ PCREFLECTOR reflector,
    _Out_ PCOLOR3 color
    )
{
    assert(color_integrator != NULL);
    assert(color != NULL);

    if (reflector == NULL)
    {
        *color = ColorCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
        return ISTATUS_SUCCESS;
    }

    PCREFLECTOR_LIST_ENTRY entry;
    bool found = ColorIntegratorFindReflectorEntry(color_integrator,
                                                   reflector,
                                                   &entry);

    if (found)
    {
        *color = entry->color;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status =
        color_integrator->vtable->compute_reflector_color_routine(
            color_integrator->data, reflector, color);

    return status;
}

static
inline
ISTATUS
ColorIntegratorComputeSpectrumColorFast(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_opt_ PCSPECTRUM spectrum,
    _Out_ PCOLOR3 color
    )
{
    assert(color_integrator != NULL);
    assert(color != NULL);

    if (spectrum == NULL)
    {
        *color = ColorCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
        return ISTATUS_SUCCESS;
    }

    PCSPECTRUM_LIST_ENTRY entry;
    bool found = ColorIntegratorFindSpectrumEntry(color_integrator,
                                                  spectrum,
                                                  &entry);

    if (found)
    {
        *color = entry->color;
        return ISTATUS_SUCCESS;
    }

    if (found)
    {
        *color = entry->color;
        return ISTATUS_SUCCESS;
    }

    if (spectrum->reference_count == 0)
    {
        PCINTERNAL_SPECTRUM_VTABLE internal_vtable =
            (const void*)spectrum->vtable;

        ISTATUS status =
            internal_vtable->compute_color_routine(spectrum->data,
                                                   color_integrator,
                                                   color);

        return status;
    }

    ISTATUS status =
        color_integrator->vtable->compute_spectrum_color_routine(
            color_integrator->data, spectrum, color);

    return status;
}

static
inline
ISTATUS
ColorIntegratorComputeReflectorColorFast(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_opt_ PCREFLECTOR reflector,
    _Out_ PCOLOR3 color
    )
{
    assert(color_integrator != NULL);
    assert(color != NULL);

    if (reflector == NULL)
    {
        *color = ColorCreate((float_t)0.0, (float_t)0.0, (float_t)0.0);
        return ISTATUS_SUCCESS;
    }

    PCREFLECTOR_LIST_ENTRY entry;
    bool found = ColorIntegratorFindReflectorEntry(color_integrator,
                                                   reflector,
                                                   &entry);

    if (found)
    {
        *color = entry->color;
        return ISTATUS_SUCCESS;
    }

    if (reflector->reference_count == 0)
    {
        PCINTERNAL_REFLECTOR_VTABLE internal_vtable =
            (const void*)reflector->vtable;

        ISTATUS status =
            internal_vtable->compute_color_routine(reflector->data,
                                                   color_integrator,
                                                   color);

        return status;
    }

    ISTATUS status =
        color_integrator->vtable->compute_reflector_color_routine(
            color_integrator->data, reflector, color);

    return status;
}

#endif // _IRIS_PHYSX_COLOR_INTEGRATOR_INTERNAL_