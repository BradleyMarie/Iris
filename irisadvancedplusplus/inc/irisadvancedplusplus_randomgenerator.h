/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisplusplus_randomgenerator.h

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ RandomGenerator type.

--*/

#include <irisadvancedplusplus.h>

#ifndef _RANDOM_GENERATOR_IRIS_ADVANCED_PLUS_PLUS_
#define _RANDOM_GENERATOR_IRIS_ADVANCED_PLUS_PLUS_

namespace IrisAdvanced {

//
// Types
//

class RandomGenerator final {
public:
    RandomGenerator(
        _In_ PRANDOM_GENERATOR RandomGeneratorPtr
        )
    : Data(RandomGeneratorPtr, [](PRANDOM_GENERATOR ToDelete){ RandomGeneratorFree(ToDelete); })
    { 
        if (RandomGeneratorPtr == nullptr)
        {
            throw std::invalid_argument("RandomAllocatorPtr");
        }
    }
    
    void
    Generate(
        _In_ std::function<void(Random &)> & Callback
        ) const;

    _Ret_
    PCRANDOM_GENERATOR
    AsPCRANDOM_GENERATOR(
        void
        ) const noexcept
    {
        return Data.get();
    }

private:
    std::shared_ptr<RANDOM_GENERATOR> Data;
};

} // namespace IrisAdvanced

#endif // _RANDOM_GENERATOR_IRIS_ADVANCED_PLUS_PLUS_