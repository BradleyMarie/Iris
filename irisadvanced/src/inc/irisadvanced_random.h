/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvanced_random.h

Abstract:

    Internal definitions and declarations for RANDOM type.

--*/

#ifndef _IRIS_ADVANCED_RANDOM_INTERNAL_
#define _IRIS_ADVANCED_RANDOM_INTERNAL_

#include <irisadvancedp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RandomAllocate(
    _In_ PCRANDOM_VTABLE RandomVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PRANDOM *Rng
    );

VOID
RandomFree(
    _In_opt_ _Post_invalid_ PRANDOM Rng
    );

#endif // _IRIS_ADVANCED_RANDOM_INTERNAL_
