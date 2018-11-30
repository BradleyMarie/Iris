/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    reflector_test.cc

Abstract:

    Unit tests for reflector.c

--*/

extern "C" {
#include "iris_physx/reflector.h"
}

#include <limits>

#include "googletest/include/gtest/gtest.h"

TEST(ReflectorTest, ReflectorAllocateErrors)
{
    PREFLECTOR reflector;
    REFLECTOR_VTABLE vtable;

    ISTATUS status = ReflectorAllocate(NULL,
                                       NULL,
                                       0,
                                       0,
                                       &reflector);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = ReflectorAllocate(&vtable,
                               NULL,
                               0,
                               0,
                               NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_04, status);

    status = ReflectorAllocate(&vtable,
                               NULL,
                               16,
                               1,
                               &reflector);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, status);

    int data = 0;
    status = ReflectorAllocate(&vtable,
                               &data,
                               sizeof(int),
                               0,
                               &reflector);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_01, status);

    int data2[3] = { 0, 0, 0 };
    status = ReflectorAllocate(&vtable,
                               &data2,
                               sizeof(int),
                               8,
                               &reflector);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_02, status);
}

TEST(ReflectorTest, ReflectorReflectNull)
{
    float_t intensity = (float_t)1.0;
    ISTATUS status = ReflectorReflect(NULL, 
                                      (float_t)1.0,
                                      (float_t)1.0,
                                      &intensity);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, intensity);
}

struct TestContext {
    ISTATUS return_status;
    float_t wavelength;
    float_t incoming_intensity;
    float_t outgoing_intensity;
    bool *encountered;
};

ISTATUS
SampleRoutine(
    _In_ const void *data,
    _In_ float_t wavelength,
    _In_ float_t incoming_intensity,
    _Out_ float_t *outgoing_intensity
    )
{
    const TestContext *context = static_cast<const TestContext*>(data);
    EXPECT_FALSE(*context->encountered);
    *context->encountered = true;

    EXPECT_EQ(context->wavelength, wavelength);
    EXPECT_EQ(context->incoming_intensity, incoming_intensity);
    *outgoing_intensity = context->outgoing_intensity;
    return context->return_status;
}

TEST(ReflectorTest, ReflectorReflectErrors)
{
    bool encountered = false;
    TestContext context = {
        ISTATUS_INTEGER_OVERFLOW,
        INFINITY,
        (float_t)1.0,
        INFINITY,
        &encountered
    };

    REFLECTOR_VTABLE vtable = { SampleRoutine, NULL, NULL };
    PREFLECTOR reflector;

    ISTATUS status = ReflectorAllocate(&vtable,
                                       &context,
                                       sizeof(TestContext),
                                       alignof(TestContext),
                                       &reflector);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t intensity;
    status = ReflectorReflect(reflector, (float_t)0.0, (float_t)1.0, &intensity);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    EXPECT_FALSE(encountered);

    status = ReflectorReflect(reflector, (float_t)-1.0,(float_t)1.0, &intensity);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    EXPECT_FALSE(encountered);

    status = ReflectorReflect(reflector, -INFINITY, (float_t)1.0,&intensity);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    status = ReflectorReflect(reflector, INFINITY, (float_t)1.0,&intensity);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    EXPECT_FALSE(encountered);

    status = ReflectorReflect(reflector,
                              std::numeric_limits<float_t>::quiet_NaN(),
                              (float_t)1.0,
                              &intensity);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    EXPECT_FALSE(encountered);

    status = ReflectorReflect(reflector, (float_t)1.0, (float_t)1.0, NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);
    EXPECT_FALSE(encountered);

    ReflectorRelease(reflector);
}

TEST(ReflectorTest, ReflectorReflect)
{
    bool encountered = false;
    TestContext context = { 
        ISTATUS_INTEGER_OVERFLOW,
        (float_t)5.0,
        (float_t)10.0,
        (float_t)1.0,
        &encountered
    };

    REFLECTOR_VTABLE vtable = { SampleRoutine, NULL, NULL };
    PREFLECTOR reflector;

    ISTATUS status = ReflectorAllocate(&vtable,
                                       &context,
                                       sizeof(TestContext),
                                       alignof(TestContext),
                                       &reflector);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t intensity;
    status = ReflectorReflect(reflector, 
                              (float_t)5.0,
                              (float_t)10.0,
                              &intensity);

    EXPECT_EQ(ISTATUS_INTEGER_OVERFLOW, status);
    EXPECT_EQ((float_t)1.0, intensity);
    EXPECT_TRUE(encountered);

    ReflectorRelease(reflector);

    encountered = false;
    context = {
        ISTATUS_SUCCESS,
        (float_t)5.0,
        (float_t)2.0,
        (float_t)1.0,
        &encountered
    };

    status = ReflectorAllocate(&vtable,
                               &context,
                               sizeof(TestContext),
                               alignof(TestContext),
                               &reflector);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = ReflectorReflect(reflector, 
                              (float_t)5.0,
                              (float_t)2.0,
                              &intensity);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, intensity);
    EXPECT_TRUE(encountered);

    ReflectorRelease(reflector);
}

