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
private:
    _Ret_    
    IRISPHYSXPLUSPLUSAPI
    bool
    Test(
        _In_ const Iris::Ray & WorldRay,
        _In_ PCPBR_LIGHT LightPtr
        );    

public:
    VisibilityTester(
        _In_ PPBR_VISIBILITY_TESTER VisibilityTesterPtr
        )
    : Data(VisibilityTesterPtr)
    { }
    
    _Ret_
    IRISPHYSXPLUSPLUSAPI
    bool
    Test(
        _In_ const Iris::Ray & WorldRay
        );

    _Ret_    
    IRISPHYSXPLUSPLUSAPI
    bool
    Test(
        _In_ const Iris::Ray & WorldRay,
        _In_ FLOAT Distance
        );

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