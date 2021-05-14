/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    spectrum_compositor_test.cc

Abstract:

    Unit tests for spectrum_compositor.c

--*/

extern "C" {
#include "iris_physx/reflector_compositor.h"
#include "iris_physx/reflector_compositor_test_util.h"
#include "iris_physx/spectrum_compositor.h"
#include "iris_physx/spectrum_compositor_test_util.h"
}

#include <limits>
#include <vector>

#include "googletest/include/gtest/gtest.h"

TEST(SpectrumCompositorTest, SpectrumCompositorAddSpectraErrors)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PSPECTRUM spectrum0 = (PSPECTRUM)(void*)(uintptr_t)1;
    PSPECTRUM spectrum1 = (PSPECTRUM)(void*)(uintptr_t)2;

    PCSPECTRUM output;
    ISTATUS status = SpectrumCompositorAddSpectra(NULL,
                                                  spectrum0,
                                                  spectrum1,
                                                  &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = SpectrumCompositorAddSpectra(compositor,
                                          spectrum0,
                                          spectrum1,
                                          NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    SpectrumCompositorFree(compositor);
}

TEST(SpectrumCompositorTest, SpectrumCompositorAttenateSpectrumErrors)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PSPECTRUM spectrum = (PSPECTRUM)(void*)(uintptr_t)1;

    PCSPECTRUM output;
    ISTATUS status = SpectrumCompositorAttenuateSpectrum(NULL,
                                                         spectrum,
                                                         2.0,
                                                         &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 spectrum,
                                                 -1.0,
                                                 &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 spectrum,
                                                 INFINITY,
                                                 &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 spectrum,
                                                 -INFINITY,
                                                 &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 spectrum,
                                                 std::numeric_limits<float_t>::quiet_NaN(),
                                                 &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 spectrum,
                                                 2.0,
                                                 NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    SpectrumCompositorFree(compositor);
}

TEST(SpectrumCompositorTest, SpectrumCompositorAttenatedAddSpectraErrors)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PSPECTRUM spectrum = (PSPECTRUM)(void*)(uintptr_t)1;

    PCSPECTRUM output;
    ISTATUS status = SpectrumCompositorAttenuatedAddSpectra(NULL,
                                                            spectrum,
                                                            spectrum,
                                                            2.0,
                                                            &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    spectrum,
                                                    spectrum,
                                                    -1.0,
                                                    &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    spectrum,
                                                    spectrum,
                                                    INFINITY,
                                                    &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    spectrum,
                                                    spectrum,
                                                    -INFINITY,
                                                    &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    spectrum,
                                                    spectrum,
                                                    std::numeric_limits<float_t>::quiet_NaN(),
                                                    &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    spectrum,
                                                    spectrum,
                                                    2.0,
                                                    NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_04, status);

    SpectrumCompositorFree(compositor);
}

