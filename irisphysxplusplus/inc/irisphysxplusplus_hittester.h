/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_hittester.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ HitTester type.

--*/

#include <irisphysxplusplus.h>

#ifndef _HIT_TESTER_IRIS_PHYSX_PLUS_PLUS_
#define _HIT_TESTER_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class HitTester final {
public:
    HitTester(
        _In_ PPHYSX_HIT_TESTER HitTesterPtr
        )
    : Data(HitTesterPtr)
    { 
        if (HitTesterPtr == nullptr)
        {
            throw std::invalid_argument("HitTesterPtr");
        }
    }
    
    _Ret_
    PPHYSX_HIT_TESTER
    AsPPHYSX_HIT_TESTER(
        void
        )
    {
        return Data;
    }

    void
    Test(
        _In_ const Geometry & GeometryRef
        )
    {
        ISTATUS Status = PhysxHitTesterTestGeometry(Data,
                                                    GeometryRef.AsPCPHYSX_GEOMETRY());

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
    }

    void
    Test(
        _In_ const Geometry & GeometryRef,
        _In_ const Iris::Matrix & MatrixRef
        )
    {
        ISTATUS Status = PhysxHitTesterTestPremultipliedGeometryWithTransform(Data,
                                                                              GeometryRef.AsPCPHYSX_GEOMETRY(),
                                                                              MatrixRef.AsPCMATRIX());

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
    }

    void
    Test(
        _In_ const Geometry & GeometryRef,
        _In_ const Iris::Matrix & MatrixRef,
        _In_ bool Premultiplied
        )
    {
        BOOL IrisPremultiplied = (Premultiplied) ? TRUE : FALSE;
        
        ISTATUS Status = PhysxHitTesterTestGeometryWithTransform(Data,
                                                                 GeometryRef.AsPCPHYSX_GEOMETRY(),
                                                                 MatrixRef.AsPCMATRIX(),
                                                                 IrisPremultiplied);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }
    }

private:
    PPHYSX_HIT_TESTER Data;
};

} // namespace IrisPhysx

#endif // _HIT_TESTER_IRIS_PHYSX_PLUS_PLUS_
