/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_object.h

Abstract:

    This module declares the internal ris object management routines.

--*/

#ifndef _IRIS_OBJECT_INTERNAL_
#define _IRIS_OBJECT_INTERNAL_

#include <irisp.h>

struct _IRIS_OBJECT {
    PFREE_ROUTINE FreeRoutine;
    SIZE_T DataSize;
    UINT8 Type;
};

_Check_return_
_Ret_maybenull_bytes_(ObjectSize)
PIRIS_OBJECT
IrisAllocate(
    PFREE_ROUTINE FreeRoutine,
    SIZE_T DataSize,
    UINT8 Type,
    __out PVOID *Data
    );

#endif // _IRIS_OBJECT_INTERNAL_