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

struct HitData {
    float_t distance;
    bool set_model_hit;
    POINT3 model_hit;
};

static
ISTATUS 
AllocateHitRoutine(
    _In_opt_ const void *data, 
    _In_ PCRAY ray,
    _Inout_ PHIT_ALLOCATOR hit_allocator,
    _Pre_null_ _Outptr_result_maybenull_ PHIT *hits
    )
{
    auto hit_data = static_cast<const HitData*>(data);
    
    if (hit_data->set_model_hit)
    {
        ISTATUS status = HitAllocatorAllocate(hit_allocator,
                                              nullptr,
                                              hit_data->distance,
                                              (uint32_t)hit_data->distance,
                                              (uint32_t)hit_data->distance,
                                              nullptr,
                                              0,
                                              0,
                                              hits);
        EXPECT_EQ(ISTATUS_SUCCESS, status);
    }
    else
    {
        ISTATUS status = HitAllocatorAllocateWithHitPoint(hit_allocator,
                                                          nullptr,
                                                          hit_data->distance,
                                                          (uint32_t)hit_data->distance,
                                                          (uint32_t)hit_data->distance,
                                                          nullptr,
                                                          0,
                                                          0,
                                                          hit_data->model_hit,
                                                          hits);
        EXPECT_EQ(ISTATUS_SUCCESS, status);
    }

    return ISTATUS_SUCCESS;
}

struct GeometryData {
    HitData hit_data;
    PMATRIX matrix;
    bool premultiplied;
};

typedef std::vector<GeometryData> GeometryDataList;

GeometryDataList
AllocateGeometryData(
    void
    )
{
    GeometryDataList result;

    GeometryData data;

    // First Hit
    data.hit_data.distance = (float_t)1.0;
    data.hit_data.set_model_hit = false;
    data.matrix = nullptr;
    data.premultiplied = false;
    result.push_back(data);

    // Second Hit
    data.hit_data.distance = (float_t)2.0;
    data.hit_data.set_model_hit = true;
    data.hit_data.model_hit = PointCreate(200.0, 200.0, 200.0);
    data.matrix = nullptr;
    data.premultiplied = false;
    result.push_back(data);

    // Third Hit
    data.hit_data.distance = (float_t)3.0;
    data.hit_data.set_model_hit = false;
    EXPECT_EQ(ISTATUS_SUCCESS, 
              MatrixAllocateTranslation(0.0, 1.0, 2.0, &data.matrix));
    data.premultiplied = false;
    result.push_back(data);

    // Fourth Hit
    data.hit_data.distance = (float_t)4.0;
    data.hit_data.set_model_hit = true;
    data.hit_data.model_hit = PointCreate(400.0, 400.0, 400.0);
    EXPECT_EQ(ISTATUS_SUCCESS, 
              MatrixAllocateTranslation(1.0, 2.0, 3.0, &data.matrix));
    data.premultiplied = false;
    result.push_back(data);

    // Fifth Hit
    data.hit_data.distance = (float_t)5.0;
    data.hit_data.set_model_hit = false;
    EXPECT_EQ(ISTATUS_SUCCESS, 
              MatrixAllocateTranslation(2.0, 3.0, 4.0, &data.matrix));
    data.premultiplied = true;
    result.push_back(data);

    // Sixth Hit
    data.hit_data.distance = (float_t)6.0;
    data.hit_data.set_model_hit = true;
    data.hit_data.model_hit = PointCreate(600.0, 600.0, 600.0);
    EXPECT_EQ(ISTATUS_SUCCESS, 
              MatrixAllocateTranslation(3.0, 4.0, 5.0, &data.matrix));
    data.premultiplied = true;
    result.push_back(data);

    return result;
}

void
FreeGeometryData(
    GeometryDataList* data
    )
{
    for (const auto& entry : *data)
    {
        MatrixRelease(entry.matrix);
    }

    data->clear();
}

static
ISTATUS 
TraceSceneRoutine(
    _In_opt_ const void *context, 
    _Inout_ PHIT_TESTER hit_tester,
    _In_ RAY ray
    )
{
    auto geometry_data = static_cast<const GeometryDataList*>(context);
    for (const auto& entry : *geometry_data)
    {
        ISTATUS status = HitTesterTestGeometry(hit_tester,
                                               AllocateHitRoutine,
                                               &entry.hit_data,
                                               nullptr,
                                               entry.matrix,
                                               entry.premultiplied);
        EXPECT_EQ(ISTATUS_SUCCESS, status);
    }
    return ISTATUS_SUCCESS;
}

