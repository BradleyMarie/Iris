/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iris_malloc.h

Abstract:

    This file contains the prototypes for the aligned Iris malloc/free.

--*/

#include <iris.h>

#ifndef _MALLOC_IRIS_
#define _MALLOC_IRIS_

//
// Prototypes
//

_Check_return_
_Success_(return != NULL)
_Ret_maybenull_ 
_Post_writable_byte_size_(HeaderSizeInBytes)
IRISAPI
PVOID
IrisAlignedMallocWithHeader(
    _In_ SIZE_T HeaderSizeInBytes,
    _In_ SIZE_T HeaderAlignment,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment,
    _When_(DataSizeInBytes == 0, _Deref_post_null_) _When_(DataSizeInBytes != 0, _Outptr_result_bytebuffer_(DataSizeInBytes)) PVOID *DataPointer
    );

_Check_return_
_Ret_maybenull_ 
_Post_writable_byte_size_(SizeInBytes)
IRISAPI
PVOID
IrisAlignedMalloc(
    _In_ SIZE_T SizeInBytes,
    _In_ SIZE_T Alignment
    );

IRISAPI
VOID
IrisAlignedFree(
    _Pre_maybenull_ _Post_invalid_ PVOID Data
    );

#endif // _MALLOC_IRIS_