/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameraplusplus_randomgenerator.h

Abstract:

    This file contains the definitions for the 
    IrisCamera++ RandomGenerator type.

--*/

#include <iriscameraplusplus.h>

#ifndef _RANDOM_GENERATOR_IRIS_CAMERA_PLUS_PLUS_HEADER_
#define _RANDOM_GENERATOR_IRIS_CAMERA_PLUS_PLUS_HEADER_

namespace IrisCamera {

//
// Types
//

class RandomGenerator final {
public:
    RandomGenerator(
        _In_ PRANDOM_GENERATOR RandomGeneratorPtr
        )
    : Data(RandomGeneratorPtr)
    { 
        if (RandomGeneratorPtr == nullptr)
        {
            throw std::invalid_argument("RandomGeneratorPtr");
        }
    }

    RandomGenerator(
        _In_ RandomGenerator && ToMove
        )
    : Data(ToMove.Data)
    { 
        ToMove.Data = nullptr;
    }

    _Ret_
    PCRANDOM_GENERATOR
    AsPCRANDOM_GENERATOR(
        void
        ) const
    {
        return Data;
    }

    RandomGenerator(
        _In_ const RandomGenerator & ToCopy
        ) = delete;
        
    RandomGenerator &
    operator=(
        _In_ const RandomGenerator & ToCopy
        ) = delete;

    ~RandomGenerator(
        void
        )
    {
        RandomGeneratorFree(Data);
    }

private:
    PRANDOM_GENERATOR Data;
};

} // namespace IrisCamera

#endif // _RANDOM_GENERATOR_IRIS_CAMERA_PLUS_PLUS_HEADER_
