/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    random.cpp

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ matrix type.

--*/

#include <irisadvancedplusplusp.h>

namespace IrisAdvanced {

_Ret_range_(Minimum, Maximum)
FLOAT
Random::GenerateFloat(
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum
    )
{
    FLOAT Result;
    
    ISTATUS Status = RandomGenerateFloat(Data,
                                         Minimum,
                                         Maximum,
                                         &Result);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_01:
            throw std::invalid_argument("Minimum");
            break;
        case ISTATUS_INVALID_ARGUMENT_02:
            throw std::invalid_argument("Maximum");
            break;
        case ISTATUS_INVALID_ARGUMENT_COMBINATION_00:
            throw std::invalid_argument("Minimum > Maximum");
            break;
        default:
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
            break;
    }
    
    return Result;
}

_Ret_range_(Minimum, Maximum)
SIZE_T
Random::GenerateIndex(
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum
    )
{
    SIZE_T Result;
    
    ISTATUS Status = RandomGenerateIndex(Data,
                                         Minimum,
                                         Maximum,
                                         &Result);

    switch (Status)
    {
        case ISTATUS_SUCCESS:
            break;
        case ISTATUS_INVALID_ARGUMENT_COMBINATION_00:
            throw std::invalid_argument("Minimum > Maximum");
            break;
        default:
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
            break;
    }
    
    return Result;    
}

} // namespace IrisAdvanced