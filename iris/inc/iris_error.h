/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_error.h

Abstract:

    This file contains the definitions for the Iris error types

--*/

#include <iris.h>

#ifndef _ERROR_IRIS_
#define _ERROR_IRIS_

typedef UINT8 ISTATUS;

#define ISTATUS_SUCCESS           0x00
#define ISTATUS_ALLOCATION_FAILED 0x01
#define ISTATUS_INVALID_ARGUMENT  0x02
#define ISTATUS_ARITHMETIC_ERROR  0x03
#define ISTATUS_INTEGER_OVERFLOW  0x04
#define ISTATUS_NO_MORE_DATA      0x05

#endif // _ERROR_IRIS_