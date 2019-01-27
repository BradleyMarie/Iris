/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    color_integrator_internal.h

Abstract:

    The internal routines for a color integrator.

--*/

#ifndef _IRIS_PHYSX_COLOR_INTEGRATOR_INTERNAL_
#define _IRIS_PHYSX_COLOR_INTEGRATOR_INTERNAL_

#include <stdlib.h>

#include "iris_physx/color_integrator_vtable.h"

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

static
PCSPECTRUM_LIST_ENTRY
ColorIntegratorFindSpectrumEntry(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_ PCSPECTRUM spectrum
    )
{
    assert(color_integrator != NULL);
    assert(spectrum != NULL);

    SPECTRUM_LIST_ENTRY key;
    key.spectrum = (PSPECTRUM)spectrum;

    const void *result = bsearch(&key,
                                 color_integrator->spectrum_list,
                                 color_integrator->spectrum_list_size,
                                 sizeof(SPECTRUM_LIST_ENTRY),
                                 ColorIntegratorCompareSpectrumEntry);

    return (PCSPECTRUM_LIST_ENTRY)result;
}

static
PCREFLECTOR_LIST_ENTRY
ColorIntegratorFindReflectorEntry(
    _In_ const struct _COLOR_INTEGRATOR *color_integrator,
    _In_ PCREFLECTOR reflector
    )
{
    assert(color_integrator != NULL);
    assert(reflector != NULL);

    REFLECTOR_LIST_ENTRY key;
    key.reflector = (PREFLECTOR)reflector;

    const void *result = bsearch(&key,
                                 color_integrator->reflector_list,
                                 color_integrator->reflector_list_size,
                                 sizeof(REFLECTOR_LIST_ENTRY),
                                 ColorIntegratorCompareReflectorEntry);

    return (PCREFLECTOR_LIST_ENTRY)result;
}

inline
static
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

    PCSPECTRUM_LIST_ENTRY old_entry =
        ColorIntegratorFindSpectrumEntry(color_integrator, spectrum);
    if (old_entry != NULL)
    {
        *color = old_entry->color;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status =
        color_integrator->compute_spectrum_color_routine(spectrum, color);

    return status;
}

inline
static
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

    PCREFLECTOR_LIST_ENTRY old_entry =
        ColorIntegratorFindReflectorEntry(color_integrator, reflector);
    if (old_entry != NULL)
    {
        *color = old_entry->color;
        return ISTATUS_SUCCESS;
    }

    ISTATUS status =
        color_integrator->compute_reflector_color_routine(reflector, color);

    return status;
}

#endif // _IRIS_PHYSX_COLOR_INTEGRATOR_INTERNAL_