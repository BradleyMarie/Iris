/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    utility.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ utility routines.

--*/

#include <irisplusplusp.h>
#include <vector>

namespace Iris {

//
// Data Structures
//

static std::vector<const char *> StatusNames {
    "ISTATUS_SUCCESS",
    "ISTATUS_ALLOCATION_FAILED",
    "ISTATUS_ARITHMETIC_ERROR",
    "ISTATUS_INTEGER_OVERFLOW",
    "ISTATUS_NO_MORE_DATA",
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    "ISTATUS_INVALID_ARGUMENT_00",
    "ISTATUS_INVALID_ARGUMENT_01",
    "ISTATUS_INVALID_ARGUMENT_02",
    "ISTATUS_INVALID_ARGUMENT_03",
    "ISTATUS_INVALID_ARGUMENT_04",
    "ISTATUS_INVALID_ARGUMENT_05",
    "ISTATUS_INVALID_ARGUMENT_06",
    "ISTATUS_INVALID_ARGUMENT_07",
    "ISTATUS_INVALID_ARGUMENT_08",
    "ISTATUS_INVALID_ARGUMENT_09",
    "ISTATUS_INVALID_ARGUMENT_10",
    "ISTATUS_INVALID_ARGUMENT_11",
    "ISTATUS_INVALID_ARGUMENT_12",
    "ISTATUS_INVALID_ARGUMENT_13",
    "ISTATUS_INVALID_ARGUMENT_14",
    "ISTATUS_INVALID_ARGUMENT_15",
    "ISTATUS_INVALID_ARGUMENT_16",
    "ISTATUS_INVALID_ARGUMENT_17",
    "ISTATUS_INVALID_ARGUMENT_18",
    "ISTATUS_INVALID_ARGUMENT_19",
    "ISTATUS_INVALID_ARGUMENT_20",
    "ISTATUS_INVALID_ARGUMENT_21",
    "ISTATUS_INVALID_ARGUMENT_22",
    "ISTATUS_INVALID_ARGUMENT_23",
    "ISTATUS_INVALID_ARGUMENT_24",
    "ISTATUS_INVALID_ARGUMENT_25",
    "ISTATUS_INVALID_ARGUMENT_26",
    "ISTATUS_INVALID_ARGUMENT_27",
    "ISTATUS_INVALID_ARGUMENT_28",
    "ISTATUS_INVALID_ARGUMENT_29",
    "ISTATUS_INVALID_ARGUMENT_30",
    "ISTATUS_INVALID_ARGUMENT_31",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_00",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_01",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_02",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_03",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_04",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_05",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_06",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_07",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_08",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_09",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_10",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_11",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_12",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_13",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_14",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_15",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_16",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_17",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_18",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_19",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_20",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_21",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_22",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_23",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_24",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_25",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_26",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_27",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_28",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_29",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_30",
    "ISTATUS_INVALID_ARGUMENT_COMBINATION_31"
};


//
// Functions
//

const char *
ISTATUSToCString(
    _In_ ISTATUS Status
    )
{
    if (Status > StatusNames.size())
    {
        throw std::invalid_argument("Status");
    }
    
    const char * StatusName = StatusNames[Status];
    
    if (StatusName == nullptr)
    {
        throw std::invalid_argument("Status");
    }
    
    return StatusName;
}

} // namespace Iris