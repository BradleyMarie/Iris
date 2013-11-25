/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_sal.h

Abstract:

    This file contains the definitions for the Microsoft SAL annotations
    needed by Iris. This module should only be needed when compiling for
    non-Windows systems.

--*/

#ifdef _MSC_VER

#include <sal.h>

#else

#ifndef _SAL_IRIS_
#define _SAL_IRIS_

#define _In_ const
#define _In_range_(low, hi) const
#define _In_reads_(size) const
#define _In_reads_bytes_opt_(size) const

#define _Field_size_(size) 
#define _Field_size_bytes_opt_(size) 

#define _Inout_
#define _Inout_updates_(size)

#define _Pre_maybenull_
#define _Post_invalid_

#define _Out_
#define _Out_writes_(size)

#define _Success_(expr)

#endif // _SAL_IRIS_

#endif // _MSC_VER