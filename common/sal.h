/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    sal.h

Abstract:

    Turns Microsoft SAL annotations into no-ops if not detected as available.

--*/

#ifndef _COMMON_SAL_
#define _COMMON_SAL_

#ifndef _In_

#define _In_
#define _In_opt_
#define _In_range_(low, hi)
#define _In_reads_(size)
#define _In_reads_opt_(size)
#define _In_reads_bytes_(size)
#define _In_reads_bytes_opt_(size)

#define _Field_size_(size) 
#define _Field_size_bytes_(size) 
#define _Field_size_bytes_opt_(size) 
#define _Field_size_full_(size) 
#define _Field_size_full_opt_(size) 

#define _Struct_size_bytes_(size) 

#define _Inout_
#define _Inout_opt_
#define _Inout_updates_(size)

#define _Pre_bytecount_(size)
#define _Pre_invalid_
#define _Pre_null_
#define _Pre_maybenull_
#define _Post_valid_
#define _Post_invalid_

#define _Pre_writable_byte_size_(size)
#define _Post_writable_byte_size_(size)

#define _Out_
#define _Out_opt_
#define _Out_writes_(size)
#define _Out_range_(min, max)

#define _Outptr_
#define _Outptr_result_buffer_(size)
#define _Outptr_result_bytebuffer_(size)
#define _Outptr_result_bytebuffer_to_(s,c)
#define _Outptr_result_maybenull_
#define _Outptr_opt_result_bytebuffer_(size)
#define _Outptr_opt_result_bytebuffer_maybenull_(size)

#define _Deref_post_null_

#define _Ret_
#define _Ret_opt_
#define _Ret_range_(min, max)
#define _Return_type_success_(expr)

#define _Success_(expr)
#define _Check_return_
#define _Ret_maybenull_

#define _When_(expr0,expr1)
#define _Satisfies_(exor0)
#define _Pre_satisfies_(exor0)

#endif // #ifndef _In_

#endif // _COMMON_SAL_