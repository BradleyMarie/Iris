/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_constpointerlist.h

Abstract:

    This module implements a variable sized list of constant pointers.

--*/

#ifndef _IRIS_CONSTANT_POINTER_LIST_INTERNAL_
#define _IRIS_CONSTANT_POINTER_LIST_INTERNAL_

#include <irisp.h>

#define IRIS_CONSTANT_POINTER_LIST_INITIAL_SIZE 16
#define IRIS_CONSTANT_POINTER_LIST_GROWTH_FACTOR 2

//
// Types
//

typedef struct _IRIS_CONSTANT_POINTER_LIST {
    _Field_size_(PointerListCapacity) PCVOID *PointerList;
    SIZE_T PointerListCapacity;
    SIZE_T PointerListSize;
} IRIS_CONSTANT_POINTER_LIST, *PIRIS_CONSTANT_POINTER_LIST;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
IrisConstantPointerListInitialize(
    _Out_ PIRIS_CONSTANT_POINTER_LIST PointerList
    )
{
    PVOID *List;
    SIZE_T ListCapacityInBytes;
    SIZE_T ListCapacity;

	ASSERT(PointerList != NULL);

    ListCapacity = IRIS_CONSTANT_POINTER_LIST_INITIAL_SIZE;

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
_Ret_maybenull_
SFORCEINLINE
ISTATUS
IrisConstantPointerListAddPointer(
    _Inout_ PIRIS_CONSTANT_POINTER_LIST PointerList,
    _In_ PCVOID Pointer
    )
{
    SIZE_T NewCapacityInBytes;
    SIZE_T NewCapacity;
    PCVOID *ResizedList;

    ASSERT(PointerList != NULL);

    if (PointerList->PointerListSize == PointerList->PointerListCapacity)
    {
        NewCapacity = IRIS_CONSTANT_POINTER_LIST_GROWTH_FACTOR * 
                      PointerList->PointerListCapacity;

        NewCapacityInBytes = NewCapacity * sizeof(PCVOID);

        ResizedList = realloc((PVOID)PointerList->PointerList, NewCapacityInBytes);

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

SFORCEINLINE
PCVOID
IrisConstantPointerListRetrieveAtIndex(
    _In_ PIRIS_CONSTANT_POINTER_LIST PointerList,
    _In_ SIZE_T Index
    )
{
    ASSERT(PointerList != NULL);
    ASSERT(Index < PointerList->PointerListSize);

    return PointerList->PointerList[Index];
}

SFORCEINLINE
PCVOID*
IrisConstantPointerListGetStorage(
    _In_ PIRIS_CONSTANT_POINTER_LIST PointerList
    )
{
    ASSERT(PointerList != NULL);

    return PointerList->PointerList;
}

SFORCEINLINE
SIZE_T
IrisConstantPointerListGetSize(
    _In_ PIRIS_CONSTANT_POINTER_LIST PointerList
    )
{
    ASSERT(PointerList != NULL);

    return PointerList->PointerListSize;
}

SFORCEINLINE
VOID
IrisConstantPointerListClear(
    _Inout_ PIRIS_CONSTANT_POINTER_LIST PointerList
    )
{
    ASSERT(PointerList != NULL);

    PointerList->PointerListSize = 0;
}

SFORCEINLINE
VOID
IrisConstantPointerListDestroy(
    _Inout_ PIRIS_CONSTANT_POINTER_LIST PointerList
    )
{
    ASSERT(PointerList != NULL);

    free((PVOID)PointerList->PointerList);
}

SFORCEINLINE
VOID
IrisConstantPointerListSort(
    _Inout_ PIRIS_CONSTANT_POINTER_LIST PointerList,
    _In_ PLIST_SORT_ROUTINE SortRoutine
    )
{
    ASSERT(PointerList != NULL);
    ASSERT(SortRoutine != NULL);

    qsort((PVOID)PointerList->PointerList,
          PointerList->PointerListSize,
          sizeof(PCVOID),
          SortRoutine);
}

#undef IRIS_CONSTANT_POINTER_LIST_INITIAL_SIZE
#undef IRIS_CONSTANT_POINTER_LIST_GROWTH_FACTOR

#endif // _IRIS_CONSTANT_POINTER_LIST_INTERNAL_