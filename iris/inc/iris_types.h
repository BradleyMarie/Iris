/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_types.h

Abstract:

    This file contains the definitions for the types used by Iris. This module
    should only be needed when compiling for non-Windows systems.

    N.B. The on non-Windows systems, Iris uses the C boolean type, for its
         BOOL type.

--*/

#ifdef _MSC_VER

#include <windows.h>

#else

#include <stdint.h>
#include <stdbool.h>

#ifndef _TYPES_IRIS_
#define _TYPES_IRIS_

typedef float FLOAT;
typedef FLOAT *PFLOAT;

typedef uint8_t UINT8;
typedef UINT8 *PUINT8;

typedef uint16_t UINT16;
typedef UINT16 *PUINT16;

typedef uint32_t UINT32;
typedef UINT32 *PUINT32;

typedef uint64_t UINT64;
typedef UINT64 *PUINT64;

typedef int8_t INT8;
typedef INT8 *PINT8;

typedef int16_t INT16;
typedef INT16 *PINT16;

typedef int32_t INT32;
typedef INT32 *PINT32;

typedef int64_t INT64;
typedef INT64 *PINT64;

typedef size_t SIZE_T;
typedef SIZE_T *PSIZE_T;

#define VOID void
typedef VOID *PVOID;

typedef bool BOOL;
typedef BOOL *PBOOL;

#define TRUE true
#define FALSE false

#endif // _TYPES_IRIS_

#endif // _MSC_VER
