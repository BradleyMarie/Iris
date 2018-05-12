/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    ray_tracer_test.cc

Abstract:

    Unit tests for ray_tracer.c

--*/

extern "C" {
#include "iris/ray_tracer.h"
}

#include "googletest/include/gtest/gtest.h"
#include "test/test_util.h"

#include <vector>

static
ISTATUS 
TraceSceneRoutine(
    _In_opt_ const void *context, 
    _Inout_ PHIT_TESTER hit_tester,
    _In_ RAY ray
    )
{
    return ISTATUS_SUCCESS;
}

static
ISTATUS 
TraceSceneRoutineReturnError(
    _In_opt_ const void *context, 
    _Inout_ PHIT_TESTER hit_tester,
    _In_ RAY ray
    )
{
    return ISTATUS_INVALID_ARGUMENT_COMBINATION_31;
}

static
_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS 
ProcessHitRoutine(
    _Inout_opt_ void *context, 
    _In_ PCHIT_CONTEXT hit_context
    )
{
    return ISTATUS_SUCCESS;
}

static
_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS 
ProcessHitRoutineReturnError(
    _Inout_opt_ void *context, 
    _In_ PCHIT_CONTEXT hit_context
    )
{
    return ISTATUS_INVALID_ARGUMENT_COMBINATION_30;
}

static
_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS 
ProcessHitWithCoordinatesRoutine(
    _Inout_opt_ void *context, 
    _In_ PCHIT_CONTEXT hit_context,
    _In_ PCMATRIX model_to_world,
    _In_ VECTOR3 model_viewer,
    _In_ POINT3 model_hit_point,
    _In_ POINT3 world_hit_point
    )
{
    return ISTATUS_SUCCESS;
}

static
_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS 
ProcessHitWithCoordinatesRoutineReturnError(
    _Inout_opt_ void *context, 
    _In_ PCHIT_CONTEXT hit_context,
    _In_ PCMATRIX model_to_world,
    _In_ VECTOR3 model_viewer,
    _In_ POINT3 model_hit_point,
    _In_ POINT3 world_hit_point
    )
{
    return ISTATUS_INVALID_ARGUMENT_COMBINATION_29;
}

std::vector<PMATRIX>
AllocateMatrices(
    void
    )
{
    std::vector<PMATRIX> matrices;

    PMATRIX mat;
    EXPECT_EQ(ISTATUS_SUCCESS, MatrixAllocateTranslation(0.0, 1.0, 2.0, &mat));
    matrices.push_back(mat);

    EXPECT_EQ(ISTATUS_SUCCESS, MatrixAllocateTranslation(1.0, 2.0, 3.0, &mat));
    matrices.push_back(mat);

    return matrices;
}

void
FreeMatrices(
    std::vector<PMATRIX>* matrices
    )
{
    for (auto& matrix : *matrices)
    {
        MatrixRelease(matrix);
    }

    matrices->clear();
}

