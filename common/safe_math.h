/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    safe_math.h

Abstract:

    Safe arithmetic routines with overflow checking.

--*/

#ifndef _COMMON_SAFE_MATH_
#define _COMMON_SAFE_MATH_

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "common/sal.h"

//
// Functions
//

_Check_return_
_Success_(return != 0)
static
inline
bool
CheckedAddSizeT(
    _In_ size_t addend0,
    _In_ size_t addend1,
    _Out_ size_t *sum
    )
{
    assert(sum != NULL);

    if (SIZE_MAX - addend0 < addend1)
    {
        return false;
    }

    *sum = addend0 + addend1;

    return true;
}

_Check_return_
_Success_(return != 0)
static
inline
bool
CheckedMultiplySizeT(
    _In_ size_t multiplicand0,
    _In_ size_t multiplicand1,
    _Out_ size_t *product
    )
{
    assert(product != NULL);

    size_t unchecked_product = multiplicand0 * multiplicand1;

    if (multiplicand0 != 0 &&
        unchecked_product / multiplicand0 != multiplicand1)
    {
        return false;
    }

    *product = unchecked_product;

    return true;
}

#endif // _COMMON_SAFE_MATH_