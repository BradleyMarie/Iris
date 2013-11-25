/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_object.h

Abstract:

    This module declares the iris object management routines.

--*/

typedef VOID (*PFREE_ROUTINE)(_Pre_maybenull_ _Post_invalid_ PVOID);

_Check_return_
_Ret_maybenull_bytes_(ObjectSize)
PVOID
IrisAllocate(
    PFREE_ROUTINE FreeRoutine,
    PVOID Data,
    SIZE_T DataSize
    );