/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisadvancedplusplus_scenebase.h

Abstract:

    This file contains the definitions for the 
    Iris++ RandomBase type.

--*/

#include <irisadvancedplusplus.h>

#ifndef _RANDOM_BASE_IRIS_ADVANCED_PLUS_PLUS_
#define _RANDOM_BASE_IRIS_ADVANCED_PLUS_PLUS_

namespace IrisAdvanced {

//
// Types
//

class RandomBase {
protected:
    IRISADVANCEDPLUSPLUSAPI
    static
    Random
    Create(
        _In_ std::unique_ptr<RandomBase> RandomBasePtr
        );

public:
    _Ret_range_(Minimum, Maximum)
    virtual
    FLOAT
    GenerateFloat(
        _In_ FLOAT Minimum,
        _In_ FLOAT Maximum
        ) = 0;

    _Ret_range_(Minimum, Maximum)
    virtual
    SIZE_T
    GenerateIndex(
        _In_ SIZE_T Minimum,
        _In_ SIZE_T Maximum
        ) = 0;

    virtual
    ~RandomBase(
        void
        )
    { }
};

} // namespace IrisAdvanced

#endif // _RANDOM_BASE_IRIS_ADVANCED_PLUS_PLUS_