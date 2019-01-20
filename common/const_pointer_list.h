/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    const_pointer_list.h

Abstract:

    A variable sized list of constant pointers.

--*/

#ifndef _COMMON_CONST_POINTER_LIST_
#define _COMMON_CONST_POINTER_LIST_

#include <stdlib.h>

#include "common/safe_math.h"

//
// Defines
//

#define CONSTANT_POINTER_LIST_INITIAL_CAPACITY 16
#define CONSTANT_POINTER_LIST_GROWTH_FACTOR 2

//
// Types
//

typedef struct _CONSTANT_POINTER_LIST {
    _Field_size_(capacity) const void **list;
    size_t capacity;
    size_t size;
} CONSTANT_POINTER_LIST, *PCONSTANT_POINTER_LIST;

typedef const CONSTANT_POINTER_LIST *PCCONSTANT_POINTER_LIST;

//
// Functions
//

_Check_return_
_Success_(return != 0)
static
inline
bool
ConstantPointerListInitialize(
    _Out_ PCONSTANT_POINTER_LIST list
    )
{
	assert(list != NULL);

    void *allocation = calloc(CONSTANT_POINTER_LIST_INITIAL_CAPACITY,
                              sizeof(const void *));

    if (allocation == NULL)
    {
        return false;
    }

    list->list = (const void **) allocation;
    list->capacity = CONSTANT_POINTER_LIST_INITIAL_CAPACITY;
    list->size = 0;

    return true;
}

_Check_return_
_Success_(return != 0)
static
inline
bool
ConstantPointerListAddPointer(
    _Inout_ PCONSTANT_POINTER_LIST list,
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

        size_t new_capacity_in_bytes;
        success = CheckedMultiplySizeT(new_capacity,
                                       sizeof(const void*),
                                       &new_capacity_in_bytes);
        
        if (!success)
        {
            return false;
        }

        void *new_list = realloc((void *)list->list, new_capacity_in_bytes);

        if (new_list == NULL)
        {
            return false;
        }

        list->list = (const void **)new_list;
        list->capacity = new_capacity;
    }

    list->list[list->size++] = pointer;

    return true;
}

_Ret_
static
inline
const void *
ConstantPointerListRetrieveAtIndex(
    _In_ PCCONSTANT_POINTER_LIST list,
    _Satisfies_(_Curr_ >= 0 && _Curr_ < list->size) size_t index
    )
{
    assert(list != NULL);
    assert(index < list->size);

    return list->list[index];
}

static
inline
size_t
ConstantPointerListGetSize(
    _In_ PCCONSTANT_POINTER_LIST list
    )
{
    assert(list != NULL);

    return list->size;
}

static
inline
void
ConstantPointerListGetData(
    _In_ PCCONSTANT_POINTER_LIST list,
    _Outptr_result_buffer_(*size) const void ***entries,
    _Out_ size_t *size
    )
{
    assert(list != NULL);
    assert(entries != NULL);
    assert(size != NULL);

    *entries = list->list;
    *size = list->size;
}

static
inline
void
ConstantPointerListClear(
    _Inout_ PCONSTANT_POINTER_LIST list
    )
{
    assert(list != NULL);

    list->size = 0;
}

static
inline
void
ConstantPointerListDestroy(
    _Inout_  PCONSTANT_POINTER_LIST list
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
void
ConstantPointerListSort(
    _Inout_ PCONSTANT_POINTER_LIST list,
    _In_ int (*compar)(const void*,const void*)
    )
{
    assert(list != NULL);
    assert(compar != NULL);

    qsort((void *)list->list,
          list->size,
          sizeof(const void *),
          compar);
}

#undef CONSTANT_POINTER_LIST_INITIAL_SIZE
#undef CONSTANT_POINTER_LIST_GROWTH_FACTOR

#endif // _COMMON_CONST_POINTER_LIST_