/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    spectrum_compositor_test.cc

Abstract:

    Unit tests for spectrum_compositor.c

--*/

extern "C" {
#include "iris_spectrum/spectrum_compositor.h"
#include "iris_spectrum/test_util.h"
}

#include <limits>

#include "googletest/include/gtest/gtest.h"

TEST(SpectrumCompositorTest, SpectrumCompositorAddSpectraErrors)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PCSPECTRUM output;
    ISTATUS status = SpectrumCompositorAddSpectra(NULL,
                                                  NULL,
                                                  NULL,
                                                  &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = SpectrumCompositorAddSpectra(compositor,
                                          NULL,
                                          NULL,
                                          NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    SpectrumCompositorFree(compositor);
}

TEST(SpectrumCompositorTest, SpectrumCompositorAttenuateSpectrumErrors)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PCSPECTRUM output;
    ISTATUS status = SpectrumCompositorAttenuateSpectrum(NULL,
                                                         NULL,
                                                         (float_t)1.0,
                                                         &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 NULL,
                                                 (float_t)-1.0,
                                                 &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 NULL,
                                                 (float_t)INFINITY,
                                                 &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 NULL,
                                                 std::numeric_limits<float_t>::quiet_NaN(),
                                                 &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 NULL,
                                                 (float_t)1.0,
                                                 NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    SpectrumCompositorFree(compositor);
}

TEST(SpectrumCompositorTest, SpectrumCompositorAddReflectionErrors)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PCSPECTRUM output;
    ISTATUS status = SpectrumCompositorAddReflection(NULL,
                                                     NULL,
                                                     NULL,
                                                     &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = SpectrumCompositorAddReflection(compositor,
                                             NULL,
                                             NULL,
                                             NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    SpectrumCompositorFree(compositor);
}

TEST(SpectrumCompositorTest, SpectrumCompositorAttenuatedAddReflectionErrors)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PCSPECTRUM output;
    ISTATUS status = SpectrumCompositorAttenuatedAddReflection(NULL,
                                                               NULL,
                                                               NULL,
                                                               (float_t)1.0,
                                                               &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                       NULL,
                                                       NULL,
                                                       (float_t)-1.0,
                                                       &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                       NULL,
                                                       NULL,
                                                       (float_t)INFINITY,
                                                       &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                       NULL,
                                                       NULL,
                                                       std::numeric_limits<float_t>::quiet_NaN(),
                                                       &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                       NULL,
                                                       NULL,
                                                       (float_t)1.0,
                                                       NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_04, status);

    SpectrumCompositorFree(compositor);
}