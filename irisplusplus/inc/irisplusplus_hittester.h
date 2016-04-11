/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisplusplus_hittester.h

Abstract:

    This file contains the definitions for the 
    Iris++ raytracer reference type.

--*/

#include <irisplusplus.h>

#ifndef _HIT_TESTER_IRIS_PLUS_PLUS_
#define _HIT_TESTER_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class HitTester final {
public:
    HitTester(
        _In_ PHIT_TESTER HitTesterPtr
        )
    : Data(HitTesterPtr)
    { 
        if (HitTesterPtr == nullptr)
        {
            throw std::invalid_argument("HitTesterPtr");
        }
    }
    
    _Ret_
    PHIT_TESTER
    AsPHIT_TESTER(
        void
        )
    {
        return Data;
    }

    void
    Test(
        _In_ std::function<PHIT_LIST(Ray, HitAllocator)> TestGeometryRoutine
        )
    {
        ISTATUS Status = HitTesterTestGeometry(Data, 
                                               TestGeometryAdapter,
                                               &TestGeometryRoutine);
        
        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(ISTATUSToCString(Status));
        }
    }

    void
    Test(
        _In_ std::function<PHIT_LIST(Ray, HitAllocator)> TestGeometryRoutine,
        _In_ const Matrix & MatrixRef
        )
    {
        ISTATUS Status = HitTesterTestPremultipliedGeometryWithTransform(Data, 
                                                                         TestGeometryAdapter,
                                                                         &TestGeometryRoutine,
                                                                         MatrixRef.AsPCMATRIX());
        
        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(ISTATUSToCString(Status));
        }
    }

    void
    Test(
        _In_ std::function<PHIT_LIST(Ray, HitAllocator)> TestGeometryRoutine,
        _In_ const Matrix & MatrixRef,
        _In_ bool Premultiplied
        )
    {
        ISTATUS Status = HitTesterTestGeometryWithTransform(Data, 
                                                            TestGeometryAdapter,
                                                            &TestGeometryRoutine,
                                                            MatrixRef.AsPCMATRIX(),
                                                            Premultiplied ? TRUE : FALSE);
                                                          
        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(ISTATUSToCString(Status));
        }
    }

private:
    PHIT_TESTER Data;

    IRISPLUSPLUSAPI
    static
    ISTATUS
    TestGeometryAdapter(
        _In_opt_ PCVOID Context,
        _In_ RAY Ray,
        _Inout_ PHIT_ALLOCATOR HitAllocator,
        _Outptr_result_maybenull_ PHIT_LIST *HitList
        );
};

} // namespace Iris

#endif // _HIT_TESTER_IRIS_PLUS_PLUS_