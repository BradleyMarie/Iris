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
    
private:
    _Success_(return == ISTATUS_SUCCESS)
    static
    ISTATUS
    GenerateFloat(
        _In_ PVOID Context,
        _In_ FLOAT Minimum,
        _In_ FLOAT Maximum,
        _Out_range_(Minimum, Maximum) PFLOAT RandomValue
        );

    _Success_(return == ISTATUS_SUCCESS)
    static
    ISTATUS
    GenerateIndex(
        _In_ PVOID Context,
        _In_ SIZE_T Minimum,
        _In_ SIZE_T Maximum,
        _Out_range_(Minimum, Maximum) PSIZE_T RandomValue
        );
    
    static
    VOID 
    Free(
        _In_ _Post_invalid_ PVOID Context
        );
    
    const static RANDOM_VTABLE InteropVTable;
};

} // namespace IrisAdvanced

#endif // _RANDOM_BASE_IRIS_ADVANCED_PLUS_PLUS_