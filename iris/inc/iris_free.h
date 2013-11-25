/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_free.h

Abstract:

    This module declares the public Iris free routine.

--*/

#ifndef _IRIS_FREE_
#define _IRIS_FREE_

#include <iris.h>

typedef struct _IRIS_OBJECT_VTABLE {
    VOID (*FreeRoutine)(_Pre_maybenull_ _Post_invalid_ PVOID);
    SIZE_T ObjectSize;
} IRIS_OBJECT_VTABLE, *PIRIS_OBJECT_VTABLE;

_Struct_size_bytes_(ObjectVTable->ObjectSize)
typedef struct _IRIS_OBJECT_HEADER {
    PIRIS_OBJECT_VTABLE ObjectVTable;
} IRIS_OBJECT_HEADER, *PIRIS_OBJECT_HEADER;

SFORCEINLINE
VOID
IrisFree(
    _Pre_maybenull_ _Post_invalid_ PIRIS_OBJECT_HEADER IrisObject
    )
{
    if (IrisObject == NULL)
    {
        return;
    }

    IrisObject->ObjectVTable.FreeRoutine(IrisObject);
}

#endif // _IRIS_FREE_