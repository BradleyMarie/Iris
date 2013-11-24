/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_assert.h

Abstract:

    This module implements the Iris assertion routines. This module should 
    only be needed when compiling for non-Windows systems.

--*/

#include <assert.h>

#ifndef _ASSERT_IRIS_
#define _ASSERT_IRIS_

#define ASSERT(expr) assert(expr)

#endif // _ASSERT_IRIS_