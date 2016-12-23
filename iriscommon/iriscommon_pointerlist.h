/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iriscommon_pointerlist.h

Abstract:

    This module implements a variable sized list of pointers.

--*/

#ifndef _IRIS_COMMON_POINTER_LIST_
#define _IRIS_COMMON_POINTER_LIST_

#include <iris.h>
#include "iriscommon_types.h"

#define POINTER_LIST_INITIAL_SIZE 16
#define POINTER_LIST_GROWTH_FACTOR 2

//
// Types
//

typedef struct _POINTER_LIST {
    _Field_size_(PointerListCapacity) PVOID *PointerList;
    SIZE_T PointerListCapacity;
    SIZE_T PointerListSize;
} POINTER_LIST, *PPOINTER_LIST;

typedef const POINTER_LIST *PCPOINTER_LIST;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PointerListInitialize(
    _Out_ PPOINTER_LIST PointerList
    )
{
    PVOID *List;
    SIZE_T ListCapacityInBytes;
    SIZE_T ListCapacity;

	ASSERT(PointerList != NULL);

    ListCapacity = POINTER_LIST_INITIAL_SIZE;

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
PointerListPrepareToAddPointers(
    _Inout_ PPOINTER_LIST PointerList,
    _In_ SIZE_T NumberOfPointers
    )
{
    SIZE_T NewCapacityInBytes;
    SIZE_T NewCapacity;
    SIZE_T RequiredCapacityInBytes;
    SIZE_T RequiredCapacity;
    PVOID *ResizedList;
    SIZE_T Status;

    ASSERT(PointerList != NULL);
    ASSERT(PointerList->PointerListCapacity != 0);

    Status = CheckedAddSizeT(PointerList->PointerListSize, 
                             NumberOfPointers,
                             &RequiredCapacity);

    if (Status != ISTATUS_SUCCESS)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    if (RequiredCapacity <= PointerList->PointerListCapacity)
    {
        return ISTATUS_SUCCESS;
    }

    Status = CheckedMultiplySizeT(RequiredCapacity,
                                  sizeof(PVOID),
                                  &RequiredCapacityInBytes);

    if (Status != ISTATUS_SUCCESS)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = CheckedMultiplySizeT(PointerList->PointerListCapacity,
                                  POINTER_LIST_GROWTH_FACTOR,
                                  &NewCapacity);

    if (Status != ISTATUS_SUCCESS)
    {
        NewCapacity = RequiredCapacity;
        NewCapacityInBytes = RequiredCapacityInBytes;
    }
    else
    {
        Status = CheckedMultiplySizeT(NewCapacity,
                                      sizeof(PVOID),
                                      &NewCapacityInBytes);

        if (Status != ISTATUS_SUCCESS)
        {
            NewCapacity = RequiredCapacity;
            NewCapacityInBytes = RequiredCapacityInBytes;
        }
    }        
    
    ResizedList = realloc(PointerList->PointerList, NewCapacityInBytes);

    if (ResizedList == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PointerList->PointerList = ResizedList;
    PointerList->PointerListCapacity = NewCapacity;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
PointerListAddPointer(
    _Inout_ PPOINTER_LIST PointerList,
    _In_ PVOID Pointer
    )
{
    ISTATUS Status;

    ASSERT(PointerList != NULL);
    ASSERT(PointerList->PointerListCapacity != 0);

    Status = PointerListPrepareToAddPointers(PointerList, 1);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    PointerList->PointerList[PointerList->PointerListSize++] = Pointer;

    return ISTATUS_SUCCESS;
}

_When_(Index < PointerList->PointerListSize, _Ret_)
SFORCEINLINE
PVOID
PointerListRetrieveAtIndex(
    _In_ PCPOINTER_LIST PointerList,
    _In_ SIZE_T Index
    )
{
    ASSERT(PointerList != NULL);
    ASSERT(Index < PointerList->PointerListSize);

    return PointerList->PointerList[Index];
}

SFORCEINLINE
SIZE_T
PointerListGetSize(
    _In_ PCPOINTER_LIST PointerList
    )
{
    ASSERT(PointerList != NULL);

    return PointerList->PointerListSize;
}

SFORCEINLINE
VOID
PointerListGetData(
    _In_ PCPOINTER_LIST PointerList,
	_Outptr_result_buffer_(*Size) PVOID *Data[],
	_Out_ PSIZE_T Size
    )
{
    ASSERT(PointerList != NULL);
	ASSERT(Data != NULL);
	ASSERT(Size != NULL);

	*Data = PointerList->PointerList;
	*Size = PointerList->PointerListSize;
}

SFORCEINLINE
VOID
PointerListClear(
    _Inout_ PPOINTER_LIST PointerList
    )
{
    ASSERT(PointerList != NULL);

    PointerList->PointerListSize = 0;
}

SFORCEINLINE
VOID
PointerListDestroy(
    _Inout_ PPOINTER_LIST PointerList
    )
{
    ASSERT(PointerList != NULL);

    free(PointerList->PointerList);
    PointerList->PointerList = NULL;
}

SFORCEINLINE
VOID
PointerListSort(
    _Inout_ PPOINTER_LIST PointerList,
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

#undef POINTER_LIST_INITIAL_SIZE
#undef POINTER_LIST_GROWTH_FACTOR

#endif // _IRIS_COMMON_POINTER_LIST_