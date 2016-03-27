/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    hittester.c

Abstract:

    This module implements the HIT_TESTER functions.

--*/

#include <irisp.h>

//
// Internal Functions
//

COMPARISON_RESULT
HitTesterInternalHitPointerCompare(
    _In_ PCVOID Hit0,
    _In_ PCVOID Hit1
    )
{
    PCINTERNAL_HIT *InternalHit0;
    PCINTERNAL_HIT *InternalHit1;
    COMPARISON_RESULT Result;

    ASSERT(Hit0 != NULL);
    ASSERT(Hit1 != NULL);

    InternalHit0 = (PCINTERNAL_HIT*) Hit0;
    InternalHit1 = (PCINTERNAL_HIT*) Hit1;

    Result = InternalHitCompare(*InternalHit0, *InternalHit1);

    return Result;
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
HitTesterTestGeometry(
    _Inout_ PHIT_TESTER HitTester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID Data
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PSHARED_HIT_DATA SharedHitData;
    PHIT_ALLOCATOR HitAllocator;
    PINTERNAL_HIT InternalHit;
    PHIT_LIST HitList;
    ISTATUS Status;

    if (HitTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (TestGeometryRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    SharedHitDataAllocator = &HitTester->SharedHitDataAllocator;
    HitAllocator = &HitTester->HitAllocator;
    PointerList = &HitTester->HitList;

    SharedHitData = SharedHitDataAllocatorAllocate(SharedHitDataAllocator);

    if (SharedHitData == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    SharedHitData->ModelToWorld = NULL;
    SharedHitData->Premultiplied = TRUE;
    SharedHitData->ModelRay = HitTester->CurrentRay;

    Status = TestGeometryRoutine(Data,
                                 HitTester->CurrentRay,
                                 HitAllocator,
                                 &HitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (HitList == NULL)
    {
        SharedHitDataAllocatorFreeLastAllocation(SharedHitDataAllocator);
        SharedHitData = NULL;
        return ISTATUS_SUCCESS;
    }

    while (HitList != NULL)
    {
        InternalHit = (PINTERNAL_HIT) HitList->Hit;

        InternalHit->SharedHitData = SharedHitData;

        Status = ConstantPointerListAddPointer(PointerList,
                                               InternalHit);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        HitList = HitList->NextHit;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
HitTesterTestGeometryWithTransform(
    _Inout_ PHIT_TESTER HitTester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID Data,
    _In_opt_ PCMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PSHARED_HIT_DATA SharedHitData;
    PHIT_ALLOCATOR HitAllocator;
    PINTERNAL_HIT InternalHit;
    PHIT_LIST HitList;
    ISTATUS Status;
    RAY TraceRay;

    if (HitTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (TestGeometryRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    SharedHitDataAllocator = &HitTester->SharedHitDataAllocator;
    HitAllocator = &HitTester->HitAllocator;
    PointerList = &HitTester->HitList;

    SharedHitData = SharedHitDataAllocatorAllocate(SharedHitDataAllocator);

    if (SharedHitData == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    SharedHitData->ModelToWorld = ModelToWorld;

    if (Premultiplied != FALSE)
    {   
        SharedHitData->Premultiplied = TRUE;
        TraceRay = HitTester->CurrentRay;
    }
    else
    {
        SharedHitData->Premultiplied = FALSE;

        SharedHitData->ModelRay = RayMatrixInverseMultiply(ModelToWorld,
                                                           HitTester->CurrentRay);

        TraceRay = SharedHitData->ModelRay;
    }

    Status = TestGeometryRoutine(Data,
                                 TraceRay,
                                 HitAllocator,
                                 &HitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (HitList == NULL)
    {
        SharedHitDataAllocatorFreeLastAllocation(SharedHitDataAllocator);
        SharedHitData = NULL;
        return ISTATUS_SUCCESS;
    }

    while (HitList != NULL)
    {
        InternalHit = (PINTERNAL_HIT) HitList->Hit;

        InternalHit->SharedHitData = SharedHitData;

        Status = ConstantPointerListAddPointer(PointerList,
                                               InternalHit);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        HitList = HitList->NextHit;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
HitTesterTestPremultipliedGeometryWithTransform(
    _Inout_ PHIT_TESTER HitTester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID Data,
    _In_opt_ PCMATRIX ModelToWorld
    )
{
    PSHARED_HIT_DATA_ALLOCATOR SharedHitDataAllocator;
    PCONSTANT_POINTER_LIST PointerList;
    PSHARED_HIT_DATA SharedHitData;
    PHIT_ALLOCATOR HitAllocator;
    PINTERNAL_HIT InternalHit;
    PHIT_LIST HitList;
    ISTATUS Status;

    if (HitTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (TestGeometryRoutine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    SharedHitDataAllocator = &HitTester->SharedHitDataAllocator;
    HitAllocator = &HitTester->HitAllocator;
    PointerList = &HitTester->HitList;

    SharedHitData = SharedHitDataAllocatorAllocate(SharedHitDataAllocator);

    if (SharedHitData == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    SharedHitData->ModelToWorld = ModelToWorld;
    SharedHitData->Premultiplied = TRUE;

    Status = TestGeometryRoutine(Data,
                                 HitTester->CurrentRay,
                                 HitAllocator,
                                 &HitList);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (HitList == NULL)
    {
        SharedHitDataAllocatorFreeLastAllocation(SharedHitDataAllocator);
        SharedHitData = NULL;
        return ISTATUS_SUCCESS;
    }

    while (HitList != NULL)
    {
        InternalHit = (PINTERNAL_HIT) HitList->Hit;

        InternalHit->SharedHitData = SharedHitData;

        Status = ConstantPointerListAddPointer(PointerList,
                                               InternalHit);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        HitList = HitList->NextHit;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
HitTesterGetRay(
    _In_ PHIT_TESTER HitTester,
    _Out_ PRAY Ray
    )
{
    if (HitTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Ray == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    *Ray = HitTester->CurrentRay;

    return ISTATUS_SUCCESS;
}