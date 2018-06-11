/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    reflector_allocator_context_test.cc

Abstract:

    Unit tests for reflector_allocator_context.c

--*/

extern "C" {
#include "iris_spectrum/reflector_allocator_context.h"
}

#include "googletest/include/gtest/gtest.h"

TEST(ReflectorAllocatorContext, ReflectorAllocatorContextCreateErrors)
{
    ISTATUS status = ReflectorAllocatorContextCreate(nullptr, nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);
}

struct CallbackContext {
    void* address;
    bool encountered;
    ISTATUS result;
};

ISTATUS
ReflectorAllocatorContextCreateCallback(
    _Inout_opt_ void *context,
    _In_ PREFLECTOR_ALLOCATOR_CONTEXT allocator_context
    )
{
    EXPECT_TRUE(allocator_context != nullptr);
    auto *data = static_cast<CallbackContext*>(context);
    EXPECT_EQ(data, data->address);
    EXPECT_FALSE(data->encountered);
    data->encountered = true;
    return data->result;
}

TEST(ReflectorAllocatorContext, ReflectorAllocatorContextCreate)
{
    CallbackContext context = { &context, false, ISTATUS_INVALID_ARGUMENT_31 };
    ISTATUS status = ReflectorAllocatorContextCreate(
        ReflectorAllocatorContextCreateCallback, &context);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_31, status);
    EXPECT_TRUE(context.encountered);

    context = { &context, false, ISTATUS_SUCCESS };
    status = ReflectorAllocatorContextCreate(
        ReflectorAllocatorContextCreateCallback, &context);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(context.encountered);
}

ISTATUS
ReflectorAllocatorCreateCallback(
    _Inout_opt_ void *context,
    _In_ PREFLECTOR_ALLOCATOR compositor
    )
{
    EXPECT_TRUE(compositor != nullptr);
    auto *data = static_cast<CallbackContext*>(context);
    EXPECT_EQ(data, data->address);
    EXPECT_FALSE(data->encountered);
    data->encountered = true;
    return data->result;
}

ISTATUS
ReflectorAllocatorCreateCallbackContext(
    _Inout_opt_ void *data,
    _In_ PREFLECTOR_ALLOCATOR_CONTEXT allocator_context
    )
{
    EXPECT_TRUE(!data);

    ISTATUS status = ReflectorAllocatorCreate(
        nullptr, ReflectorAllocatorCreateCallback, nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = ReflectorAllocatorCreate(allocator_context, nullptr, nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    CallbackContext context = { &context, false, ISTATUS_INVALID_ARGUMENT_31 };
    status = ReflectorAllocatorCreate(
        allocator_context, ReflectorAllocatorCreateCallback, &context);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_31, status);
    EXPECT_TRUE(context.encountered);

    context = { &context, false, ISTATUS_SUCCESS };
    status = ReflectorAllocatorCreate(
        allocator_context, ReflectorAllocatorCreateCallback, &context);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(context.encountered);

    return ISTATUS_SUCCESS;
}

TEST(ReflectorAllocatorContext, ReflectorAllocatorCreate)
{
    ISTATUS status = ReflectorAllocatorContextCreate(
        ReflectorAllocatorCreateCallbackContext, nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
}