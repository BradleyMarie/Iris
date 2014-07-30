/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_constantnormal.h

Abstract:

    This file contains the prototypes for the CONSTANT_NORMAL type.

--*/

#ifndef _CONSTANT_NORMAL_IRIS_TOOLKIT_
#define _CONSTANT_NORMAL_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PNORMAL
ConstantNormalAllocate(
    _In_ VECTOR3 Normal,
    _In_ BOOL Normalize
    );

#endif // _CONSTANT_NORMAL_IRIS_TOOLKIT_