TEST(RayTracerTest, RayTracerTraceClosestHitErrors)
{
    PRAY_TRACER ray_tracer;
    EXPECT_EQ(ISTATUS_SUCCESS, RayTracerAllocate(&ray_tracer));

    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);

    float_t minimum_distance = 0.0;

    ISTATUS status = RayTracerTraceClosestHit(nullptr,
                                              ray,
                                              minimum_distance,
                                              TraceSceneRoutine,
                                              nullptr,
                                              ProcessHitRoutine,
                                              nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    ray.origin.x = INFINITY;
    status = RayTracerTraceClosestHit(ray_tracer,
                                      ray,
                                      minimum_distance,
                                      TraceSceneRoutine,
                                      nullptr,
                                      ProcessHitRoutine,
                                      nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    ray.origin.x = (float_t)4.0;

    status = RayTracerTraceClosestHit(ray_tracer,
                                      ray,
                                      (float_t)-1.0f,
                                      TraceSceneRoutine,
                                      nullptr,
                                      ProcessHitRoutine,
                                      nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = RayTracerTraceClosestHit(ray_tracer,
                                      ray,
                                      INFINITY,
                                      TraceSceneRoutine,
                                      nullptr,
                                      ProcessHitRoutine,
                                      nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);
    
    status = RayTracerTraceClosestHit(ray_tracer,
                                      ray,
                                      minimum_distance,
                                      nullptr,
                                      nullptr,
                                      ProcessHitRoutine,
                                      nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);
    
    status = RayTracerTraceClosestHit(ray_tracer,
                                      ray,
                                      minimum_distance,
                                      TraceSceneRoutine,
                                      nullptr,
                                      nullptr,
                                      nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_05, status);

    RayTracerFree(ray_tracer);
}

TEST(RayTracerTest, RayTracerTraceClosestHit)
{
    PRAY_TRACER ray_tracer;
    EXPECT_EQ(ISTATUS_SUCCESS, RayTracerAllocate(&ray_tracer));
    std::vector<PMATRIX> matrices = AllocateMatrices();

    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);

    float_t minimum_distance = 0.0;

    // TODO: Fill In

    RayTracerFree(ray_tracer);
    FreeMatrices(&matrices);
}

TEST(RayTracerTest, RayTracerTraceClosestHitWithCoordinatesErrors)
{
    PRAY_TRACER ray_tracer;
    EXPECT_EQ(ISTATUS_SUCCESS, RayTracerAllocate(&ray_tracer));

    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);

    float_t minimum_distance = 0.0;

    ISTATUS status = RayTracerTraceClosestHitWithCoordinates(nullptr,
                                                             ray,
                                                             minimum_distance,
                                                             TraceSceneRoutine,
                                                             nullptr,
                                                             ProcessHitWithCoordinatesRoutine,
                                                             nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    ray.origin.x = INFINITY;
    status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                     ray,
                                                     minimum_distance,
                                                     TraceSceneRoutine,
                                                     nullptr,
                                                     ProcessHitWithCoordinatesRoutine,
                                                     nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    ray.origin.x = (float_t)4.0;

    status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                     ray,
                                                     (float_t)-1.0f,
                                                     TraceSceneRoutine,
                                                     nullptr,
                                                     ProcessHitWithCoordinatesRoutine,
                                                     nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                     ray,
                                                     INFINITY,
                                                     TraceSceneRoutine,
                                                     nullptr,
                                                     ProcessHitWithCoordinatesRoutine,
                                                     nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);
    
    status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                     ray,
                                                     minimum_distance,
                                                     nullptr,
                                                     nullptr,
                                                     ProcessHitWithCoordinatesRoutine,
                                                     nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);
    
    status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                     ray,
                                                     minimum_distance,
                                                     TraceSceneRoutine,
                                                     nullptr,
                                                     nullptr,
                                                     nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_05, status);

    RayTracerFree(ray_tracer);
}

TEST(RayTracerTest, RayTracerTraceAllHitsErrors)
{
    PRAY_TRACER ray_tracer;
    EXPECT_EQ(ISTATUS_SUCCESS, RayTracerAllocate(&ray_tracer));

    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);

    float_t minimum_distance = 0.0;

    ISTATUS status = RayTracerTraceAllHits(nullptr,
                                           ray,
                                           minimum_distance,
                                           TraceSceneRoutine,
                                           nullptr,
                                           ProcessHitWithCoordinatesRoutine,
                                           nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    ray.origin.x = INFINITY;
    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   minimum_distance,
                                   TraceSceneRoutine,
                                   nullptr,
                                   ProcessHitWithCoordinatesRoutine,
                                   nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    ray.origin.x = (float_t)4.0;

    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   (float_t)-1.0f,
                                   TraceSceneRoutine,
                                   nullptr,
                                   ProcessHitWithCoordinatesRoutine,
                                   nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   INFINITY,
                                   TraceSceneRoutine,
                                   nullptr,
                                   ProcessHitWithCoordinatesRoutine,
                                   nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);
    
    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   minimum_distance,
                                   nullptr,
                                   nullptr,
                                   ProcessHitWithCoordinatesRoutine,
                                   nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);
    
    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   minimum_distance,
                                   TraceSceneRoutine,
                                   nullptr,
                                   nullptr,
                                   nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_05, status);

    RayTracerFree(ray_tracer);
}