static
ISTATUS 
TraceSceneRoutineEmpty(
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

struct ProcessHitData {
    size_t last_hit_to_process;
    size_t hits_processed;
    bool sorted;
    float_t distance;
};

static
_Check_return_
_Success_(return == 0 || return == 1)
ISTATUS 
ProcessHitRoutine(
    _Inout_opt_ void *context, 
    _In_ PCHIT_CONTEXT hit_context
    )
{
    auto process_data = static_cast<ProcessHitData*>(context);
    EXPECT_GE(process_data->last_hit_to_process, process_data->hits_processed);
    process_data->hits_processed++;

    if (process_data->sorted)
    {
        EXPECT_EQ(process_data->distance, hit_context->distance);
        process_data->distance += (float_t)1.0;
    }

    if (process_data->last_hit_to_process < process_data->hits_processed)
    {
        return ISTATUS_DONE;
    }

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
    auto process_data = static_cast<ProcessHitData*>(context);
    EXPECT_GE(process_data->last_hit_to_process, process_data->hits_processed);
    process_data->hits_processed++;

    if (process_data->sorted)
    {
        EXPECT_EQ(process_data->distance, hit_context->distance);
        process_data->distance += (float_t)1.0;
    }
    
    if (process_data->last_hit_to_process < process_data->hits_processed)
    {
        return ISTATUS_DONE;
    }
    
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

TEST(RayTracerTest, RayTracerTraceClosestHitErrors)
{
    PRAY_TRACER ray_tracer;
    EXPECT_EQ(ISTATUS_SUCCESS, RayTracerAllocate(&ray_tracer));
    auto geometry_data = AllocateGeometryData();

    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);

    float_t minimum_distance = 0.0;

    ISTATUS status = RayTracerTraceClosestHit(nullptr,
                                              ray,
                                              minimum_distance,
                                              TraceSceneRoutine,
                                              &geometry_data,
                                              ProcessHitRoutine,
                                              nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    ray.origin.x = INFINITY;
    status = RayTracerTraceClosestHit(ray_tracer,
                                      ray,
                                      minimum_distance,
                                      TraceSceneRoutine,
                                      &geometry_data,
                                      ProcessHitRoutine,
                                      nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    ray.origin.x = (float_t)4.0;

    status = RayTracerTraceClosestHit(ray_tracer,
                                      ray,
                                      (float_t)-1.0f,
                                      TraceSceneRoutine,
                                      &geometry_data,
                                      ProcessHitRoutine,
                                      nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = RayTracerTraceClosestHit(ray_tracer,
                                      ray,
                                      INFINITY,
                                      TraceSceneRoutine,
                                      &geometry_data,
                                      ProcessHitRoutine,
                                      nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);
    
    status = RayTracerTraceClosestHit(ray_tracer,
                                      ray,
                                      minimum_distance,
                                      nullptr,
                                      &geometry_data,
                                      ProcessHitRoutine,
                                      nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);
    
    status = RayTracerTraceClosestHit(ray_tracer,
                                      ray,
                                      minimum_distance,
                                      TraceSceneRoutine,
                                      &geometry_data,
                                      nullptr,
                                      nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_05, status);
    
    status = RayTracerTraceClosestHit(ray_tracer,
                                      ray,
                                      minimum_distance,
                                      TraceSceneRoutine,
                                      &geometry_data,
                                      ProcessHitRoutineReturnError,
                                      nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_30, status);
    
    status = RayTracerTraceClosestHit(ray_tracer,
                                      ray,
                                      minimum_distance,
                                      TraceSceneRoutineReturnError,
                                      &geometry_data,
                                      ProcessHitRoutineReturnError,
                                      nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);

    RayTracerFree(ray_tracer);
    FreeGeometryData(&geometry_data);
}

TEST(RayTracerTest, RayTracerTraceClosestHitEmpty)
{
    PRAY_TRACER ray_tracer;
    EXPECT_EQ(ISTATUS_SUCCESS, RayTracerAllocate(&ray_tracer));

    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);

    float_t minimum_distance = 0.0;
    
    ISTATUS status = RayTracerTraceClosestHit(ray_tracer,
                                              ray,
                                              minimum_distance,
                                              TraceSceneRoutineEmpty,
                                              nullptr,
                                              ProcessHitRoutineReturnError,
                                              nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    RayTracerFree(ray_tracer);
}

TEST(RayTracerTest, RayTracerTraceClosestHit)
{
    PRAY_TRACER ray_tracer;
    EXPECT_EQ(ISTATUS_SUCCESS, RayTracerAllocate(&ray_tracer));
    auto geometry_data = AllocateGeometryData();

    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);

    float_t minimum_distance = 0.0;
    ProcessHitData process_data;
    process_data.hits_processed = 0;
    process_data.last_hit_to_process = 6;
    process_data.sorted = true;
    process_data.distance = 1.0;
    
    ISTATUS status = RayTracerTraceClosestHit(ray_tracer,
                                              ray,
                                              minimum_distance,
                                              TraceSceneRoutine,
                                              &geometry_data,
                                              ProcessHitRoutine,
                                              &process_data);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(1u, process_data.hits_processed);
    
    minimum_distance = 1.5;
    process_data.hits_processed = 0;
    process_data.last_hit_to_process = 0;
    process_data.sorted = true;
    process_data.distance = 2.0;
    
    status = RayTracerTraceClosestHit(ray_tracer,
                                      ray,
                                      minimum_distance,
                                      TraceSceneRoutine,
                                      &geometry_data,
                                      ProcessHitRoutine,
                                      &process_data);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(1u, process_data.hits_processed);

    RayTracerFree(ray_tracer);
    FreeGeometryData(&geometry_data);
}

TEST(RayTracerTest, RayTracerTraceClosestHitWithCoordinatesErrors)
{
    PRAY_TRACER ray_tracer;
    EXPECT_EQ(ISTATUS_SUCCESS, RayTracerAllocate(&ray_tracer));
    auto geometry_data = AllocateGeometryData();

    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);

    float_t minimum_distance = 0.0;

    ISTATUS status = RayTracerTraceClosestHitWithCoordinates(nullptr,
                                                             ray,
                                                             minimum_distance,
                                                             TraceSceneRoutine,
                                                             &geometry_data,
                                                             ProcessHitWithCoordinatesRoutine,
                                                             nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    ray.origin.x = INFINITY;
    status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                     ray,
                                                     minimum_distance,
                                                     TraceSceneRoutine,
                                                     &geometry_data,
                                                     ProcessHitWithCoordinatesRoutine,
                                                     nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    ray.origin.x = (float_t)4.0;

    status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                     ray,
                                                     (float_t)-1.0f,
                                                     TraceSceneRoutine,
                                                     &geometry_data,
                                                     ProcessHitWithCoordinatesRoutine,
                                                     nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                     ray,
                                                     INFINITY,
                                                     TraceSceneRoutine,
                                                     &geometry_data,
                                                     ProcessHitWithCoordinatesRoutine,
                                                     nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);
    
    status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                     ray,
                                                     minimum_distance,
                                                     nullptr,
                                                     &geometry_data,
                                                     ProcessHitWithCoordinatesRoutine,
                                                     nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);
    
    status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                     ray,
                                                     minimum_distance,
                                                     TraceSceneRoutine,
                                                     &geometry_data,
                                                     nullptr,
                                                     nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_05, status);
    
    status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                     ray,
                                                     minimum_distance,
                                                     TraceSceneRoutine,
                                                     &geometry_data,
                                                     ProcessHitWithCoordinatesRoutineReturnError,
                                                     nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_29, status);
    
    status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                     ray,
                                                     minimum_distance,
                                                     TraceSceneRoutineReturnError,
                                                     &geometry_data,
                                                     ProcessHitWithCoordinatesRoutineReturnError,
                                                     nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);

    RayTracerFree(ray_tracer);
    FreeGeometryData(&geometry_data);
}

