/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_types.h

Abstract:

    This file contains the definitions for the types used by Iris.

--*/

#ifdef _MSC_VER
#include "iris_types_windows.h"
#else
#include "iris_types_others.h"
#endif // _MSC_VER

#ifndef _TYPES_IRIS_
#define _TYPES_IRIS_

typedef CONST FLOAT *PCFLOAT;
typedef CONST UINT8 *PCUINT8;
typedef CONST UINT16 *PCUINT16;
typedef CONST UINT32 *PCUINT32;
typedef CONST UINT64 *PCUINT64;

typedef CONST INT8 *PCINT8;
typedef CONST INT16 *PCINT16;
typedef CONST INT32 *PCINT32;
typedef CONST INT64 *PCINT64;

typedef CONST SIZE_T *PCSIZE_T;

typedef CONST VOID *PCVOID;

typedef CONST BOOL *PCBOOL;

typedef int COMPARISON_RESULT;

typedef
VOID
(*PFREE_ROUTINE)(
    _Pre_maybenull_ _Post_invalid_ PVOID Pointer
    );

#endif // _TYPES_IRIS_