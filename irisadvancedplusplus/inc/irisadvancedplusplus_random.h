/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisplusplus_random.h

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ random type.

--*/

#include <irisadvancedplusplus.h>

#ifndef _RANDOM_IRIS_ADVANCED_PLUS_PLUS_
#define _RANDOM_IRIS_ADVANCED_PLUS_PLUS_

namespace IrisAdvanced {

//
// Types
//

class Random final {
public:
    Random(
        _In_ PRANDOM RandomPtr
        )
    : Data(RandomPtr)
    { 
        if (RandomPtr == NULL)
        {
            throw std::invalid_argument("RandomPtr");
        }
    }

    Random(
        _In_ Random && ToMove
        )
    : Data(ToMove.Data)
    {
        ToMove.Data = nullptr;
    }
    
    _Ret_range_(Minimum, Maximum)
    FLOAT
    GenerateFloat(
        _In_ FLOAT Minimum,
        _In_ FLOAT Maximum
        )
    {
        FLOAT Result;
        
        ISTATUS Status = RandomGenerateFloat(Data,
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
        
        ISTATUS Status = RandomGenerateIndex(Data,
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
    PRANDOM
    AsPRANDOM(
        void
        )
    {
        return Data;
    }
        
    _Ret_
    PRANDOM_REFERENCE
    AsPRANDOM_REFERENCE(
        void
        )
    {
        return RandomGetRandomReference(Data);
    }

    RandomReference
    AsRandomReference(
        void
        )
    {
        return RandomReference(AsPRANDOM_REFERENCE());
    }

    Random(
        _In_ Random & ToCopy
        ) = delete;

    Random & 
    operator=(
        _In_ const Random & ToCopy
        ) = delete;

    ~Random(
        void
        )
    { 
        RandomFree(Data);
    }

private:
    PRANDOM Data;
};

} // namespace IrisAdvanced

#endif // _RANDOM_IRIS_ADVANCED_PLUS_PLUS_