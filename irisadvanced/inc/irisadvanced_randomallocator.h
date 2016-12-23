/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvanced_randomallocator.h

Abstract:

    This file contains the definitions for the RANDOM_ALLOCATOR type.

--*/

#ifndef _RANDOM_ALLOCATOR_IRIS_ALLOCATOR_
#define _RANDOM_ALLOCATOR_IRIS_ALLOCATOR_

#include <irisadvanced.h>

//
// Types
//

typedef struct _RANDOM_ALLOCATOR RANDOM_ALLOCATOR, *PRANDOM_ALLOCATOR;
typedef CONST RANDOM_ALLOCATOR *PCRANDOM_ALLOCATOR;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
RandomAllocatorAllocateRandom(
    _Inout_ PRANDOM_ALLOCATOR RandomAllocator,
    _In_ PCRANDOM_VTABLE RandomVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PRANDOM *Rng
    );

#endif // _RANDOM_ALLOCATOR_IRIS_ALLOCATOR_