TEST(SpectrumCompositorTest, SpectrumCompositorAttenuateReflectionErrors)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PSPECTRUM spectrum = (PSPECTRUM)(void*)(uintptr_t)1;
    PREFLECTOR reflector = (PREFLECTOR)(void*)(uintptr_t)2;

    PCSPECTRUM output;
    ISTATUS status = SpectrumCompositorAttenuateReflection(NULL,
                                                           spectrum,
                                                           reflector,
                                                           2.0,
                                                           &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = SpectrumCompositorAttenuateReflection(compositor,
                                                   spectrum,
                                                   reflector,
                                                   -1.0,
                                                   &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = SpectrumCompositorAttenuateReflection(compositor,
                                                   spectrum,
                                                   reflector,
                                                   INFINITY,
                                                   &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = SpectrumCompositorAttenuateReflection(compositor,
                                                   spectrum,
                                                   reflector,
                                                   -INFINITY,
                                                   &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = SpectrumCompositorAttenuateReflection(compositor,
                                                   spectrum,
                                                   reflector,
                                                   std::numeric_limits<float_t>::quiet_NaN(),
                                                   &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = SpectrumCompositorAttenuateReflection(compositor,
                                                   spectrum,
                                                   reflector,
                                                   2.0,
                                                   NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_04, status);

    SpectrumCompositorFree(compositor);
}

TEST(SpectrumCompositorTest, SpectrumCompositorReflectErrors)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PSPECTRUM spectrum = (PSPECTRUM)(void*)(uintptr_t)1;
    PREFLECTOR reflector = (PREFLECTOR)(void*)(uintptr_t)2;

    PCSPECTRUM output;
    ISTATUS status = SpectrumCompositorReflect(NULL,
                                               spectrum,
                                               reflector,
                                               &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = SpectrumCompositorReflect(compositor,
                                       spectrum,
                                       reflector,
                                       NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    SpectrumCompositorFree(compositor);
}

struct CutoffContext {
    float_t cutoff;
    float_t intensity;
};

ISTATUS
CutoffSpectrumRoutine(
    _In_ const void *data,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    )
{
    const CutoffContext *context = static_cast<const CutoffContext*>(data);
    if (wavelength > context->cutoff)
    {
        *intensity = (float_t)0.0;
    }
    else
    {
        *intensity = context->intensity;
    }

    return ISTATUS_SUCCESS;
}

const SPECTRUM_VTABLE cutoff_vtable = {
    CutoffSpectrumRoutine,
    NULL
};

PSPECTRUM
CutoffSpectrumCreate(
    _In_ float_t intensity,
    _In_ float_t cutoff
    )
{
    CutoffContext context = { cutoff, intensity };

    PSPECTRUM result;
    ISTATUS status = SpectrumAllocate(&cutoff_vtable,
                                      &context,
                                      sizeof(CutoffContext),
                                      alignof(CutoffContext),
                                      &result);

    if (status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    return result;
}

TEST(SpectrumCompositor, SpectrumCompositorAddSpectra)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PSPECTRUM root_spectrum0 = CutoffSpectrumCreate((float_t)8.0,
                                                    (float_t)1.5);
    ASSERT_TRUE(NULL != root_spectrum0);

    PSPECTRUM root_spectrum1 = CutoffSpectrumCreate((float_t)4.0,
                                                    (float_t)2.5);
    ASSERT_TRUE(NULL != root_spectrum1);

    PCSPECTRUM result;
    ISTATUS status = SpectrumCompositorAddSpectra(compositor,
                                                  root_spectrum0,
                                                  NULL,
                                                  &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(root_spectrum0, result);
    status = SpectrumCompositorAddSpectra(compositor,
                                          NULL,
                                          root_spectrum0,
                                          &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(root_spectrum0, result);

    status = SpectrumCompositorAddSpectra(compositor,
                                          root_spectrum0,
                                          root_spectrum1,
                                          &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t value;
    status = SpectrumSample(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)12.0, value);

    status = SpectrumSample(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)4.0, value);

    SpectrumRelease(root_spectrum0);
    SpectrumRelease(root_spectrum1);

    SpectrumCompositorFree(compositor);
}

struct AttenuatingContext {
    float_t attenuation;
};

ISTATUS
AttenuatingReflectorRoutine(
    _In_ const void *data,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    )
{
    const AttenuatingContext *context =
        static_cast<const AttenuatingContext*>(data);
    *reflectance = context->attenuation;
    return ISTATUS_SUCCESS;
}

const REFLECTOR_VTABLE attenuating_ref_vtable = {
    AttenuatingReflectorRoutine,
    NULL
};

PREFLECTOR
AttenuatingReflectorCreate(
    _In_ float_t attenuation
    )
{
    AttenuatingContext context = { attenuation };

    PREFLECTOR result;
    ISTATUS status = ReflectorAllocate(&attenuating_ref_vtable,
                                       &context,
                                       sizeof(AttenuatingContext),
                                       alignof(AttenuatingContext),
                                       &result);

    if (status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    return result;
}

TEST(SpectrumCompositor, SpectrumCompositorAttenuateReflection)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PSPECTRUM root_spectrum0 = CutoffSpectrumCreate((float_t)8.0,
                                                    (float_t)1.5);
    ASSERT_TRUE(NULL != root_spectrum0);

    PREFLECTOR root_reflector0 = AttenuatingReflectorCreate((float_t)0.5);
    ASSERT_TRUE(NULL != root_reflector0);

    PCSPECTRUM result;
    ISTATUS status = SpectrumCompositorAttenuateReflection(compositor,
                                                           NULL,
                                                           root_reflector0,
                                                           (float_t)0.5,
                                                           &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(NULL, result);

    status = SpectrumCompositorAttenuateReflection(compositor,
                                                   root_spectrum0,
                                                   NULL,
                                                   (float_t)0.5,
                                                   &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(NULL, result);

    status = SpectrumCompositorAttenuateReflection(compositor,
                                                   root_spectrum0,
                                                   root_reflector0,
                                                   (float_t)0.0,
                                                   &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(NULL, result);

    status = SpectrumCompositorAttenuateReflection(compositor,
                                                   root_spectrum0,
                                                   root_reflector0,
                                                   (float_t)0.5,
                                                   &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t value;
    status = SpectrumSample(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)2.0, value);

    status = SpectrumSample(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, value);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 root_spectrum0,
                                                 (float_t)0.5,
                                                 &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = SpectrumCompositorAttenuateReflection(compositor,
                                                   result,
                                                   root_reflector0,
                                                   (float_t)0.5,
                                                   &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = SpectrumSample(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, value);

    status = SpectrumSample(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, value);

    SpectrumRelease(root_spectrum0);
    ReflectorRelease(root_reflector0);

    SpectrumCompositorFree(compositor);
}

TEST(SpectrumCompositor, SpectrumCompositorAttenuateSpectrum)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PREFLECTOR root_reflector0 = AttenuatingReflectorCreate((float_t)0.5);
    ASSERT_TRUE(NULL != root_reflector0);

    PSPECTRUM root_spectrum0 = CutoffSpectrumCreate((float_t)16.0,
                                                    (float_t)1.5);
    ASSERT_TRUE(NULL != root_spectrum0);

    PCSPECTRUM result;
    ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                         NULL,
                                                         (float_t)0.5,
                                                         &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(NULL, result);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 root_spectrum0,
                                                 (float_t)0.0,
                                                 &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(NULL, result);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 root_spectrum0,
                                                 (float_t)0.5,
                                                 &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t value;
    status = SpectrumSample(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)8.0, value);

    status = SpectrumSample(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, value);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 result,
                                                 (float_t)0.5,
                                                 &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = SpectrumSample(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)4.0, value);

    status = SpectrumSample(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, value);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 result,
                                                 (float_t)4.0,
                                                 &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(root_spectrum0, result);

    status = SpectrumCompositorAttenuateReflection(compositor,
                                                   root_spectrum0,
                                                   root_reflector0,
                                                   (float_t)0.5,
                                                   &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 result,
                                                 (float_t)0.5,
                                                 &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = SpectrumSample(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)2.0, value);

    status = SpectrumSample(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, value);

    SpectrumRelease(root_spectrum0);
    ReflectorRelease(root_reflector0);

    SpectrumCompositorFree(compositor);
}

TEST(SpectrumCompositor, SpectrumCompositorAttenuatedAddSpectra)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PSPECTRUM root_spectrum0 = CutoffSpectrumCreate((float_t)1.0,
                                                    (float_t)1.5);
    ASSERT_TRUE(NULL != root_spectrum0);

    PSPECTRUM root_spectrum1 = CutoffSpectrumCreate((float_t)2.0,
                                                    (float_t)2.5);
    ASSERT_TRUE(NULL != root_spectrum1);

    PCSPECTRUM result;
    ISTATUS status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                            NULL,
                                                            NULL,
                                                            (float_t)0.5,
                                                            &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(NULL, result);

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    NULL,
                                                    root_spectrum1,
                                                    (float_t)0.0,
                                                    &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(NULL, result);

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    root_spectrum0,
                                                    NULL,
                                                    (float_t)0.5,
                                                    &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(root_spectrum0, result);

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    root_spectrum0,
                                                    root_spectrum1,
                                                    (float_t)0.0,
                                                    &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(root_spectrum0, result);

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    NULL,
                                                    root_spectrum1,
                                                    (float_t)1.0,
                                                    &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(root_spectrum1, result);

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    root_spectrum0,
                                                    root_spectrum1,
                                                    (float_t)0.5,
                                                    &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t value;
    status = SpectrumSample(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)2.0, value);

    status = SpectrumSample(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, value);

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    NULL,
                                                    root_spectrum0,
                                                    (float_t)0.5,
                                                    &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = SpectrumSample(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.5, value);

    status = SpectrumSample(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, value);

    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 root_spectrum1,
                                                 (float_t)0.5,
                                                 &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = SpectrumCompositorAttenuatedAddSpectra(compositor,
                                                    root_spectrum0,
                                                    result,
                                                    (float_t)0.5,
                                                    &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = SpectrumSample(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.5, value);

    status = SpectrumSample(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.5, value);

    SpectrumRelease(root_spectrum0);
    SpectrumRelease(root_spectrum1);

    SpectrumCompositorFree(compositor);
}

TEST(SpectrumCompositor, SpectrumCompositorReflect)
{
    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PSPECTRUM root_spectrum0 = CutoffSpectrumCreate((float_t)8.0,
                                                    (float_t)1.5);
    ASSERT_TRUE(NULL != root_spectrum0);

    PREFLECTOR root_reflector0 = AttenuatingReflectorCreate((float_t)0.5);
    ASSERT_TRUE(NULL != root_reflector0);

    PCSPECTRUM result;
    ISTATUS status = SpectrumCompositorReflect(compositor,
                                               NULL,
                                               root_reflector0,
                                               &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(NULL, result);

    status = SpectrumCompositorReflect(compositor,
                                       root_spectrum0,
                                       NULL,
                                       &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(NULL, result);

    status = SpectrumCompositorReflect(compositor,
                                       root_spectrum0,
                                       root_reflector0,
                                       &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t value;
    status = SpectrumSample(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)4.0, value);

    SpectrumRelease(root_spectrum0);
    ReflectorRelease(root_reflector0);

    SpectrumCompositorFree(compositor);
}

TEST(SpectrumCompositor, SpectrumCompositorAttenuateCompositedReflectionSpectrum)
{
    PREFLECTOR_COMPOSITOR reflector_compositor = ReflectorCompositorCreate();
    ASSERT_TRUE(reflector_compositor != NULL);

    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    PREFLECTOR root_reflector0 = AttenuatingReflectorCreate((float_t)0.5);
    ASSERT_TRUE(NULL != root_reflector0);

    PSPECTRUM root_spectrum0 = CutoffSpectrumCreate((float_t)16.0,
                                                    (float_t)1.5);
    ASSERT_TRUE(NULL != root_spectrum0);

    PCREFLECTOR attenuated_reflector;
    ISTATUS status = ReflectorCompositorAttenuateReflector(reflector_compositor,
                                                           root_reflector0,
                                                           (float_t)0.5,
                                                           &attenuated_reflector);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PCSPECTRUM attrenuated_spectrum;
    status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                 root_spectrum0,
                                                 (float_t)0.5,
                                                 &attrenuated_spectrum);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    PCSPECTRUM result;
    status = SpectrumCompositorAttenuateReflection(compositor,
                                                   root_spectrum0,
                                                   attenuated_reflector,
                                                   (float_t)0.5,
                                                   &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t value;
    status = SpectrumSample(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)2.0, value);

    status = SpectrumSample(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, value);

    status = SpectrumCompositorAttenuateReflection(compositor,
                                                   root_spectrum0,
                                                   attenuated_reflector,
                                                   (float_t)1.0,
                                                   &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = SpectrumSample(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)4.0, value);

    status = SpectrumSample(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, value);

    status = SpectrumCompositorAttenuateReflection(compositor,
                                                   attrenuated_spectrum,
                                                   attenuated_reflector,
                                                   (float_t)0.5,
                                                   &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = SpectrumSample(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)1.0, value);

    status = SpectrumSample(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, value);

    SpectrumRelease(root_spectrum0);
    ReflectorRelease(root_reflector0);

    SpectrumCompositorFree(compositor);
    ReflectorCompositorFree(reflector_compositor);
}