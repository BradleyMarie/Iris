/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    ray_tracer.c

Abstract:

    The top level ray tracer type used by Iris.

--*/

#include <stdlib.h>

#include "iris/ray_tracer.h"
#include "iris/hit_tester_internal.h"
#include "iris/multiply_internal.h"

//
// Types
//

struct _RAY_TRACER {
    HIT_TESTER hit_tester;
};

//
// Static Functions
//

static
ISTATUS
RayTracerValidateArugumentsAndTrace(
    _Inout_ PRAY_TRACER ray_tracer,
    _In_ RAY ray,
    _In_ float_t minimum_distance,
    _In_ PRAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ const void *process_hit_routine,
    _Inout_opt_ void *process_hit_context
    )
{
    if (ray_tracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!RayValidate(ray))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(minimum_distance) || minimum_distance < (float_t)0.0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (trace_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (process_hit_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    HitTesterReset(&ray_tracer->hit_tester, ray, minimum_distance);

    ISTATUS status = trace_routine(trace_context,
                                   &ray_tracer->hit_tester,
                                   ray);
    
    return status;
}

_Ret_
static
PCFULL_HIT_CONTEXT
RayTracerFindClosestHit(
    _In_reads_(num_hits) PCFULL_HIT_CONTEXT* hits,
    _In_ size_t num_hits
    )
{
    assert(hits != NULL);
    assert(num_hits != 0);

    PCFULL_HIT_CONTEXT closest = hits[0];
    for (size_t i = 0; i < num_hits; i++)
    {
        if (hits[i]->context.distance < closest->context.distance) {
            closest = hits[i];
        }
    }

    return closest;
}

static
ISTATUS
RayTracerProcessHitWithContext(
    _In_ RAY ray,
    _In_ PCFULL_HIT_CONTEXT hit,
    _In_ PRAY_TRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE process_hit_routine,
    _Inout_opt_ void *process_hit_context
    )
{
    assert(hit != NULL);
    assert(process_hit_routine != NULL);

    if (hit->shared_context == NULL)
    {
        POINT3 world_hit_point;
        if (hit->model_hit_point_valid)
        {
            world_hit_point = hit->model_hit_point;
        }
        else
        {
            world_hit_point = RayEndpoint(ray, hit->context.distance);
        }

        ISTATUS status = process_hit_routine(process_hit_context,
                                             &hit->context,
                                             NULL,
                                             world_hit_point,
                                             world_hit_point);
        return status;
    }

    PCSHARED_HIT_CONTEXT shared_context = hit->shared_context;
    PCMATRIX model_to_world = shared_context->model_to_world;

    if (shared_context->premultiplied)
    {
        POINT3 world_hit_point;
        if (hit->model_hit_point_valid)
        {
            world_hit_point = hit->model_hit_point;
        }
        else
        {
            world_hit_point = RayEndpoint(ray, hit->context.distance);
        }
        
        POINT3 model_hit_point = 
            PointMatrixInverseMultiplyInline(model_to_world, world_hit_point);

        ISTATUS status = process_hit_routine(process_hit_context,
                                             &hit->context,
                                             model_to_world,
                                             model_hit_point,
                                             world_hit_point);
        return status;
    }

    POINT3 world_hit_point = RayEndpoint(ray, hit->context.distance);

    POINT3 model_hit_point;
    if (hit->model_hit_point_valid)
    {
        model_hit_point = hit->model_hit_point;
    }
    else
    {
        model_hit_point = 
            PointMatrixInverseMultiplyInline(model_to_world, world_hit_point);
    }

    ISTATUS status = process_hit_routine(process_hit_context,
                                         &hit->context,
                                         model_to_world,
                                         model_hit_point,
                                         world_hit_point);
    return status;
}

//
// Functions
//

ISTATUS
RayTracerAllocate(
    _Out_ PRAY_TRACER *ray_tracer
    )
{
    if (ray_tracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    PRAY_TRACER result = (PRAY_TRACER)malloc(sizeof(RAY_TRACER));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    bool success = HitTesterInitialize(&result->hit_tester);

    if (!success)
    {
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    *ray_tracer = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
RayTracerTraceClosestHit(
    _Inout_ PRAY_TRACER ray_tracer,
    _In_ RAY ray,
    _In_ float_t minimum_distance,
    _In_ PRAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ PRAY_TRACER_PROCESS_HIT_ROUTINE process_hit_routine,
    _Inout_opt_ void *process_hit_context
    )
{
    ISTATUS status = RayTracerValidateArugumentsAndTrace(ray_tracer,
                                                         ray,
                                                         minimum_distance,
                                                         trace_routine,
                                                         trace_context,
                                                         process_hit_routine,
                                                         process_hit_context);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PCFULL_HIT_CONTEXT *hits;
    size_t num_hits;
    HitTesterGetHits(&ray_tracer->hit_tester, &hits, &num_hits);

    if (num_hits == 0)
    {
        return ISTATUS_SUCCESS;
    }

    PCFULL_HIT_CONTEXT closest = RayTracerFindClosestHit(hits, num_hits);

    status = process_hit_routine(process_hit_context, &closest->context);

    if (status == ISTATUS_DONE)
    {
        return ISTATUS_SUCCESS;
    }

    return status;
}

ISTATUS
RayTracerTraceClosestHitWithCoordinates(
    _Inout_ PRAY_TRACER ray_tracer,
    _In_ RAY ray,
    _In_ float_t minimum_distance,
    _In_ PRAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ PRAY_TRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE process_hit_routine,
    _Inout_opt_ void *process_hit_context
    )
{
    ISTATUS status = RayTracerValidateArugumentsAndTrace(ray_tracer,
                                                         ray,
                                                         minimum_distance,
                                                         trace_routine,
                                                         trace_context,
                                                         process_hit_routine,
                                                         process_hit_context);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PCFULL_HIT_CONTEXT *hits;
    size_t num_hits;
    HitTesterGetHits(&ray_tracer->hit_tester, &hits, &num_hits);

    if (num_hits == 0)
    {
        return ISTATUS_SUCCESS;
    }

    PCFULL_HIT_CONTEXT closest = RayTracerFindClosestHit(hits, num_hits);

    status = RayTracerProcessHitWithContext(ray,
                                            closest,
                                            process_hit_routine,
                                            process_hit_context);

    if (status == ISTATUS_DONE)
    {
        return ISTATUS_SUCCESS;
    }
    
    return status;
}

ISTATUS
RayTracerTraceAllHits(
    _Inout_ PRAY_TRACER ray_tracer,
    _In_ RAY ray,
    _In_ float_t minimum_distance,
    _In_ PRAY_TRACER_TRACE_ROUTINE trace_routine,
    _In_opt_ const void *trace_context,
    _In_ PRAY_TRACER_PROCESS_HIT_WITH_COORDINATES_ROUTINE process_hit_routine,
    _Inout_opt_ void *process_hit_context
    )
{
    ISTATUS status = RayTracerValidateArugumentsAndTrace(ray_tracer,
                                                         ray,
                                                         minimum_distance,
                                                         trace_routine,
                                                         trace_context,
                                                         process_hit_routine,
                                                         process_hit_context);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    HitTesterSortHits(&ray_tracer->hit_tester);

    PCFULL_HIT_CONTEXT *hits;
    size_t num_hits;
    HitTesterGetHits(&ray_tracer->hit_tester, &hits, &num_hits);

    for (size_t i = 0; i < num_hits; i++)
    {
        status = RayTracerProcessHitWithContext(ray,
                                                hits[i],
                                                process_hit_routine,
                                                process_hit_context);

        if (status == ISTATUS_DONE)
        {
            break;
        }
        
        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }
    
    return ISTATUS_SUCCESS;
}

void
RayTracerFree(
    _In_opt_ _Post_invalid_ PRAY_TRACER ray_tracer
    )
{
    if (ray_tracer == NULL)
    {
        return;
    }

    HitTesterDestroy(&ray_tracer->hit_tester);
    free(ray_tracer);
}