/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    color_cache_internal.h

Abstract:

    The internal routines for a color cache.

--*/

#ifndef _IRIS_PHYSX_COLOR_CACHE_INTERNAL_
#define _IRIS_PHYSX_COLOR_CACHE_INTERNAL_

#include <stdlib.h>
#include <string.h>

#include "common/safe_math.h"
#include "iris_physx/color_integrator.h"
#include "third_party/smhasher/MurmurHash2.h"
#include "third_party/smhasher/MurmurHash3.h"

//
// Defines
//

#define SPECTRUM_TO_COLOR_MAP_INITIAL_LIST_SIZE 16
#define SPECTRUM_TO_COLOR_MAP_GROWTH_FACTOR 2
#define SPECTRUM_TO_COLOR_MAP_HASH_SEED 0

#define REFLECTOR_TO_COLOR_MAP_INITIAL_LIST_SIZE 16
#define REFLECTOR_TO_COLOR_MAP_GROWTH_FACTOR 2
#define REFLECTOR_TO_COLOR_MAP_HASH_SEED 0
//
// Types
//

typedef struct _REFLECTOR_LIST_ENTRY {
    PCREFLECTOR reflector;
    COLOR3 color;
} REFLECTOR_LIST_ENTRY, *PREFLECTOR_LIST_ENTRY;

typedef const REFLECTOR_LIST_ENTRY *PCREFLECTOR_LIST_ENTRY;

typedef struct _SPECTRUM_LIST_ENTRY {
    PCSPECTRUM spectrum;
    COLOR3 color;
} SPECTRUM_LIST_ENTRY, *PSPECTRUM_LIST_ENTRY;

typedef const SPECTRUM_LIST_ENTRY *PCSPECTRUM_LIST_ENTRY;

typedef struct _SPECTRUM_TO_COLOR_MAP {
    _Field_size_full_(capacity) PSPECTRUM_LIST_ENTRY list;
    size_t capacity;
    size_t size;
} SPECTRUM_TO_COLOR_MAP, *PSPECTRUM_TO_COLOR_MAP;

typedef const SPECTRUM_TO_COLOR_MAP *PCSPECTRUM_TO_COLOR_MAP;

typedef struct _REFLECTOR_TO_COLOR_MAP {
    _Field_size_full_(capacity) PREFLECTOR_LIST_ENTRY list;
    size_t capacity;
    size_t size;
} REFLECTOR_TO_COLOR_MAP, *PREFLECTOR_TO_COLOR_MAP;

typedef const REFLECTOR_TO_COLOR_MAP *PCREFLECTOR_TO_COLOR_MAP;

struct _COLOR_CACHE {
    PCCOLOR_INTEGRATOR color_integrator;
    SPECTRUM_TO_COLOR_MAP spectrum_map;
    REFLECTOR_TO_COLOR_MAP reflector_map;
};

//
// Static Functions
//

static
inline
size_t
SpectrumToColorMapProbeStart(
    _In_ size_t list_capacity,
    _In_ PCSPECTRUM spectrum
    )
{
    assert(list_capacity != 0);
    assert(spectrum != NULL);

    size_t hash;
    if (sizeof(PCSPECTRUM) == 4) {
        MurmurHash3_x86_32(&spectrum,
                           sizeof(PCSPECTRUM),
                           SPECTRUM_TO_COLOR_MAP_HASH_SEED,
                           &hash);
    } else if (sizeof(PCSPECTRUM) == 8) {
        hash = MurmurHash64A(&spectrum,
                             sizeof(PCSPECTRUM),
                             SPECTRUM_TO_COLOR_MAP_HASH_SEED);
    } else {
        assert(false);
        hash = 0;
    }

    return hash % list_capacity;
}

