/*++

Copyright (c) 2015 Brad Weinberger

Module Name:

    HitTester.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ HitTester type.

--*/

#include <irisplusplusp.h>

namespace Iris {

//
// Static Functions
//

ISTATUS
HitTester::TestGeometryAdapter(
    _In_opt_ PCVOID Context,
    _In_ RAY WorldRay,
    _Inout_ PHIT_ALLOCATOR Allocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    )
{
    auto TestGeometryRoutine = static_cast<const std::function<PHIT_LIST(Ray, HitAllocator)> *>(Context);

    PHIT_LIST Result = (*TestGeometryRoutine)(Ray(WorldRay), HitAllocator(Allocator));

    *HitList = Result;
    return ISTATUS_SUCCESS;
}

//
// Functions
//

void
HitTester::Test(
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
HitTester::Test(
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
HitTester::Test(
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

} // namespace Iris