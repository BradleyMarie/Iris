/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    randombase.cpp

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ RandomBase type.

--*/

#include <irisadvancedplusplusp.h>

namespace IrisAdvanced {

//
// Static Functions
//

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RandomBase::GenerateFloat(
    _In_ PVOID Context,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum,
    _Out_range_(Minimum, Maximum) PFLOAT RandomValue
    )
{
    assert(Context != nullptr);
    assert(IsFiniteFloat(Minimum));
    assert(IsFiniteFloat(Maximum));
    assert(Minimum < Maximum);
    assert(RandomValue != nullptr);
    
    RandomBase **RandomBasePtr = (RandomBase**) Context;
    *RandomValue = (*RandomBasePtr)->GenerateFloat(Minimum, Maximum);
    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
RandomBase::GenerateIndex(
    _In_ PVOID Context,
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum,
    _Out_range_(Minimum, Maximum) PSIZE_T RandomValue
    )
{
    assert(Context != nullptr);
    assert(Minimum < Maximum);
    assert(RandomValue != nullptr);
    
    RandomBase **RandomBasePtr = (RandomBase**) Context;
    *RandomValue = (*RandomBasePtr)->GenerateIndex(Minimum, Maximum);
    return ISTATUS_SUCCESS;
}

VOID 
RandomBase::Free(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != nullptr);

    RandomBase **RandomBasePtr = (RandomBase**) Context;
    delete *RandomBasePtr;
}

Random
RandomBase::Create(
    _In_ std::unique_ptr<RandomBase> RandomBasePtr
    )
{
    if (!RandomBasePtr)
    {
        throw std::invalid_argument("RandomBasePtr");
    }
    
    RandomBase *UnmanagedRandomBasePtr = RandomBasePtr.release();
    PRANDOM RandomPtr;

    ISTATUS Success = RandomAllocate(&InteropVTable,
                                     &UnmanagedRandomBasePtr,
                                     sizeof(RandomBase*),
                                     alignof(RandomBase*),
                                     &RandomPtr);

    if (Success != ISTATUS_SUCCESS)
    {
        throw std::bad_alloc();
    }
    
    return Random(RandomPtr);
}

//
// Static Variables
//

const RANDOM_VTABLE RandomBase::InteropVTable = {
    RandomBase::GenerateFloat, 
    RandomBase::GenerateIndex, 
    RandomBase::Free
};

} // namespace IrisAdvanced