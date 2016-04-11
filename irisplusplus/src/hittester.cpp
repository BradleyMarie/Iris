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
    assert(Context != nullptr);
    assert(RayValidate(WorldRay) != FALSE);
    assert(Allocator != nullptr);
    assert(HitList != nullptr);
    
    auto TestGeometryRoutine = static_cast<const std::function<PHIT_LIST(Ray, HitAllocator)> *>(Context);

    PHIT_LIST Result = (*TestGeometryRoutine)(Ray(WorldRay), HitAllocator(Allocator));

    *HitList = Result;
    return ISTATUS_SUCCESS;
}

} // namespace Iris