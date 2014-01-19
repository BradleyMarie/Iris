/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_normal.h

Abstract:

    This file contains the definitions for the NORMAL type.

--*/

#ifndef _NORMAL_IRIS_TOOLKIT_
#define _NORMAL_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Types
//

typedef struct _NORMAL {
    PVOID Shader;
} NORMAL, *PNORMAL;

typedef CONST NORMAL *PCNORMAL;

#endif // _NORMAL_IRIS_TOOLKIT_