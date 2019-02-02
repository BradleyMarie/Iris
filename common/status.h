/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    status.h

Abstract:

    The status type and constants used by Iris.

--*/

#ifndef _COMMON_STATUS_
#define _COMMON_STATUS_

#include "common/sal.h"

typedef _Return_type_success_(return == 0) enum _ISTATUS {
    ISTATUS_SUCCESS           = 0x00,
    ISTATUS_NO_RESULT         = 0x01,
    ISTATUS_DONE              = 0x01,
    ISTATUS_ALLOCATION_FAILED = 0x02,
    ISTATUS_ARITHMETIC_ERROR  = 0x03,
    ISTATUS_INTEGER_OVERFLOW  = 0x04,
    ISTATUS_IO_ERROR          = 0x05,
    ISTATUS_INVALID_ARGUMENT_00 = 0x80,
    ISTATUS_INVALID_ARGUMENT_01 = 0x81,
    ISTATUS_INVALID_ARGUMENT_02 = 0x82,
    ISTATUS_INVALID_ARGUMENT_03 = 0x83,
    ISTATUS_INVALID_ARGUMENT_04 = 0x84,
    ISTATUS_INVALID_ARGUMENT_05 = 0x85,
    ISTATUS_INVALID_ARGUMENT_06 = 0x86,
    ISTATUS_INVALID_ARGUMENT_07 = 0x87,
    ISTATUS_INVALID_ARGUMENT_08 = 0x88,
    ISTATUS_INVALID_ARGUMENT_09 = 0x89,
    ISTATUS_INVALID_ARGUMENT_10 = 0x8A,
    ISTATUS_INVALID_ARGUMENT_11 = 0x8B,
    ISTATUS_INVALID_ARGUMENT_12 = 0x8C,
    ISTATUS_INVALID_ARGUMENT_13 = 0x8D,
    ISTATUS_INVALID_ARGUMENT_14 = 0x8E,
    ISTATUS_INVALID_ARGUMENT_15 = 0x8F,
    ISTATUS_INVALID_ARGUMENT_16 = 0x90,
    ISTATUS_INVALID_ARGUMENT_17 = 0x91,
    ISTATUS_INVALID_ARGUMENT_18 = 0x92,
    ISTATUS_INVALID_ARGUMENT_19 = 0x93,
    ISTATUS_INVALID_ARGUMENT_20 = 0x94,
    ISTATUS_INVALID_ARGUMENT_21 = 0x95,
    ISTATUS_INVALID_ARGUMENT_22 = 0x96,
    ISTATUS_INVALID_ARGUMENT_23 = 0x97,
    ISTATUS_INVALID_ARGUMENT_24 = 0x98,
    ISTATUS_INVALID_ARGUMENT_25 = 0x99,
    ISTATUS_INVALID_ARGUMENT_26 = 0x9A,
    ISTATUS_INVALID_ARGUMENT_27 = 0x9B,
    ISTATUS_INVALID_ARGUMENT_28 = 0x9C,
    ISTATUS_INVALID_ARGUMENT_29 = 0x9D,
    ISTATUS_INVALID_ARGUMENT_30 = 0x9E,
    ISTATUS_INVALID_ARGUMENT_31 = 0x9F,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_00 = 0xA0,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_01 = 0xA1,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_02 = 0xA2,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_03 = 0xA3,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_04 = 0xA4,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_05 = 0xA5,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_06 = 0xA6,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_07 = 0xA7,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_08 = 0xA8,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_09 = 0xA9,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_10 = 0xAA,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_11 = 0xAB,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_12 = 0xAC,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_13 = 0xAD,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_14 = 0xAE,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_15 = 0xAF,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_16 = 0xB0,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_17 = 0xB1,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_18 = 0xB2,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_19 = 0xB3,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_20 = 0xB4,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_21 = 0xB5,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_22 = 0xB6,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_23 = 0xB7,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_24 = 0xB8,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_25 = 0xB9,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_26 = 0xBA,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_27 = 0xBB,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_28 = 0xBC,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_29 = 0xBD,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_30 = 0xBE,
    ISTATUS_INVALID_ARGUMENT_COMBINATION_31 = 0xBF,
    ISTATUS_INVALID_RESULT = 0xFF,
} ISTATUS;

#endif // _COMMON_STATUS_