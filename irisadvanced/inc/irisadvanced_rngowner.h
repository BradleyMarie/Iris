/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisadvanced_random.h

Abstract:

    This file contains the definitions for the RANDOM type.

--*/

#ifndef _RANDOM_OWNER_IRIS_ADVANCED_
#define _RANDOM_OWNER_IRIS_ADVANCED_

#include <irisadvanced.h>

//
// Types
//

typedef struct _RANDOM_OWNER RANDOM_OWNER, *PRANDOM_OWNER;
typedef CONST RANDOM_OWNER *PCRANDOM_OWNER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDAPI
ISTATUS
RandomOwnerAllocate(
    _In_ PCRANDOM_VTABLE RandomVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PRANDOM_OWNER *RandomOwner
    );

_Ret_
IRISADVANCEDAPI
PRANDOM
RandomOwnerGetRandom(
    _In_ PRANDOM_OWNER RandomOwner
    );

IRISADVANCEDAPI
VOID
RandomOwnerFree(
    _In_opt_ _Post_invalid_ PRANDOM_OWNER RandomOwner
    );

#endif // _RANDOM_OWNER_IRIS_ADVANCED_