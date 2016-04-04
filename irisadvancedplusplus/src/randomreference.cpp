/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    randomrandom.cpp

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ RandomReference type.

--*/

#include <irisadvancedplusplusp.h>

namespace IrisAdvanced {

RandomReference::RandomReference(
    _In_ PRANDOM_REFERENCE RandomReferencePtr
    )
: Data(RandomReferencePtr)
{ 
    if (RandomReferencePtr == NULL)
    {
        throw std::invalid_argument("RandomReferencePtr");
    }
}

_Ret_range_(Minimum, Maximum)
FLOAT
RandomReference::GenerateFloat(
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum
    )
{
    FLOAT Result;
    
    ISTATUS Status = RandomReferenceGenerateFloat(Data,
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
RandomReference::GenerateIndex(
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum
    )
{
    SIZE_T Result;
    
    ISTATUS Status = RandomReferenceGenerateIndex(Data,
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