static
inline
size_t
ReflectorToColorMapProbeStart(
    _In_ size_t list_capacity,
    _In_ PCREFLECTOR reflector
    )
{
    assert(list_capacity != 0);
    assert(reflector != NULL);

    size_t hash;
    if (sizeof(PCREFLECTOR) == 4) {
        MurmurHash3_x86_32(&reflector,
                           sizeof(PCREFLECTOR),
                           REFLECTOR_TO_COLOR_MAP_HASH_SEED,
                           &hash);
    } else if (sizeof(PCREFLECTOR) == 8) {
        hash = MurmurHash64A(&reflector,
                             sizeof(PCREFLECTOR),
                             REFLECTOR_TO_COLOR_MAP_HASH_SEED);
    } else {
        assert(false);
        hash = 0;
    }

    return hash % list_capacity;
}

static
inline
bool
SpectrumToColorMapFindIndex(
    _In_ PCSPECTRUM_TO_COLOR_MAP map,
    _In_ PCSPECTRUM spectrum,
    _Out_ size_t *index
    )
{
    assert(map != NULL);
    assert(spectrum != NULL);
    assert(index != NULL);

    *index = SpectrumToColorMapProbeStart(map->capacity, spectrum);
    for (;;) {
        if (map->list[*index].spectrum == spectrum)
        {
            return true;
        }

        if (map->list[*index].spectrum == NULL)
        {
            return false;
        }

        *index += 1;

        if (*index == map->capacity)
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
ReflectorToColorMapFindIndex(
    _In_ PCREFLECTOR_TO_COLOR_MAP map,
    _In_ PCREFLECTOR reflector,
    _Out_ size_t *index
    )
{
    assert(map != NULL);
    assert(reflector != NULL);
    assert(index != NULL);

    *index = ReflectorToColorMapProbeStart(map->capacity, reflector);
    for (;;) {
        if (map->list[*index].reflector == reflector)
        {
            return true;
        }

        if (map->list[*index].reflector == NULL)
        {
            return false;
        }

        *index += 1;

        if (*index == map->capacity)
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
SpectrumToColorMapLookup(
    _In_ PCSPECTRUM_TO_COLOR_MAP map,
    _In_ PCSPECTRUM spectrum,
    _Out_ PCOLOR3 color
    )
{
    assert(map != NULL);
    assert(spectrum != NULL);
    assert(color != NULL);

    size_t index;
    bool result = SpectrumToColorMapFindIndex(map, spectrum, &index);

    *color = map->list[index].color;

    return result;
}

static
inline
bool
ReflectorToColorMapLookup(
    _In_ PCREFLECTOR_TO_COLOR_MAP map,
    _In_ PCREFLECTOR reflector,
    _Out_ PCOLOR3 color
    )
{
    assert(map != NULL);
    assert(reflector != NULL);
    assert(color != NULL);

    size_t index;
    bool result = ReflectorToColorMapFindIndex(map, reflector, &index);

    *color = map->list[index].color;

    return result;
}

static
inline
void
SpectrumToColorMapInsert(
    _Inout_ PSPECTRUM_TO_COLOR_MAP map,
    _In_ PCSPECTRUM spectrum,
    _In_ COLOR3 color
    )
{
    assert(map != NULL);
    assert(spectrum != NULL);
    assert(ColorValidate(color));

    size_t index = SpectrumToColorMapProbeStart(map->capacity, spectrum);
    for (;;) {
        if (map->list[index].spectrum == spectrum)
        {
            return;
        }

        if (map->list[index].spectrum == NULL)
        {
            map->list[index].spectrum = spectrum;
            map->list[index].color = color;
            return;
        }

        index += 1;

        if (index == map->capacity) {
            index = 0;
        }
    }
}

static
inline
bool
ReflectorToColorMapInsert(
    _Inout_ PREFLECTOR_TO_COLOR_MAP map,
    _In_ PCREFLECTOR reflector,
    _In_ COLOR3 color
    )
{
    assert(map != NULL);
    assert(reflector != NULL);
    assert(ColorValidate(color));

    size_t index = ReflectorToColorMapProbeStart(map->capacity, reflector);
    for (;;) {
        if (map->list[index].reflector == reflector)
        {
            return false;
        }

        if (map->list[index].reflector == NULL)
        {
            map->list[index].reflector = reflector;
            map->list[index].color = color;
            return true;
        }

        index += 1;

        if (index == map->capacity) {
            index = 0;
        }
    }
}

static
inline
bool
SpectrumToColorMapGrow(
    _Inout_ PSPECTRUM_TO_COLOR_MAP map
    )
{
    size_t new_capacity;
    bool success = CheckedMultiplySizeT(map->capacity,
                                        SPECTRUM_TO_COLOR_MAP_GROWTH_FACTOR,
                                        &new_capacity);
    
    if (!success)
    {
        return false;
    }

    PSPECTRUM_LIST_ENTRY new_list =
        (PSPECTRUM_LIST_ENTRY)calloc(new_capacity, sizeof(SPECTRUM_LIST_ENTRY));

    if (new_list == NULL)
    {
        return false;
    }

    size_t old_capacity = map->capacity;
    PSPECTRUM_LIST_ENTRY old_list = map->list;

    map->list = new_list;
    map->capacity = new_capacity;

    for (size_t i = 0; i < old_capacity; i++)
    {
        SpectrumToColorMapInsert(map,
                                 old_list[i].spectrum,
                                 old_list[i].color);
    }

    free(old_list);

    return true;
}

static
inline
bool
ReflectorToColorMapGrow(
    _Inout_ PREFLECTOR_TO_COLOR_MAP map
    )
{
    size_t new_capacity;
    bool success = CheckedMultiplySizeT(map->capacity,
                                        REFLECTOR_TO_COLOR_MAP_GROWTH_FACTOR,
                                        &new_capacity);
    
    if (!success)
    {
        return false;
    }

    PREFLECTOR_LIST_ENTRY new_list =
        (PREFLECTOR_LIST_ENTRY)calloc(new_capacity, sizeof(REFLECTOR_LIST_ENTRY));

    if (new_list == NULL)
    {
        return true;
    }

    size_t old_capacity = map->capacity;
    PREFLECTOR_LIST_ENTRY old_list = map->list;

    map->list = new_list;
    map->capacity = new_capacity;

    for (size_t i = 0; i < old_capacity; i++)
    {
        ReflectorToColorMapInsert(map,
                                  old_list[i].reflector,
                                  old_list[i].color);
    }

    free(old_list);

    return true;
}

static
inline
size_t
SpectrumToColorMapAvailableSlots(
    _In_ PCSPECTRUM_TO_COLOR_MAP map
    )
{
    size_t usable_slots = map->capacity;
    if (usable_slots < 8)
    {
        usable_slots = usable_slots - 1;
    }
    else 
    {
        usable_slots = usable_slots - usable_slots / 8;
    }

    return usable_slots - map->size;
}

static
inline
size_t
ReflectorToColorMapAvailableSlots(
    _In_ PCREFLECTOR_TO_COLOR_MAP map
    )
{
    size_t usable_slots = map->capacity;
    if (usable_slots < 8)
    {
        usable_slots = usable_slots - 1;
    }
    else 
    {
        usable_slots = usable_slots - usable_slots / 8;
    }

    return usable_slots - map->size;
}

static
inline
ISTATUS
SpectrumToColorMapAdd(
    _Inout_ PSPECTRUM_TO_COLOR_MAP map,
    _In_ PCSPECTRUM spectrum,
    _In_ COLOR3 color
    )
{
    assert(map != NULL);
    assert(spectrum != NULL);
    assert(ColorValidate(color));

    if (SpectrumToColorMapAvailableSlots(map) == 0)
    {
        bool resized = SpectrumToColorMapGrow(map);
        if (resized)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }
    }

    bool added = SpectrumToColorMapAdd(map, spectrum, color);
    if (!added)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    map->size += 1;

    return ISTATUS_SUCCESS;
}

static
inline
ISTATUS
ReflectorToColorMapAdd(
    _Inout_ PREFLECTOR_TO_COLOR_MAP map,
    _In_ PCREFLECTOR reflector,
    _In_ COLOR3 color
    )
{
    assert(map != NULL);
    assert(reflector != NULL);
    assert(ColorValidate(color));

    if (ReflectorToColorMapAvailableSlots(map) == 0)
    {
        bool resized = ReflectorToColorMapGrow(map);
        if (resized)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }
    }

    bool added = ReflectorToColorMapAdd(map, reflector, color);
    if (!added)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    map->size += 1;

    return ISTATUS_SUCCESS;
}

static
inline
bool
SpectrumToColorMapInitialize(
    _Out_ PSPECTRUM_TO_COLOR_MAP map
    )
{
    assert(map != NULL);

    map->list =
        (PSPECTRUM_LIST_ENTRY)calloc(
            SPECTRUM_TO_COLOR_MAP_INITIAL_LIST_SIZE,
            sizeof(SPECTRUM_LIST_ENTRY));
    if (map->list == NULL)
    {
        return false;
    }

    map->capacity = SPECTRUM_TO_COLOR_MAP_INITIAL_LIST_SIZE;
    map->size = 0;

    return ISTATUS_SUCCESS;
}

static
inline
bool
ReflectorToColorMapInitialize(
    _Out_ PREFLECTOR_TO_COLOR_MAP map
    )
{
    assert(map != NULL);

    map->list =
        (PREFLECTOR_LIST_ENTRY)calloc(
            REFLECTOR_TO_COLOR_MAP_INITIAL_LIST_SIZE,
            sizeof(REFLECTOR_LIST_ENTRY));
    if (map->list == NULL)
    {
        return false;
    }

    map->capacity = REFLECTOR_TO_COLOR_MAP_INITIAL_LIST_SIZE;
    map->size = 0;

    return ISTATUS_SUCCESS;
}

static
inline
void
SpectrumToColorMapDestroy(
    _Inout_ PSPECTRUM_TO_COLOR_MAP map
    )
{
    assert(map != NULL);
    free(map->list);
}

static
inline
void
ReflectorToColorMapDestroy(
    _Inout_ PREFLECTOR_TO_COLOR_MAP map
    )
{
    assert(map != NULL);
    free(map->list);
}

static
inline
bool
ColorCacheInitialize(
    _Out_ struct _COLOR_CACHE *cache,
    _In_ PCCOLOR_INTEGRATOR color_integrator
    )
{
    assert(cache != NULL);
    assert(color_integrator != NULL);

    cache->color_integrator = color_integrator;

    bool success = SpectrumToColorMapInitialize(&cache->spectrum_map);

    if (!success)
    {
        return false;
    }

    success = ReflectorToColorMapInitialize(&cache->reflector_map);

    if (!success)
    {
        SpectrumToColorMapDestroy(&cache->spectrum_map);
        return false;
    }
}

static
inline
void
ColorCacheClear(
    _Inout_ struct _COLOR_CACHE *cache
    )
{
    cache->color_integrator = NULL;

    memset(&cache->spectrum_map.list,
           0,
           sizeof(SPECTRUM_LIST_ENTRY) * cache->spectrum_map.capacity);

    memset(&cache->reflector_map.list,
           0,
           sizeof(REFLECTOR_LIST_ENTRY) * cache->reflector_map.capacity);
}

static
inline
void
ColorCacheDestroy(
    _Inout_ struct _COLOR_CACHE *cache
    )
{
    cache->color_integrator = NULL;
    SpectrumToColorMapDestroy(&cache->spectrum_map);
    ReflectorToColorMapDestroy(&cache->reflector_map);
}

#endif // _IRIS_PHYSX_COLOR_CACHE_INTERNAL_