TEST(RayTracerTest, RayTracerTraceClosestHitWithCoordinatesEmpty)
{
    PRAY_TRACER ray_tracer;
    EXPECT_EQ(ISTATUS_SUCCESS, RayTracerAllocate(&ray_tracer));

    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);

    float_t minimum_distance = 0.0;
    
    ISTATUS status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                             ray,
                                                             minimum_distance,
                                                             TraceSceneRoutineEmpty,
                                                             nullptr,
                                                             ProcessHitWithCoordinatesRoutineReturnError,
                                                             nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    RayTracerFree(ray_tracer);
}

TEST(RayTracerTest, RayTracerTraceClosestHitWithCoordinates)
{
    PRAY_TRACER ray_tracer;
    EXPECT_EQ(ISTATUS_SUCCESS, RayTracerAllocate(&ray_tracer));
    auto geometry_data = AllocateGeometryData();

    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);

    float_t minimum_distance = 0.0;
    ProcessHitData process_data;
    process_data.hits_processed = 0;
    process_data.last_hit_to_process = 6;
    process_data.sorted = true;
    process_data.distance = 1.0;
    
    ISTATUS status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                             ray,
                                                             minimum_distance,
                                                             TraceSceneRoutine,
                                                             &geometry_data,
                                                             ProcessHitWithCoordinatesRoutine,
                                                             &process_data);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(1u, process_data.hits_processed);
    
    minimum_distance = 1.5;
    process_data.hits_processed = 0;
    process_data.last_hit_to_process = 0;
    process_data.sorted = true;
    process_data.distance = 2.0;
    
    status = RayTracerTraceClosestHitWithCoordinates(ray_tracer,
                                                     ray,
                                                     minimum_distance,
                                                     TraceSceneRoutine,
                                                     &geometry_data,
                                                     ProcessHitWithCoordinatesRoutine,
                                                     &process_data);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(1u, process_data.hits_processed);

    RayTracerFree(ray_tracer);
    FreeGeometryData(&geometry_data);
}

