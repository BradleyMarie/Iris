/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_visibilitytester.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ VisibilityTester type.

--*/

#include <irisphysxplusplus.h>

#ifndef _VISIBILITY_TESTER_IRIS_PHYSX_PLUS_PLUS_
#define _VISIBILITY_TESTER_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Forward Declarations
//

class LightReference;
class Light;

//
// Types
//

class VisibilityTester final {
public:
    VisibilityTester(
        _In_ PPBR_VISIBILITY_TESTER VisibilityTesterPtr
        )
    : Data(VisibilityTesterPtr)
    { 
        if (VisibilityTesterPtr == nullptr)
        {
            throw std::invalid_argument("VisibilityTesterPtr");
        }
    }
    
    _Ret_
    PPBR_VISIBILITY_TESTER
    AsPPBR_VISIBILITY_TESTER(
        void
        )
    {
        return Data;
    }

    _Ret_
    bool
    Test(
        _In_ const Iris::Ray & WorldRay
        )
    {
        BOOL Result;

        ISTATUS Status = PBRVisibilityTesterTestVisibilityAnyDistance(Data,
                                                                      WorldRay.AsRAY(),
                                                                      &Result);
    
        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        return Result != FALSE;
    }

    _Ret_
    bool
    Test(
        _In_ const Iris::Ray & WorldRay,
        _In_ FLOAT Distance
        )
    {
        BOOL Result;

        ISTATUS Status = PBRVisibilityTesterTestVisibility(Data,
                                                           WorldRay.AsRAY(),
                                                           Distance,
                                                           &Result);
    
        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        return Result != FALSE;
    }

    _Ret_
    bool
    Test(
        _In_ const Iris::Ray & WorldRay,
        _In_ const LightReference & LightRef
        );

    _Ret_
    bool
    Test(
        _In_ const Iris::Ray & WorldRay,
        _In_ const Light & LightRef
        );

private:
    PPBR_VISIBILITY_TESTER Data;
};

} // namespace Iris

#endif // _VISIBILITY_TESTER_IRIS_PHYSX_PLUS_PLUS_