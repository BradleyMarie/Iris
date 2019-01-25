/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    reflector_allocator_test.cc

Abstract:

    Unit tests for reflector_allocator.c

--*/

extern "C" {
#include "iris_physx/reflector_allocator.h"
#include "iris_physx/reflector_allocator_test_util.h"
}

#include "googletest/include/gtest/gtest.h"

TEST(ReflectorAllocatorTest, ReflectorAllocatorAllocateErrors)
{
    PREFLECTOR_ALLOCATOR allocator = ReflectorAllocatorCreate();
    ASSERT_TRUE(allocator != NULL);

    PCREFLECTOR reflector;
    REFLECTOR_VTABLE vtable;

    ISTATUS status = ReflectorAllocatorAllocate(NULL,
                                                &vtable,
                                                NULL,
                                                0,
                                                0,
                                                &reflector);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = ReflectorAllocatorAllocate(allocator,
                                        NULL,
                                        NULL,
                                        0,
                                        0,
                                        NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    status = ReflectorAllocatorAllocate(allocator,
                                        &vtable,
                                        NULL,
                                        0,
                                        0,
                                        NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_05, status);

    status = ReflectorAllocatorAllocate(allocator,
                                        &vtable,
                                        NULL,
                                        16,
                                        1,
                                        &reflector);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, status);

    int data = 0;
    status = ReflectorAllocatorAllocate(allocator,
                                        &vtable,
                                        &data,
                                        sizeof(int),
                                        0,
                                        &reflector);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_01, status);

    int data2[3] = { 0, 0, 0 };
    status = ReflectorAllocatorAllocate(allocator,
                                        &vtable,
                                        &data2,
                                        sizeof(int),
                                        8,
                                        &reflector);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_02, status);

    ReflectorAllocatorFree(allocator);
}

struct ReflectorContext {
    bool reflect_encountered;
    bool get_albedo_encountered;
    bool free_encountered;
};

ISTATUS
ReflectorReflectRoutine(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    )
{
    auto data = static_cast<ReflectorContext* const *>(context);
    EXPECT_FALSE((*data)->reflect_encountered);
    (*data)->reflect_encountered = true;
    *reflectance = (float_t) 1.0;
    return ISTATUS_SUCCESS;
}

ISTATUS
ReflectorGetAlbedoRoutine(
    _In_ const void *context,
    _Out_ float_t *albedo
    )
{
    auto data = static_cast<ReflectorContext* const *>(context);
    EXPECT_FALSE((*data)->get_albedo_encountered);
    (*data)->get_albedo_encountered = true;
    *albedo = (float_t) 1.0;
    return ISTATUS_SUCCESS;
}

void
ReflectorFreeRoutine(
    _In_ _Post_invalid_ void *pointer
    )
{
    auto context = static_cast<ReflectorContext**>(pointer);
    (*context)->free_encountered = true;
}

TEST(ReflectorAllocatorTest, ReflectorAllocatorAllocate)
{
    PREFLECTOR_ALLOCATOR allocator = ReflectorAllocatorCreate();
    ASSERT_TRUE(allocator != NULL);

    PCREFLECTOR reflector;
    REFLECTOR_VTABLE vtable = { ReflectorReflectRoutine, 
                                ReflectorGetAlbedoRoutine,
                                ReflectorFreeRoutine };

    ReflectorContext context = { false, false, false };
    ReflectorContext *data = &context;
    
    ISTATUS status = ReflectorAllocatorAllocate(allocator,
                                                &vtable,
                                                &data,
                                                sizeof(ReflectorContext*),
                                                alignof(ReflectorContext*),
                                                &reflector);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t outgoing;
    status = ReflectorReflect(reflector, (float_t)1.0, &outgoing);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, outgoing);
    EXPECT_TRUE(context.reflect_encountered);

    float_t albedo;
    status = ReflectorGetAlbedo(reflector, &albedo);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, albedo);
    EXPECT_TRUE(context.get_albedo_encountered);

    ReflectorAllocatorFree(allocator);
    EXPECT_FALSE(context.free_encountered);
}