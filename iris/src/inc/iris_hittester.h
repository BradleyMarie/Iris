/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iris_hittester.h

Abstract:

    This module implements the HIT_TESTER internal functions.

--*/

#ifndef _IRIS_HIT_TESTER_INTERNAL_
#define _IRIS_HIT_TESTER_INTERNAL_

#include <irisp.h>

//
// Types
//

struct _HIT_TESTER {
    HIT_ALLOCATOR HitAllocator;
    SHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    CONSTANT_POINTER_LIST HitList;
    SIZE_T HitIndex;
    RAY CurrentRay;
};

//
// Internal Function Declarations
//

COMPARISON_RESULT
HitTesterInternalHitPointerCompare(
    _In_ PCVOID Hit0,
    _In_ PCVOID Hit1
    );

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
HitTesterInitialize(
    _Out_ PHIT_TESTER HitTester
    )
{
    ISTATUS Status;

    ASSERT(HitTester != NULL);

    Status = HitAllocatorInitialize(&HitTester->HitAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = ConstantPointerListInitialize(&HitTester->HitList);

    if (Status != ISTATUS_SUCCESS)
    {
        HitAllocatorDestroy(&HitTester->HitAllocator);
        return Status;
    }

    Status = SharedHitDataAllocatorInitialize(&HitTester->SharedHitDataAllocator);

    if (Status != ISTATUS_SUCCESS)
    {
        ConstantPointerListDestroy(&HitTester->HitList);
        HitAllocatorDestroy(&HitTester->HitAllocator);
        return Status;
    }

    HitTester->HitIndex = 0;

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
HitTesterSetRay(
    _Inout_ PHIT_TESTER HitTester,
    _In_ RAY Ray
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PHIT_ALLOCATOR HitAllocator;

    ASSERT(HitTester != NULL);
    ASSERT(RayValidate(Ray) != FALSE);

    SharedHitDataAllocator = &HitTester->SharedHitDataAllocator;
    HitAllocator = &HitTester->HitAllocator;
    PointerList = &HitTester->HitList;

    SharedHitDataAllocatorFreeAll(SharedHitDataAllocator);
    HitAllocatorFreeAll(HitAllocator);
    ConstantPointerListClear(PointerList);

    HitTester->HitIndex = 0;
    HitTester->CurrentRay = Ray;
}

SFORCEINLINE
VOID
HitTesterSort(
    _Inout_ PHIT_TESTER HitTester
    )
{
    ASSERT(HitTester != NULL);

    ConstantPointerListSort(&HitTester->HitList,
                            HitTesterInternalHitPointerCompare);
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
HitTesterGetNextHit(
    _Inout_ PHIT_TESTER HitTester,
    _Out_ PCINTERNAL_HIT *Hit
    )
{
    PCCONSTANT_POINTER_LIST PointerList;
    PCVOID ValueAtIndex;
    SIZE_T CurrentIndex;
    SIZE_T HitCount;

    ASSERT(HitTester != NULL);
    ASSERT(Hit != NULL);

    PointerList = &HitTester->HitList;

    HitCount = ConstantPointerListGetSize(PointerList);
    CurrentIndex = HitTester->HitIndex;

    if (HitCount == CurrentIndex)
    {
        return ISTATUS_NO_MORE_DATA;
    }

    ValueAtIndex = ConstantPointerListRetrieveAtIndex(PointerList,
                                                      CurrentIndex);

    HitTester->HitIndex = CurrentIndex + 1;

    *Hit = (PCINTERNAL_HIT) ValueAtIndex;

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
HitTesterComputeHitData(
    _In_ PCHIT_TESTER HitTester,
    _In_ PCINTERNAL_HIT Hit,
    _Out_ PCMATRIX *ModelToWorld,
    _Out_ PVECTOR3 ModelViewer,
    _Out_ PPOINT3 ModelHit,
    _Out_ PPOINT3 WorldHit
    )
{
    PCSHARED_HIT_DATA SharedHitData;

    ASSERT(HitTester != NULL);
    ASSERT(Hit != NULL);
    ASSERT(ModelToWorld != NULL);
    ASSERT(ModelViewer != NULL);
    ASSERT(ModelHit != NULL);
    ASSERT(WorldHit != NULL);

    SharedHitData = Hit->SharedHitData;

    *ModelToWorld = SharedHitData->ModelToWorld;
    
    if (SharedHitData->Premultiplied != FALSE)
    {
        if (Hit->ModelHitPointValid != FALSE)
        {
            *WorldHit = Hit->ModelHitPoint;
        }
        else
        {
            *WorldHit = RayEndpoint(HitTester->CurrentRay,
                                    Hit->Hit.Distance);
        }
        
        *ModelHit = PointMatrixMultiply(SharedHitData->ModelToWorld,
                                        *WorldHit);

        *ModelViewer = VectorMatrixInverseMultiply(SharedHitData->ModelToWorld,
                                                   HitTester->CurrentRay.Direction);
    }
    else
    {
        if (Hit->ModelHitPointValid != FALSE)
        {
            *ModelHit = Hit->ModelHitPoint;
        }
        else
        {
            *ModelHit = RayEndpoint(SharedHitData->ModelRay,
                                    Hit->Hit.Distance);
        }

        *ModelViewer = SharedHitData->ModelRay.Direction;

        *WorldHit = RayEndpoint(HitTester->CurrentRay,
                                Hit->Hit.Distance);
    }
}

SFORCEINLINE
VOID
HitTesterDestroy(
    _In_ _Post_invalid_ PHIT_TESTER HitTester
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PHIT_ALLOCATOR HitAllocator;

    ASSERT(HitTester != NULL);

    SharedHitDataAllocator = &HitTester->SharedHitDataAllocator;
    HitAllocator = &HitTester->HitAllocator;
    PointerList = &HitTester->HitList;

    SharedHitDataAllocatorDestroy(SharedHitDataAllocator);
    HitAllocatorDestroy(HitAllocator);
    ConstantPointerListDestroy(PointerList);
}

#endif // _IRIS_HIT_TESTER_INTERNAL_