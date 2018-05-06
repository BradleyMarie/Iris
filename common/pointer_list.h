/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    pointer_list.h

Abstract:

    A variable sized list of mutable pointers.

--*/

#ifndef _COMMON_POINTER_LIST_
#define _COMMON_POINTER_LIST_

#include "common/safe_math.h"

//
// Defines
//

#define POINTER_LIST_INITIAL_SIZE 16
#define POINTER_LIST_GROWTH_FACTOR 2

//
// Types
//

typedef struct _POINTER_LIST {
    _Field_size_(capacity) const void **list;
    size_t capacity;
    size_t size;
} POINTER_LIST, *PPOINTER_LIST;

typedef const POINTER_LIST *PCPOINTER_LIST;

//
// Functions
//

_Check_return_
_Success_(return != 0)
static
inline
bool
PointerListInitialize(
    _Out_ PPOINTER_LIST list
    )
{
	assert(list != NULL);

    void *allocation = calloc(POINTER_LIST_INITIAL_SIZE,
                              sizeof(const void *));

    if (allocation == NULL)
    {
        return false;
    }

    list->list = (void **) allocation;
    list->capacity = POINTER_LIST_INITIAL_SIZE;
    list->size = 0;

    return true;
}

_Check_return_
_Success_(return != 0)
static
inline
bool
PointerListAddPointer(
    _Inout_ PPOINTER_LIST list,
    _In_ const void *pointer
    )
{
    assert(list != NULL);
    assert(list->list != NULL);
    assert(list->capacity != 0);

    if (list->size == list->capacity)
    {
        size_t new_capacity;
        bool success = CheckedMultiplySizeT(list->capacity,
                                            CONSTANT_POINTER_LIST_GROWTH_FACTOR,
                                            &new_capacity);
        
        if (!success)
        {
            return false;
        }

        void *new_list = realloc((void *)list->list, new_capacity);

        if (new_list == NULL)
        {
            return false;
        }

        list->list = new_list;
        list->capacity = new_capacity;
    }

    list->list[list->size++] = pointer;

    return true;
}

_Check_return_
_Success_(return != 0)
static
inline
bool
PointerListPrepareToAddPointers(
    _Inout_ PPOINTER_LIST list,
    _In_ size_t number_of_pointers
    )
{
    assert(list != NULL);

    size_t original_size = list->size;

    for (size_t i = 0; i < number_of_pointers; i++)
    {
        bool success = PointerListAddPointer(list, NULL);
        
        if (!success)
        {
            return false;
        }
    }

    list->size = original_size;

    return true;
}

_Ret_
static
inline
const void *
PointerListRetrieveAtIndex(
    _In_ PCPOINTER_LIST list,
    _In_ _Satisfies_(_Curr_ >= 0 && _Curr_ < list->size) size_t index
    )
{
    assert(list != NULL);
    assert(index < list->size);

    return list->list[index];
}

static
inline
size_t
PointerListGetSize(
    _In_ PCPOINTER_LIST list
    )
{
    assert(list != NULL);

    return list->size;
}

static
inline
void
PointerListClear(
    _Inout_ PPOINTER_LIST list
    )
{
    assert(list != NULL);

    list->size = 0;
}

static
inline
void
PointerListDestroy(
    _Inout_  PPOINTER_LIST list
    )
{
    assert(list != NULL);

    free((void *)list->list);
    list->list = NULL;
    list->capacity = 0;
    list->size = 0;
}

static
inline
VOID
PointerListSort(
    _Inout_ PPOINTER_LIST list,
    _In_ int (*compar)(const void*,const void*)
    )
{
    assert(list != NULL);
    assert(compar != NULL);

    qsort((void *)list->list,
          list->size,
          sizeof(void *),
          SortRoutine);
}

#undef POINTER_LIST_INITIAL_SIZE
#undef POINTER_LIST_GROWTH_FACTOR

#endif // _COMMON_POINTER_LIST_