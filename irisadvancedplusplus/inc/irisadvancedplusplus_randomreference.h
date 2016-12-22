/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisplusplus_randomreference.h

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ RandomReferenceReference type.

--*/

#include <irisadvancedplusplus.h>

#ifndef _RANDOM_REFERENCE_IRIS_ADVANCED_PLUS_PLUS_
#define _RANDOM_REFERENCE_IRIS_ADVANCED_PLUS_PLUS_

namespace IrisAdvanced {

//
// Types
//

class RandomReference final {
public:
    RandomReference(
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
    GenerateFloat(
        _In_ FLOAT Minimum,
        _In_ FLOAT Maximum
        )
    {
        FLOAT Result;
        
        ISTATUS Status = RandomReferenceGenerateFloat(Data,
                                                      Minimum,
                                                      Maximum,
                                                      &Result);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        return Result;
    }

    _Ret_range_(Minimum, Maximum)
    SIZE_T
    GenerateIndex(
        _In_ SIZE_T Minimum,
        _In_ SIZE_T Maximum
        )
    {
        SIZE_T Result;
        
        ISTATUS Status = RandomReferenceGenerateIndex(Data,
                                                      Minimum,
                                                      Maximum,
                                                      &Result);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        return Result;  
    }
        
    _Ret_
    PRANDOM_REFERENCE
    AsPRANDOM_REFERENCE(
        void
        ) noexcept
    {
        return Data;
    }

private:
    PRANDOM_REFERENCE Data;
};

} // namespace IrisAdvanced

#endif // _RANDOM_REFERENCE_IRIS_ADVANCED_PLUS_PLUS_