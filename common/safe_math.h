/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    safe_math.h

Abstract:

    Safe arithmetic routines with overflow checking.

--*/

#include "iris/sal.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef _COMMON_SAFE_MATH_
#define _COMMON_SAFE_MATH_

//
// Functions
//

static
inline
ISTATUS
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

static
inline
ISTATUS
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

#endif // _COMMON_SAFE_MATH_