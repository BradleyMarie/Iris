/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    spectrum_compositor_context_test.cc

Abstract:

    Unit tests for spectrum_compositor_context.c

--*/

extern "C" {
#include "iris_spectrum/spectrum_compositor_context.h"
}

#include "googletest/include/gtest/gtest.h"

TEST(SpectrumCompositorContext, SpectrumCompositorContextCreateErrors)
{
    ISTATUS status = SpectrumCompositorContextCreate(nullptr, nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);
}

struct CallbackContext {
    void* address;
    bool encountered;
    ISTATUS result;
};

ISTATUS
SpectrumCompositorContextCreateCallback(
    _Inout_opt_ void *context,
    _In_ PSPECTRUM_COMPOSITOR_CONTEXT compositor_context
    )
{
    EXPECT_TRUE(compositor_context != nullptr);
    auto *data = static_cast<CallbackContext*>(context);
    EXPECT_EQ(data, data->address);
    EXPECT_FALSE(data->encountered);
    data->encountered = true;
    return data->result;
}

TEST(SpectrumCompositorContext, SpectrumCompositorContextCreate)
{
    CallbackContext context = { &context, false, ISTATUS_INVALID_ARGUMENT_31 };
    ISTATUS status = SpectrumCompositorContextCreate(
        SpectrumCompositorContextCreateCallback, &context);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_31, status);
    EXPECT_TRUE(context.encountered);

    context = { &context, false, ISTATUS_SUCCESS };
    status = SpectrumCompositorContextCreate(
        SpectrumCompositorContextCreateCallback, &context);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(context.encountered);
}

ISTATUS
SpectrumCompositorCreateCallback(
    _Inout_opt_ void *context,
    _In_ PSPECTRUM_COMPOSITOR compositor
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
SpectrumCompositorCreateCallbackContext(
    _Inout_opt_ void *data,
    _In_ PSPECTRUM_COMPOSITOR_CONTEXT compositor_context
    )
{
    EXPECT_TRUE(!data);

    ISTATUS status = SpectrumCompositorCreate(
        nullptr, SpectrumCompositorCreateCallback, nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = SpectrumCompositorCreate(compositor_context, nullptr, nullptr);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    CallbackContext context = { &context, false, ISTATUS_INVALID_ARGUMENT_31 };
    status = SpectrumCompositorCreate(
        compositor_context, SpectrumCompositorCreateCallback, &context);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_31, status);
    EXPECT_TRUE(context.encountered);

    context = { &context, false, ISTATUS_SUCCESS };
    status = SpectrumCompositorCreate(
        compositor_context, SpectrumCompositorCreateCallback, &context);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_TRUE(context.encountered);

    return ISTATUS_SUCCESS;
}

TEST(SpectrumCompositorContext, SpectrumCompositorCreate)
{
    ISTATUS status = SpectrumCompositorContextCreate(
        SpectrumCompositorCreateCallbackContext, nullptr);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
}