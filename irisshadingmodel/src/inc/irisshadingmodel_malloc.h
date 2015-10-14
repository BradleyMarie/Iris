/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_malloc.h

Abstract:

    This file contains the internal definitions for the wrapper memory routines.

--*/

#ifndef _MALLOC_IRIS_SHADING_MODEL_INTERNAL_
#define _MALLOC_IRIS_SHADING_MODEL_INTERNAL_

#include <irisshadingmodelp.h>

//
// Functions
//

_Check_return_
_Success_(return != NULL)
_Ret_maybenull_ 
_Post_writable_byte_size_(HeaderSizeInBytes)
SFORCEINLINE
PVOID
IrisAlignedMallocWithHeader(
    _Pre_satisfies_(HeaderSizeInBytes != 0 && HeaderSizeInBytes % HeaderAlignment == 0)  SIZE_T HeaderSizeInBytes,
    _Pre_satisfies_(HeaderAlignment != 0 && (HeaderAlignment & (HeaderAlignment - 1)) == 0) SIZE_T HeaderAlignment,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(DataSizeInBytes % DataAlignment == 0)) SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_((DataAlignment & (DataAlignment - 1)) == 0)) SIZE_T DataAlignment,
    _When_(DataSizeInBytes == 0 && Data != NULL, _Deref_post_bytecap_) _When_(DataSizeInBytes != 0, _Deref_post_bytecap_(DataSizeInBytes)) PVOID *Data
    )
{
    PVOID HeaderAllocation;
    BOOL Success;

    if (HeaderSizeInBytes == 0 ||
        HeaderAlignment == 0 ||
        HeaderAlignment & (HeaderAlignment - 1) ||
        HeaderSizeInBytes % HeaderAlignment != 0)
    {
        return NULL;
    }

    if (DataSizeInBytes != 0)
    {
        if (Data == NULL ||
            DataAlignment == 0 ||
            DataAlignment & DataAlignment - 1 ||
            DataSizeInBytes % DataAlignment != 0)
        {
            return NULL;
        }
    }

    Success = IrisAlignedAllocWithHeader(HeaderSizeInBytes,
                                         HeaderAlignment,
                                         &HeaderAllocation,
                                         DataSizeInBytes,
                                         DataAlignment,
                                         Data,
                                         NULL);

    if (Success == FALSE)
    {
        return NULL;
    }

    return HeaderAllocation;
}

#endif // _MALLOC_IRIS_SHADING_MODEL_INTERNAL_