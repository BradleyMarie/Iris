/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    free_routine.h

Abstract:

    Function pointer type for free callbacks.

--*/

#ifndef _COMMON_FREE_ROUTINE_
#define _COMMON_FREE_ROUTINE_

#include "common/sal.h"

//
// Macros
//

typedef
void
(*PFREE_ROUTINE)(
    _In_opt_ _Post_invalid_ void *pointer
    );

#endif // _COMMON_FREE_ROUTINE_