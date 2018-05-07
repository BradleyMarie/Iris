/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    hit_tester.c

Abstract:

    Intersection tester.

--*/

#include "iris/hit_tester.h"

#include "iris/full_hit_context.h"
#include "iris/hit_tester_internal.h"
#include "iris/multiply_internal.h"

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
HitTesterCollectHitsAndUpdateSharedHit(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PCSHARED_HIT_CONTEXT shared_context,
    _In_ PHIT hit
    )
{
    assert(hit_tester != NULL);
    assert(shared_context != NULL);
    assert(hit != NULL);

    do
    {
        bool success = 
            ConstantPointerListAddPointer(&hit_tester->hit_list, hit);

        if (!success)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        PFULL_HIT_CONTEXT full_hit_context = (PFULL_HIT_CONTEXT)(void *)hit;
        full_hit_context->shared_context = shared_context;

        hit = hit->next;
    } while (hit != NULL);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
HitTesterTestWorldInternal(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data
    )
{
    assert(hit_tester != NULL);
    assert(test_routine != NULL);

    HitAllocatorSetData(&hit_tester->hit_allocator, hit_data);

    PHIT hit;
    ISTATUS status = test_routine(geometry_data,
                                  &hit_tester->world_ray,
                                  &hit_tester->hit_allocator,
                                  &hit);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    while (hit != NULL)
    {
        bool success = 
            ConstantPointerListAddPointer(&hit_tester->hit_list, hit);

        if (!success)
        {
            return ISTATUS_ALLOCATION_FAILED;
        }

        hit = hit->next;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
HitTesterTestPremultipliedInternal(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _In_ PCMATRIX model_to_world
    )
{
    assert(hit_tester != NULL);
    assert(test_routine != NULL);
    assert(model_to_world != NULL);

    HitAllocatorSetData(&hit_tester->hit_allocator, hit_data);

    PHIT hit;
    ISTATUS status = test_routine(geometry_data,
                                  &hit_tester->world_ray,
                                  &hit_tester->hit_allocator,
                                  &hit);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (hit == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    PSHARED_HIT_CONTEXT context;
    PSHARED_HIT_CONTEXT_ALLOCATOR allocator = &hit_tester->context_allocator;
    bool ok = SharedHitContextAllocatorAllocatePremultiplied(allocator,
                                                             model_to_world,
                                                             &context);
    
    if (!ok)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    status = HitTesterCollectHitsAndUpdateSharedHit(hit_tester,
                                                    context,
                                                    hit);

    return status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS
HitTesterTestTransformedInternal(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _In_ PCMATRIX model_to_world
    )
{
    assert(hit_tester != NULL);
    assert(test_routine != NULL);
    assert(model_to_world != NULL);

    RAY trace_ray = RayMatrixInverseMultiplyInline(model_to_world,
                                                   hit_tester->world_ray);

    HitAllocatorSetData(&hit_tester->hit_allocator, hit_data);

    PHIT hit;
    ISTATUS status = test_routine(geometry_data,
                                  &trace_ray,
                                  &hit_tester->hit_allocator,
                                  &hit);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (hit == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    PSHARED_HIT_CONTEXT context;
    PSHARED_HIT_CONTEXT_ALLOCATOR allocator = &hit_tester->context_allocator;
    bool ok = SharedHitContextAllocatorAllocateTransformed(allocator,
                                                           model_to_world,
                                                           trace_ray,
                                                           &context);
    
    if (!ok)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    status = HitTesterCollectHitsAndUpdateSharedHit(hit_tester,
                                                    context,
                                                    hit);

    return status;
}

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
HitTesterTestWorldGeometry(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data
    )
{
    if (hit_tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (test_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    ISTATUS status = HitTesterTestWorldInternal(hit_tester, 
                                                test_routine,
                                                geometry_data,
                                                hit_data);
    
    return status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
HitTesterTestPremultipliedGeometry(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _In_opt_ PCMATRIX model_to_world
    )
{
    if (hit_tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (test_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (model_to_world == NULL)
    {
        ISTATUS status = HitTesterTestWorldInternal(hit_tester, 
                                                    test_routine,
                                                    geometry_data,
                                                    hit_data);
        
        return status;
    }

    ISTATUS status = HitTesterTestPremultipliedInternal(hit_tester, 
                                                        test_routine,
                                                        geometry_data,
                                                        hit_data,
                                                        model_to_world);
    
    return status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
HitTesterTestGeometry(
    _Inout_ PHIT_TESTER hit_tester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine,
    _In_opt_ const void *geometry_data,
    _In_opt_ const void *hit_data,
    _In_opt_ PCMATRIX model_to_world,
    _In_ bool premultiplied
    )
{
    if (hit_tester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (test_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (model_to_world == NULL)
    {
        ISTATUS status = HitTesterTestWorldInternal(hit_tester, 
                                                    test_routine,
                                                    geometry_data,
                                                    hit_data);
        
        return status;
    }

    if (premultiplied)
    {
        ISTATUS status = HitTesterTestPremultipliedInternal(hit_tester, 
                                                            test_routine,
                                                            geometry_data,
                                                            hit_data,
                                                            model_to_world);
        
        return status;
    }
    
    ISTATUS status = HitTesterTestTransformedInternal(hit_tester, 
                                                      test_routine,
                                                      geometry_data,
                                                      hit_data,
                                                      model_to_world);
    
    return status;
}