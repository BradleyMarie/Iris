/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisplusplus_randomallocator.h

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ RandomAllocator type.

--*/

#include <irisadvancedplusplus.h>

#ifndef _RANDOM_ALLOCATOR_IRIS_ADVANCED_PLUS_PLUS_
#define _RANDOM_ALLOCATOR_IRIS_ADVANCED_PLUS_PLUS_

namespace IrisAdvanced {

//
// Types
//

class RandomAllocator final {
public:
    RandomAllocator(
        _In_ PRANDOM_ALLOCATOR RandomAllocatorPtr
        )
    : Data(RandomAllocatorPtr)
    { 
        if (RandomAllocatorPtr == nullptr)
        {
            throw std::invalid_argument("RandomAllocatorPtr");
        }
    }
        
    _Ret_
    PRANDOM_ALLOCATOR
    AsPRANDOM_ALLOCATOR(
        void
        ) noexcept
    {
        return Data;
    }

private:
    PRANDOM_ALLOCATOR Data;
};

} // namespace IrisAdvanced

#endif // _RANDOM_ALLOCATOR_IRIS_ADVANCED_PLUS_PLUS_