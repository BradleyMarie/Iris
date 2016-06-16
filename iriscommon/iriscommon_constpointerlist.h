/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iriscommon_constpointerlist.h

Abstract:

    This module implements a variable sized list of constant pointers.

--*/

#ifndef _IRIS_COMMON_CONSTANT_POINTER_LIST_
#define _IRIS_COMMON_CONSTANT_POINTER_LIST_

#include <iris.h>
#include "iriscommon_types.h"

#define CONSTANT_POINTER_LIST_INITIAL_SIZE 16
#define CONSTANT_POINTER_LIST_GROWTH_FACTOR 2

//
// Types
//

typedef struct _CONSTANT_POINTER_LIST {
    _Field_size_(PointerListCapacity) PCVOID *PointerList;
    SIZE_T PointerListCapacity;
    SIZE_T PointerListSize;
} CONSTANT_POINTER_LIST, *PCONSTANT_POINTER_LIST;

typedef CONST CONSTANT_POINTER_LIST *PCCONSTANT_POINTER_LIST;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
ConstantPointerListInitialize(
    _Out_ PCONSTANT_POINTER_LIST PointerList
    )
{
    PVOID *List;
    SIZE_T ListCapacityInBytes;
    SIZE_T ListCapacity;

	ASSERT(PointerList != NULL);

    ListCapacity = CONSTANT_POINTER_LIST_INITIAL_SIZE;

    ListCapacityInBytes = sizeof(PVOID) * ListCapacity;

    List = malloc(ListCapacityInBytes);

    if (List == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PointerList->PointerListCapacity = ListCapacity;
    PointerList->PointerListSize = 0;
    PointerList->PointerList = (PCVOID *) List;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
ConstantPointerListAddPointer(
    _Inout_ PCONSTANT_POINTER_LIST PointerList,
    _In_ PCVOID Pointer
    )
{
    SIZE_T NewCapacityInBytes;
    SIZE_T NewCapacity;
    PCVOID *ResizedList;

    ASSERT(PointerList != NULL);
    ASSERT(PointerList->PointerListCapacity != 0);

    if (PointerList->PointerListSize == PointerList->PointerListCapacity)
    {
        NewCapacity = CONSTANT_POINTER_LIST_GROWTH_FACTOR * 
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

_When_(Index < PointerList->PointerListSize, _Ret_)
SFORCEINLINE
PCVOID
ConstantPointerListRetrieveAtIndex(
    _In_ PCCONSTANT_POINTER_LIST PointerList,
    _In_ SIZE_T Index
    )
{
    ASSERT(PointerList != NULL);
    ASSERT(Index < PointerList->PointerListSize);

    return PointerList->PointerList[Index];
}

SFORCEINLINE
SIZE_T
ConstantPointerListGetSize(
    _In_ PCCONSTANT_POINTER_LIST PointerList
    )
{
    ASSERT(PointerList != NULL);

    return PointerList->PointerListSize;
}

SFORCEINLINE
VOID
ConstantPointerListClear(
    _Inout_ PCONSTANT_POINTER_LIST PointerList
    )
{
    ASSERT(PointerList != NULL);

    PointerList->PointerListSize = 0;
}

SFORCEINLINE
VOID
ConstantPointerListDestroy(
    _Inout_  PCONSTANT_POINTER_LIST PointerList
    )
{
    ASSERT(PointerList != NULL);

    free((PVOID)PointerList->PointerList);
    PointerList->PointerList = NULL;
}

SFORCEINLINE
VOID
ConstantPointerListSort(
    _Inout_ PCONSTANT_POINTER_LIST PointerList,
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

#undef CONSTANT_POINTER_LIST_INITIAL_SIZE
#undef CONSTANT_POINTER_LIST_GROWTH_FACTOR

#endif // _IRIS_COMMON_CONSTANT_POINTER_LIST_