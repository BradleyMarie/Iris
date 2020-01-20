/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    spectrum_test.cc

Abstract:

    Unit tests for spectrum.c

--*/

extern "C" {
#include "iris_physx/spectrum.h"
}

#include <limits>

#include "googletest/include/gtest/gtest.h"

TEST(SpectrumTest, SpectrumAllocateErrors)
{
    PSPECTRUM spectrum;
    SPECTRUM_VTABLE vtable;

    ISTATUS status = SpectrumAllocate(NULL,
                                      NULL,
                                      0,
                                      0,
                                      &spectrum);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = SpectrumAllocate(&vtable,
                              NULL,
                              0,
                              0,
                              NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_04, status);

    status = SpectrumAllocate(&vtable,
                              NULL,
                              16,
                              1,
                              &spectrum);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_00, status);

    int data = 0;
    status = SpectrumAllocate(&vtable,
                              &data,
                              sizeof(int),
                              0,
                              &spectrum);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_01, status);

    int data2[3] = { 0, 0, 0 };
    status = SpectrumAllocate(&vtable,
                              &data2,
                              sizeof(int),
                              8,
                              &spectrum);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_COMBINATION_02, status);
}

TEST(SpectrumTest, SpectrumSampleNull)
{
    float_t intensity = (float_t)1.0;
    ISTATUS status = SpectrumSample(NULL, (float_t)1.0, &intensity);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, intensity);
}

struct TestContext {
    ISTATUS return_status;
    float_t wavelength;
    float_t intensity;
    bool *encountered;
};

ISTATUS
SampleRoutine(
    _In_ const void *data,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    const TestContext *context = static_cast<const TestContext*>(data);
    EXPECT_FALSE(*context->encountered);
    *context->encountered = true;
    EXPECT_EQ(context->wavelength, wavelength);
    *intensity = context->intensity;
    return context->return_status;
}

TEST(SpectrumTest, SpectrumSampleErrors)
{
    bool encountered = false;
    TestContext context = { 
        ISTATUS_INTEGER_OVERFLOW,
        INFINITY,
        INFINITY,
        &encountered
    };

    SPECTRUM_VTABLE vtable = { SampleRoutine, NULL };
    PSPECTRUM spectrum;

    ISTATUS status = SpectrumAllocate(&vtable,
                                      &context,
                                      sizeof(TestContext),
                                      alignof(TestContext),
                                      &spectrum);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t intensity;
    status = SpectrumSample(spectrum, (float_t)0.0, &intensity);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    EXPECT_FALSE(encountered);

    status = SpectrumSample(spectrum, (float_t)-1.0, &intensity);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    EXPECT_FALSE(encountered);

    status = SpectrumSample(spectrum, -INFINITY, &intensity);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);

    status = SpectrumSample(spectrum, INFINITY, &intensity);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    EXPECT_FALSE(encountered);

    status = SpectrumSample(spectrum,
                            std::numeric_limits<float_t>::quiet_NaN(),
                            &intensity);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_01, status);
    EXPECT_FALSE(encountered);

    status = SpectrumSample(spectrum, (float_t)1.0, NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);
    EXPECT_FALSE(encountered);

    SpectrumRelease(spectrum);
}

TEST(SpectrumTest, SpectrumSample)
{
    bool encountered = false;
    TestContext context = { 
        ISTATUS_INTEGER_OVERFLOW,
        (float_t)1.0,
        (float_t)10.0,
        &encountered
    };
    
    SPECTRUM_VTABLE vtable = { SampleRoutine, NULL };
    PSPECTRUM spectrum;

    ISTATUS status = SpectrumAllocate(&vtable,
                                      &context,
                                      sizeof(TestContext),
                                      alignof(TestContext),
                                      &spectrum);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t intensity;
    status = SpectrumSample(spectrum, (float_t)1.0, &intensity);
    EXPECT_EQ(ISTATUS_INTEGER_OVERFLOW, status);
    EXPECT_EQ((float_t)10.0, intensity);
    EXPECT_TRUE(encountered);

    SpectrumRelease(spectrum);

    encountered = false;
    context = { 
        ISTATUS_SUCCESS,
        (float_t)1.0,
        (float_t)2.0,
        &encountered
    };

    status = SpectrumAllocate(&vtable,
                              &context,
                              sizeof(TestContext),
                              alignof(TestContext),
                              &spectrum);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = SpectrumSample(spectrum, (float_t)1.0, &intensity);
    EXPECT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)2.0, intensity);
    EXPECT_TRUE(encountered);

    SpectrumRelease(spectrum);
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

TEST(SpectrumTest, SpectrumFree)
{
    bool freed = false;
    SPECTRUM_VTABLE vtable = { NULL, NULL };
    FreeTestContext context = { &context, &freed };
    PSPECTRUM spectrum;

    ISTATUS status = SpectrumAllocate(&vtable,
                                      &context,
                                      sizeof(context),
                                      alignof(context),
                                      &spectrum);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    SpectrumRelease(spectrum);
    EXPECT_FALSE(freed);

    vtable.free_routine = FreeRoutine;
    status = SpectrumAllocate(&vtable,
                              &context,
                              sizeof(context),
                              alignof(context),
                              &spectrum);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    SpectrumRetain(spectrum);
    EXPECT_FALSE(freed);
    
    SpectrumRelease(spectrum);
    EXPECT_FALSE(freed);

    SpectrumRelease(spectrum);
    EXPECT_TRUE(freed);
}