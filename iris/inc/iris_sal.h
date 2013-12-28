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

#define _Struct_size_bytes_(size) 

#define _Inout_
#define _Inout_updates_(size)

#define _Pre_maybenull_
#define _Post_invalid_

#define _Post_writable_byte_size_(size)

#define _Out_
#define _Out_writes_(size)

#define _Outptr_result_buffer_(size)
#define _Outptr_result_maybenull_
#define _Outptr_opt_result_bytebuffer_(size)
#define _Outptr_opt_result_bytebuffer_maybenull_(size)

#define _Success_(expr)
#define _Check_return_
#define _Ret_maybenull_

#endif // _SAL_IRIS_

#endif // _MSC_VER