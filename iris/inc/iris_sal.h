/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_sal.h

Abstract:

    This file contains the definitions for the Microsoft SAL annotations
    needed by Iris. This module should only be needed when compiling for
    non-Windows systems.

--*/

#ifdef WIN32

#include <sal.h>

#else

#ifndef _SAL_IRIS_
#define _SAL_IRIS_

#define __in const
#define __in_range(min, max) const
#define __in_ecount(size) const

#define __inout
#define __inout_ecount(size)

#define __out
#define __out_ecount(size)

#define __success(expr)

#endif // _SAL_IRIS_

#endif // WIN32