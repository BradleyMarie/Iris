/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_pointerlist.h

Abstract:

    This module implements a variable sized list of pointers.

--*/

#ifndef _IRIS_POINTER_LIST_INTERNAL_
#define _IRIS_POINTER_LIST_INTERNAL_

#include <iris.h>
#include "iris_types.h"

#define IRIS_POINTER_LIST_INITIAL_SIZE 16
#define IRIS_POINTER_LIST_GROWTH_FACTOR 2

//
// Types
//

typedef struct _IRIS_POINTER_LIST {
    _Field_size_(PointerListCapacity) PVOID *PointerList;
    SIZE_T PointerListCapacity;
    SIZE_T PointerListSize;
} IRIS_POINTER_LIST, *PIRIS_POINTER_LIST;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
IrisPointerListInitialize(
    _Out_ PIRIS_POINTER_LIST PointerList
    )
{
    PVOID *List;
    SIZE_T ListCapacityInBytes;
    SIZE_T ListCapacity;

	ASSERT(PointerList != NULL);

    ListCapacity = IRIS_POINTER_LIST_INITIAL_SIZE;

    ListCapacityInBytes = sizeof(PVOID) * ListCapacity;

    List = malloc(ListCapacityInBytes);

    if (List == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PointerList->PointerListCapacity = ListCapacity;
    PointerList->PointerListSize = 0;
    PointerList->PointerList = List;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
IrisPointerListAddPointer(
    _Inout_ PIRIS_POINTER_LIST PointerList,
    _In_ PVOID Pointer
    )
{
    SIZE_T NewCapacityInBytes;
    SIZE_T NewCapacity;
    PVOID *ResizedList;

    ASSERT(PointerList != NULL);

    if (PointerList->PointerListSize == PointerList->PointerListCapacity)
    {
        NewCapacity = IRIS_POINTER_LIST_GROWTH_FACTOR * 
                      PointerList->PointerListCapacity;

        NewCapacityInBytes = NewCapacity * sizeof(PVOID);

        ResizedList = realloc(PointerList->PointerList, NewCapacityInBytes);

        if (ResizedList == NULL)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        PointerList->PointerList = ResizedList;
        PointerList->PointerListCapacity = NewCapacity;
    }

    PointerList->PointerList[PointerList->PointerListSize++] = Pointer;

    return ISTATUS_SUCCESS;
}

_When_(Index < PointerList->PointerListSize, _Ret_)
SFORCEINLINE
PVOID
IrisPointerListRetrieveAtIndex(
    _In_ PIRIS_POINTER_LIST PointerList,
    _In_ SIZE_T Index
    )
{
    ASSERT(PointerList != NULL);
    ASSERT(Index < PointerList->PointerListSize);

    return PointerList->PointerList[Index];
}

SFORCEINLINE
SIZE_T
IrisPointerListGetSize(
    _In_ PIRIS_POINTER_LIST PointerList
    )
{
    ASSERT(PointerList != NULL);

    return PointerList->PointerListSize;
}

SFORCEINLINE
VOID
IrisPointerListClear(
    _Inout_ PIRIS_POINTER_LIST PointerList
    )
{
    ASSERT(PointerList != NULL);

    PointerList->PointerListSize = 0;
}

SFORCEINLINE
VOID
IrisPointerListDestroy(
    _Inout_ PIRIS_POINTER_LIST PointerList
    )
{
    ASSERT(PointerList != NULL);

    free(PointerList->PointerList);
    PointerList->PointerList = NULL;
}

SFORCEINLINE
VOID
IrisPointerListSort(
    _Inout_ PIRIS_POINTER_LIST PointerList,
    _In_ PLIST_SORT_ROUTINE SortRoutine
    )
{
    ASSERT(PointerList != NULL);
    ASSERT(SortRoutine != NULL);

    qsort(PointerList->PointerList,
          PointerList->PointerListSize,
          sizeof(PCVOID),
          SortRoutine);
}

#undef IRIS_POINTER_LIST_INITIAL_SIZE
#undef IRIS_POINTER_LIST_GROWTH_FACTOR

#endif // _IRIS_POINTER_LIST_INTERNAL_