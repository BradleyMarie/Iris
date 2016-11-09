/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscommon_uint32toindexmap.h

Abstract:

    This module implements a map of uint32 to indices.

--*/

#ifndef _IRIS_COMMON_UINT32_TO_INDEX_MAP_
#define _IRIS_COMMON_UINT32_TO_INDEX_MAP_

#include <iris.h>
#include "iriscommon_types.h"

#define UINT32_TO_INDEX_MAP_INITIAL_CAPACITY 2
#define UINT32_TO_INDEX_MAP_MAX_CAPACITY \
 ((sizeof(UINT32) < sizeof(SIZE_T)) ? \
  (SIZE_MAX / sizeof(SIZE_T)) : \
  (SIZE_MAX / sizeof(UINT32)))

//
// Types
//

typedef struct _UINT32_TO_INDEX_MAP {
    _Field_size_(MapCapacity) PUINT32 Keys;
    _Field_size_(MapCapacity) PSIZE_T Values;
    SIZE_T NumberOfMappings;
    SIZE_T MapCapacity;
} UINT32_TO_INDEX_MAP, *PUINT32_TO_INDEX_MAP;

typedef CONST UINT32_TO_INDEX_MAP *PCUINT32_TO_INDEX_MAP;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
UInt32ToIndexMapInitialize(
    _Out_ PUINT32_TO_INDEX_MAP Map
    )
{
    PVOID Keys;
    PVOID Values;
    SIZE_T KeysCapacityInBytes;
    SIZE_T ValuesCapacityInBytes;
    SIZE_T MapCapacity;

	ASSERT(Map != NULL);

    MapCapacity = UINT32_TO_INDEX_MAP_INITIAL_CAPACITY;

    KeysCapacityInBytes = sizeof(UINT32) * MapCapacity;

    Keys = malloc(KeysCapacityInBytes);

    if (Keys == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    ValuesCapacityInBytes = sizeof(UINT32) * MapCapacity;

    Values = malloc(ValuesCapacityInBytes);

    if (Values == NULL)
    {
        free(Keys);
        return ISTATUS_ALLOCATION_FAILED;
    }

    Map->Keys = (PUINT32) Keys;
    Map->Values = (PSIZE_T)Values;
    Map->NumberOfMappings = 0;
    Map->MapCapacity = UINT32_TO_INDEX_MAP_INITIAL_CAPACITY;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return != FALSE)
SFORCEINLINE
BOOL
UInt32ToIndexMapFindKeyIndex(
    _In_ PCUINT32_TO_INDEX_MAP Map,
    _In_ UINT32 Key,
    _Out_ PSIZE_T Index
    )
{
    SIZE_T SearchIndex;

    ASSERT(Map != NULL);
    ASSERT(Index != NULL);

    for (SearchIndex = 0; SearchIndex < Map->NumberOfMappings; SearchIndex++)
    {
        if (Map->Keys[SearchIndex] == Key)
        {
            *Index = SearchIndex;
            return TRUE;
        }
    }

    return FALSE;
}

_Check_return_
_Success_(return != FALSE)
SFORCEINLINE
SIZE_T
UInt32ToIndexMapGetValueByIndex(
    _In_ PCUINT32_TO_INDEX_MAP Map,
    _In_ SIZE_T Index
    )
{
    ASSERT(Map != NULL);
    ASSERT(Index < Map->NumberOfMappings);

    return Map->Values[Index];
}

_Check_return_
_Success_(return != FALSE)
SFORCEINLINE
SIZE_T
UInt32ToIndexMapGetKeyByIndex(
    _In_ PCUINT32_TO_INDEX_MAP Map,
    _In_ SIZE_T Index
    )
{
    ASSERT(Map != NULL);
    ASSERT(Index < Map->NumberOfMappings);

    return Map->Keys[Index];
}

_Check_return_
_Success_(return != FALSE)
SFORCEINLINE
VOID
UInt32ToIndexMapSetValueByIndex(
    _In_ PCUINT32_TO_INDEX_MAP Map,
    _In_ SIZE_T Index,
    _In_ SIZE_T Value
    )
{
    ASSERT(Map != NULL);
    ASSERT(Index < Map->NumberOfMappings);

    Map->Values[Index] = Value;
}

_Check_return_
_Success_(return != FALSE)
SFORCEINLINE
BOOL
UInt32ToIndexMapLookup(
    _In_ PCUINT32_TO_INDEX_MAP Map,
    _In_ UINT32 Key,
    _Out_ PSIZE_T Value
    )
{
    BOOL Found;
    SIZE_T Index;

    ASSERT(Map != NULL);
    ASSERT(Value != NULL);

    Found = UInt32ToIndexMapFindKeyIndex(Map,
                                         Key,
                                         &Index);

    if (Found == FALSE)
    {
        return FALSE;
    }

    *Value = UInt32ToIndexMapGetValueByIndex(Map,
                                             Index);

    return TRUE;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
UInt32ToIndexMapAddMapping(
    _Inout_ PUINT32_TO_INDEX_MAP Map,
    _In_ UINT32 Key,
    _In_ SIZE_T Value,
    _In_ BOOL UpdateExisting,
    _Out_opt_ PBOOL MapModified
    )
{
    PVOID Allocation;
    BOOL Found;
    SIZE_T Index;
    SIZE_T NewCapacity;
    ISTATUS Status;

    ASSERT(Map != NULL);
    ASSERT(Map->MapCapacity != 0);

    Found = UInt32ToIndexMapFindKeyIndex(Map,
                                         Key,
                                         &Index);

    if (Found != TRUE)
    {
        if (UpdateExisting != FALSE)
        {
            UInt32ToIndexMapSetValueByIndex(Map,
                                            Index,
                                            Value);
            
            if (MapModified != NULL)
            {
                *MapModified = TRUE;
            }
        }
        else if (MapModified != NULL)
        {   
            *MapModified = FALSE;
        }

        return ISTATUS_SUCCESS;
    }

    if (Map->MapCapacity == Map->NumberOfMappings)
    {
        if (Map->MapCapacity == UINT32_TO_INDEX_MAP_MAX_CAPACITY)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        Status = CheckedAddSizeT(Map->MapCapacity,
                                 Map->MapCapacity,
                                 &NewCapacity);

        if (Status != ISTATUS_SUCCESS || NewCapacity > UINT32_TO_INDEX_MAP_MAX_CAPACITY)
        {
            NewCapacity = UINT32_TO_INDEX_MAP_MAX_CAPACITY; 
        }

        Allocation = realloc((PVOID) Map->Keys, NewCapacity * sizeof(UINT32));

        if (Allocation == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        Map->Keys = (PUINT32) Allocation;

        Allocation = realloc((PVOID) Map->Values, NewCapacity * sizeof(SIZE_T));

        if (Allocation == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }
        
        Map->Values = (PSIZE_T) Allocation;
    }

    if (MapModified != NULL)
    {   
        *MapModified = TRUE;
    }

    Map->Keys[Map->NumberOfMappings] = Key;
    Map->Values[Map->NumberOfMappings] = Value;
    Map->NumberOfMappings += 1;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
BOOL
UInt32ToIndexMapRemoveMapping(
    _Inout_ PUINT32_TO_INDEX_MAP Map,
    _In_ UINT32 Key
    )
{
    BOOL Found;
    SIZE_T Index;

    ASSERT(Map != NULL);
    ASSERT(Map->MapCapacity != 0);

    Found = UInt32ToIndexMapFindKeyIndex(Map,
                                         Key,
                                         &Index);

    if (Found == FALSE)
    {
        return FALSE;
    }

    Map->NumberOfMappings -= 1;

    for (;Index < Map->NumberOfMappings; Index++)
    {
        Map->Values[Index] = Map->Values[Index + 1];
        Map->Keys[Index] = Map->Keys[Index + 1];
    }

    return TRUE;
}

SFORCEINLINE
SIZE_T
UInt32ToIndexMapGetSize(
    _In_ PCUINT32_TO_INDEX_MAP Map
    )
{
    ASSERT(Map != NULL);

    return Map->NumberOfMappings;
}

SFORCEINLINE
VOID
UInt32ToIndexMapClear(
    _Inout_ PUINT32_TO_INDEX_MAP Map
    )
{
    ASSERT(Map != NULL);

    Map->NumberOfMappings = 0;
}

SFORCEINLINE
VOID
UInt32ToIndexMapDestroy(
    _Inout_ PUINT32_TO_INDEX_MAP Map
    )
{
    ASSERT(Map != NULL);

    free(Map->Keys);
    free(Map->Values);
    Map->NumberOfMappings = 0;
    Map->MapCapacity = 0;
}

#undef UINT32_TO_INDEX_MAP_INITIAL_CAPACITY
#undef UINT32_TO_INDEX_MAP_MAX_CAPACITY

#endif // _IRIS_COMMON_UINT32_TO_INDEX_MAP_