struct TestGetAlbedoContext {
    ISTATUS return_status;
    float_t albedo;
    bool *encountered;
};

ISTATUS
GetAlbedoRoutine(
    _In_ const void *data,
    _Out_ float_t *albedo
    )
{
    const TestGetAlbedoContext *context = 
        static_cast<const TestGetAlbedoContext*>(data);
    EXPECT_FALSE(*context->encountered);
    *context->encountered = true;
    *albedo = context->albedo;
    return context->return_status;
}

TEST(ReflectorTest, ReflectorGetAlbedoErrors)
{
    bool encountered = false;
    TestGetAlbedoContext context = {
        ISTATUS_INTEGER_OVERFLOW,
        (float_t)1.0,
        &encountered
    };

    REFLECTOR_VTABLE vtable = { NULL, GetAlbedoRoutine, NULL };
    PREFLECTOR reflector;

    ISTATUS status = ReflectorAllocate(&vtable,
                                       &context,
                                       sizeof(TestGetAlbedoContext),
                                       alignof(TestGetAlbedoContext),
                                       &reflector);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t albedo;
    status = ReflectorGetAlbedo(NULL, &albedo);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);
    EXPECT_FALSE(encountered);

    status = ReflectorGetAlbedo(reflector, NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    EXPECT_FALSE(encountered);

    ReflectorRelease(reflector);
}

TEST(ReflectorTest, ReflectorGetAlbedo)
{
    bool encountered = false;
    TestGetAlbedoContext context = {
        ISTATUS_SUCCESS,
        (float_t)1.0,
        &encountered
    };

    REFLECTOR_VTABLE vtable = { NULL, GetAlbedoRoutine, NULL };
    PREFLECTOR reflector;

    ISTATUS status = ReflectorAllocate(&vtable,
                                       &context,
                                       sizeof(TestGetAlbedoContext),
                                       alignof(TestGetAlbedoContext),
                                       &reflector);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t albedo;
    status = ReflectorGetAlbedo(reflector, &albedo);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, albedo);
    EXPECT_TRUE(encountered);

    ReflectorRelease(reflector);

    encountered = false;
    context = {
        ISTATUS_INVALID_ARGUMENT_COMBINATION_00,
        (float_t)1.0,
        &encountered
    };

    status = ReflectorAllocate(&vtable,
                               &context,
                               sizeof(TestGetAlbedoContext),
                               alignof(TestGetAlbedoContext),
                               &reflector);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = ReflectorGetAlbedo(reflector, &albedo);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, status);
    EXPECT_TRUE(encountered);

    ReflectorRelease(reflector);
}

struct FreeTestContext {
    void *address;
    bool *freed;
};

void
FreeRoutine(
    void *context
    )
{
    FreeTestContext *data = static_cast<FreeTestContext*>(context);
    EXPECT_NE(data->address, context);
    EXPECT_FALSE(*data->freed);
    *data->freed = true;
}

TEST(ReflectorTest, ReflectorFree)
{
    bool freed = false;
    REFLECTOR_VTABLE vtable = { NULL, NULL, NULL };
    FreeTestContext context = { &context, &freed };
    PREFLECTOR reflector;

    ISTATUS status = ReflectorAllocate(&vtable,
                                       &context,
                                       sizeof(context),
                                       alignof(context),
                                       &reflector);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    ReflectorRelease(reflector);
    EXPECT_FALSE(freed);

    vtable.free_routine = FreeRoutine;
    status = ReflectorAllocate(&vtable,
                               &context,
                               sizeof(context),
                               alignof(context),
                               &reflector);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    ReflectorRetain(reflector);
    EXPECT_FALSE(freed);

    ReflectorRelease(reflector);
    EXPECT_FALSE(freed);

    ReflectorRelease(reflector);
    EXPECT_TRUE(freed);
}