TEST(RayTracerTest, RayTracerTraceAllHitsErrors)
{
    PRAY_TRACER ray_tracer;
    EXPECT_EQ(ISTATUS_SUCCESS, RayTracerAllocate(&ray_tracer));
    auto geometry_data = AllocateGeometryData();

    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);

    float_t minimum_distance = 0.0;

    ISTATUS status = RayTracerTraceAllHits(nullptr,
                                           ray,
                                           minimum_distance,
                                           TraceSceneRoutine,
                                           &geometry_data,
                                           ProcessHitWithCoordinatesRoutine,
                                           nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    ray.origin.x = INFINITY;
    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   minimum_distance,
                                   TraceSceneRoutine,
                                   &geometry_data,
                                   ProcessHitWithCoordinatesRoutine,
                                   nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    ray.origin.x = (float_t)4.0;

    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   (float_t)-1.0f,
                                   TraceSceneRoutine,
                                   &geometry_data,
                                   ProcessHitWithCoordinatesRoutine,
                                   nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   INFINITY,
                                   TraceSceneRoutine,
                                   &geometry_data,
                                   ProcessHitWithCoordinatesRoutine,
                                   nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);
    
    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   minimum_distance,
                                   nullptr,
                                   &geometry_data,
                                   ProcessHitWithCoordinatesRoutine,
                                   nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);
    
    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   minimum_distance,
                                   TraceSceneRoutine,
                                   &geometry_data,
                                   nullptr,
                                   nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_05, status);
    
    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   minimum_distance,
                                   TraceSceneRoutine,
                                   &geometry_data,
                                   ProcessHitWithCoordinatesRoutineReturnError,
                                   nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_29, status);
    
    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   minimum_distance,
                                   TraceSceneRoutineReturnError,
                                   &geometry_data,
                                   ProcessHitWithCoordinatesRoutineReturnError,
                                   nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_31, status);

    RayTracerFree(ray_tracer);
    FreeGeometryData(&geometry_data);
}

TEST(RayTracerTest, RayTracerTraceAllHitsEmpty)
{
    PRAY_TRACER ray_tracer;
    EXPECT_EQ(ISTATUS_SUCCESS, RayTracerAllocate(&ray_tracer));

    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);

    float_t minimum_distance = 0.0;
    
    ISTATUS status = RayTracerTraceAllHits(ray_tracer,
                                           ray,
                                           minimum_distance,
                                           TraceSceneRoutineEmpty,
                                           nullptr,
                                           ProcessHitWithCoordinatesRoutineReturnError,
                                           nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    RayTracerFree(ray_tracer);
}

TEST(RayTracerTest, RayTracerTraceAllHits)
{
    PRAY_TRACER ray_tracer;
    EXPECT_EQ(ISTATUS_SUCCESS, RayTracerAllocate(&ray_tracer));
    auto geometry_data = AllocateGeometryData();

    VECTOR3 v0 = VectorCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    POINT3 p0 = PointCreate((float_t) 4.0, (float_t) 3.0, (float_t) 2.0);
    RAY ray = RayCreate(p0, v0);

    float_t minimum_distance = 0.0;
    ProcessHitData process_data;
    process_data.hits_processed = 0;
    process_data.last_hit_to_process = 6;
    process_data.sorted = true;
    process_data.distance = 1.0;
    
    ISTATUS status = RayTracerTraceAllHits(ray_tracer,
                                           ray,
                                           minimum_distance,
                                           TraceSceneRoutine,
                                           &geometry_data,
                                           ProcessHitWithCoordinatesRoutine,
                                           &process_data);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(6u, process_data.hits_processed);
    
    minimum_distance = 1.5;
    process_data.hits_processed = 0;
    process_data.last_hit_to_process = 6;
    process_data.sorted = true;
    process_data.distance = 2.0;
    
    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   minimum_distance,
                                   TraceSceneRoutine,
                                   &geometry_data,
                                   ProcessHitWithCoordinatesRoutine,
                                   &process_data);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(5u, process_data.hits_processed);
    
    minimum_distance = 0.0;
    process_data.hits_processed = 0;
    process_data.last_hit_to_process = 2;
    process_data.sorted = true;
    process_data.distance = 1.0;
    
    status = RayTracerTraceAllHits(ray_tracer,
                                   ray,
                                   minimum_distance,
                                   TraceSceneRoutine,
                                   &geometry_data,
                                   ProcessHitWithCoordinatesRoutine,
                                   &process_data);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(3u, process_data.hits_processed);

    RayTracerFree(ray_tracer);
    FreeGeometryData(&geometry_data);
}