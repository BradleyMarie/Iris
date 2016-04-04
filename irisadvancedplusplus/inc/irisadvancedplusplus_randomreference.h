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
    IRISADVANCEDPLUSPLUSAPI
    RandomReference(
        _In_ PRANDOM_REFERENCE RandomReferencePtr
        );
    
    _Ret_range_(Minimum, Maximum)
    IRISADVANCEDPLUSPLUSAPI
    FLOAT
    GenerateFloat(
        _In_ FLOAT Minimum,
        _In_ FLOAT Maximum
        );

    _Ret_range_(Minimum, Maximum)
    IRISADVANCEDPLUSPLUSAPI
    SIZE_T
    GenerateIndex(
        _In_ SIZE_T Minimum,
        _In_ SIZE_T Maximum
        );
        
    _Ret_
    PRANDOM_REFERENCE
    AsPRANDOM_REFERENCE(
        void
        )
    {
        return Data;
    }

    _Ret_
    PCRANDOM_REFERENCE
    AsPCRANDOM_REFERENCE(
        void
        ) const
    {
        return Data;
    }

    RandomReference(
        _In_ const RandomReference & ToCopy
        )
    : Data(ToCopy.Data)
    { }

    RandomReference & 
    operator=(
        _In_ const RandomReference & ToCopy
        )
    {
        Data = ToCopy.Data;
        return *this;
    }

private:
    PRANDOM_REFERENCE Data;
};

} // namespace IrisAdvanced

#endif // _RANDOM_REFERENCE_IRIS_ADVANCED_PLUS_PLUS_