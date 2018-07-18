/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    spectrum_compositor_test.cc

Abstract:

    Unit tests for spectrum_compositor.c

--*/

extern "C" {
#include "iris_physx/spectrum_compositor.h"
#include "iris_physx/test_util.h"
}

#include <limits>
#include <vector>

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

const SPECTRUM_VTABLE cutoff_vtable = { CutoffSpectrumRoutine, NULL };

PSPECTRUM
CutoffSpectrumCreate(
    float_t intensity,
    float_t cutoff
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

struct ConstantContext {
    float_t intensity;
};

ISTATUS
ConstantReflectorRoutine(
    _In_ const void *data,
    _In_ float_t wavelength,
    _In_ float_t incoming_intensity,
    _Out_ float_t *outgoing_intensity
    )
{
    const ConstantContext *context = static_cast<const ConstantContext*>(data);
    *outgoing_intensity = context->intensity;
    return ISTATUS_SUCCESS;
}

const REFLECTOR_VTABLE constant_ref_vtable = { ConstantReflectorRoutine, NULL };

PREFLECTOR
ConstantReflectorCreate(
    float_t intensity
    )
{
    ConstantContext context = { intensity };

    PREFLECTOR result;
    ISTATUS status = ReflectorAllocate(&constant_ref_vtable,
                                       &context,
                                       sizeof(ConstantContext),
                                       alignof(ConstantContext),
                                       &result);

    if (status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    return result;
}

TEST(SpectrumCompositor, SpectrumCompositorAddReflection)
{
    PSPECTRUM root_spectrum0 = CutoffSpectrumCreate((float_t)4.0,
                                                    (float_t)1.5);
    ASSERT_TRUE(NULL != root_spectrum0);

    PREFLECTOR root_reflector0 = ConstantReflectorCreate((float_t)1.0);
    ASSERT_TRUE(NULL != root_reflector0);

    PREFLECTOR root_reflector1 = ConstantReflectorCreate((float_t)2.0);
    ASSERT_TRUE(NULL != root_reflector1);

    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    std::vector<PCREFLECTOR> reflectors = { root_reflector0, 
                                            root_reflector1,
                                            nullptr };

    std::vector<float_t> reflector_answer = { (float_t)1.0, 
                                              (float_t)2.0,
                                              (float_t)0.0 };

    std::vector<PCSPECTRUM> spectra = { root_spectrum0 };

    std::vector<float_t> answer_wavelength_one = { (float_t)4.0 };

    std::vector<float_t> answer_wavelength_two = { (float_t)0.0 };
    
    for (size_t i = 0; i < reflectors.size(); i++)
    {
        PCSPECTRUM result;
        ISTATUS status = SpectrumCompositorAddReflection(compositor,
                                                         root_spectrum0,
                                                         reflectors[i],
                                                         &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i]);
        answer_wavelength_two.push_back(reflector_answer[i]);
    }
    
    for (size_t i = 0; i < reflectors.size(); i++)
    {
        PCSPECTRUM result;
        ISTATUS status = SpectrumCompositorAddReflection(compositor,
                                                         nullptr,
                                                         reflectors[i],
                                                         &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back((float_t)0.0);
        answer_wavelength_two.push_back((float_t)0.0);
    }

    for (size_t i = 0; i < spectra.size(); i++)
    {
        float_t value;
        ISTATUS status = SpectrumSample(spectra[i], (float_t)1.0, &value);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(answer_wavelength_one[i], value);
        
        status = SpectrumSample(spectra[i], (float_t)2.0, &value);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(answer_wavelength_two[i], value);
    }

    SpectrumCompositorFree(compositor);

    SpectrumRelease(root_spectrum0);
    ReflectorRelease(root_reflector0);
    ReflectorRelease(root_reflector1);
}

TEST(SpectrumCompositor, SpectrumCompositorAttenuatedAddReflection)
{
    PSPECTRUM root_spectrum0 = CutoffSpectrumCreate((float_t)4.0,
                                                    (float_t)1.5);
    ASSERT_TRUE(NULL != root_spectrum0);

    PREFLECTOR root_reflector0 = ConstantReflectorCreate((float_t)1.0);
    ASSERT_TRUE(NULL != root_reflector0);

    PREFLECTOR root_reflector1 = ConstantReflectorCreate((float_t)2.0);
    ASSERT_TRUE(NULL != root_reflector1);

    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    std::vector<PCREFLECTOR> reflectors = { root_reflector0, 
                                            root_reflector1,
                                            nullptr };

    std::vector<float_t> reflector_answer = { (float_t)1.0, 
                                              (float_t)2.0,
                                              (float_t)0.0 };

    std::vector<PCSPECTRUM> spectra = { root_spectrum0 };

    std::vector<float_t> answer_wavelength_one = { (float_t)4.0 };

    std::vector<float_t> answer_wavelength_two = { (float_t)0.0 };
    
    for (size_t i = 0; i < reflectors.size(); i++)
    {
        PCSPECTRUM result;
        ISTATUS status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                                   root_spectrum0,
                                                                   reflectors[i],
                                                                   (float_t)0.0,
                                                                   &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)0.0);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)0.0);

        status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                           root_spectrum0,
                                                           reflectors[i],
                                                           (float_t)0.5,
                                                           &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)0.5);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)0.5);

        status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                           root_spectrum0,
                                                           reflectors[i],
                                                           (float_t)1.0,
                                                           &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)1.0);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)1.0);

        status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                           root_spectrum0,
                                                           reflectors[i],
                                                           (float_t)2.0,
                                                           &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)2.0);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)2.0);
    }
    
    for (size_t i = 0; i < reflectors.size(); i++)
    {
        PCSPECTRUM result;
        ISTATUS status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                                   nullptr,
                                                                   reflectors[i],
                                                                   (float_t)0.0,
                                                                   &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)0.0);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)0.0);

        status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                           nullptr,
                                                           reflectors[i],
                                                           (float_t)0.5,
                                                           &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)0.0);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)0.0);

        status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                           nullptr,
                                                           reflectors[i],
                                                           (float_t)1.0,
                                                           &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)0.0);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)0.0);

        status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                           nullptr,
                                                           reflectors[i],
                                                           (float_t)2.0,
                                                           &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)0.0);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)0.0);
    }

    for (size_t i = 0; i < spectra.size(); i++)
    {
        float_t value;
        ISTATUS status = SpectrumSample(spectra[i], (float_t)1.0, &value);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(answer_wavelength_one[i], value);
        
        status = SpectrumSample(spectra[i], (float_t)2.0, &value);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(answer_wavelength_two[i], value);
    }

    SpectrumCompositorFree(compositor);

    SpectrumRelease(root_spectrum0);
    ReflectorRelease(root_reflector0);
    ReflectorRelease(root_reflector1);
}

TEST(SpectrumCompositor, SpectrumCompositorAttenuateSpectrum)
{
    PSPECTRUM root_spectrum0 = CutoffSpectrumCreate((float_t)4.0,
                                                    (float_t)1.5);
    ASSERT_TRUE(NULL != root_spectrum0);

    PSPECTRUM root_spectrum1 = CutoffSpectrumCreate((float_t)8.0,
                                                    (float_t)1.5);
    ASSERT_TRUE(NULL != root_spectrum1);

    PREFLECTOR root_reflector0 = ConstantReflectorCreate((float_t)1.0);
    ASSERT_TRUE(NULL != root_reflector0);

    PREFLECTOR root_reflector1 = ConstantReflectorCreate((float_t)2.0);
    ASSERT_TRUE(NULL != root_reflector1);

    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    std::vector<PCREFLECTOR> reflectors = { root_reflector0, 
                                            root_reflector1,
                                            nullptr };

    std::vector<float_t> reflector_answer = { (float_t)1.0, 
                                              (float_t)2.0,
                                              (float_t)0.0 };

    std::vector<PCSPECTRUM> spectra = { nullptr,
                                        root_spectrum0,
                                        root_spectrum1 };

    std::vector<float_t> answer_wavelength_one = { (float_t)0.0,
                                                   (float_t)4.0,
                                                   (float_t)8.0 };

    std::vector<float_t> answer_wavelength_two = { (float_t)0.0,
                                                   (float_t)0.0,
                                                   (float_t)0.0 };

    for (size_t i = 0; i < reflectors.size(); i++)
    {
        PCSPECTRUM result;
        ISTATUS status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                                   root_spectrum0,
                                                                   reflectors[i],
                                                                   (float_t)0.0,
                                                                   &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)0.0);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)0.0);

        status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                           root_spectrum0,
                                                           reflectors[i],
                                                           (float_t)0.5,
                                                           &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)0.5);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)0.5);

        status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                           root_spectrum0,
                                                           reflectors[i],
                                                           (float_t)1.0,
                                                           &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)1.0);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)1.0);

        status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                           root_spectrum0,
                                                           reflectors[i],
                                                           (float_t)2.0,
                                                           &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)2.0);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)2.0);
    }

    size_t stop_value = spectra.size();

    for (size_t i = 0; i < stop_value; i++)
    {
        PCSPECTRUM result;
        ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                             spectra[i],
                                                             (float_t)0.0,
                                                             &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(answer_wavelength_one[i] * (float_t)0.0);
        answer_wavelength_two.push_back(answer_wavelength_two[i] * (float_t)0.0);

        status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                     spectra[i],
                                                     (float_t)0.5,
                                                     &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(answer_wavelength_one[i] * (float_t)0.5);
        answer_wavelength_two.push_back(answer_wavelength_two[i] * (float_t)0.5);

        status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                     spectra[i],
                                                     (float_t)1.0,
                                                     &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(answer_wavelength_one[i] * (float_t)1.0);
        answer_wavelength_two.push_back(answer_wavelength_two[i] * (float_t)1.0);

        status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                     spectra[i],
                                                     (float_t)2.0,
                                                     &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(answer_wavelength_one[i] * (float_t)2.0);
        answer_wavelength_two.push_back(answer_wavelength_two[i] * (float_t)2.0);
    }

    stop_value = spectra.size();

    for (size_t i = 0; i < stop_value; i++)
    {
        PCSPECTRUM result;
        ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                             spectra[i],
                                                             (float_t)0.0,
                                                             &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(answer_wavelength_one[i] * (float_t)0.0);
        answer_wavelength_two.push_back(answer_wavelength_two[i] * (float_t)0.0);

        status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                     spectra[i],
                                                     (float_t)0.5,
                                                     &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(answer_wavelength_one[i] * (float_t)0.5);
        answer_wavelength_two.push_back(answer_wavelength_two[i] * (float_t)0.5);

        status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                     spectra[i],
                                                     (float_t)1.0,
                                                     &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(answer_wavelength_one[i] * (float_t)1.0);
        answer_wavelength_two.push_back(answer_wavelength_two[i] * (float_t)1.0);

        status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                     spectra[i],
                                                     (float_t)2.0,
                                                     &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(answer_wavelength_one[i] * (float_t)2.0);
        answer_wavelength_two.push_back(answer_wavelength_two[i] * (float_t)2.0);
    }

    for (size_t i = 0; i < spectra.size(); i++)
    {
        float_t value;
        ISTATUS status = SpectrumSample(spectra[i], (float_t)1.0, &value);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(answer_wavelength_one[i], value);
        
        status = SpectrumSample(spectra[i], (float_t)2.0, &value);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(answer_wavelength_two[i], value);
    }

    SpectrumCompositorFree(compositor);

    SpectrumRelease(root_spectrum0);
    SpectrumRelease(root_spectrum1);
    ReflectorRelease(root_reflector0);
    ReflectorRelease(root_reflector1);
}

TEST(SpectrumCompositor, SpectrumCompositorAddSpectra)
{
    PSPECTRUM root_spectrum0 = CutoffSpectrumCreate((float_t)4.0,
                                                    (float_t)1.5);
    ASSERT_TRUE(NULL != root_spectrum0);

    PSPECTRUM root_spectrum1 = CutoffSpectrumCreate((float_t)8.0,
                                                    (float_t)1.5);
    ASSERT_TRUE(NULL != root_spectrum1);

    PREFLECTOR root_reflector0 = ConstantReflectorCreate((float_t)1.0);
    ASSERT_TRUE(NULL != root_reflector0);

    PREFLECTOR root_reflector1 = ConstantReflectorCreate((float_t)2.0);
    ASSERT_TRUE(NULL != root_reflector1);

    PSPECTRUM_COMPOSITOR compositor = SpectrumCompositorAllocate();
    ASSERT_TRUE(compositor != NULL);

    std::vector<PCREFLECTOR> reflectors = { root_reflector0, 
                                            root_reflector1 };

    std::vector<float_t> reflector_answer = { (float_t)1.0, 
                                              (float_t)2.0,
                                              (float_t)0.0 };

    std::vector<PCSPECTRUM> spectra = { nullptr,
                                        root_spectrum0,
                                        root_spectrum1 };

    std::vector<float_t> answer_wavelength_one = { (float_t)0.0,
                                                   (float_t)4.0,
                                                   (float_t)8.0 };

    std::vector<float_t> answer_wavelength_two = { (float_t)0.0,
                                                   (float_t)0.0,
                                                   (float_t)0.0 };

    for (size_t i = 0; i < reflectors.size(); i++)
    {
        PCSPECTRUM result;
        ISTATUS status = SpectrumCompositorAddReflection(compositor,
                                                         root_spectrum0,
                                                         reflectors[i],
                                                         &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i]);
        answer_wavelength_two.push_back(reflector_answer[i]);
    }

    for (size_t i = 0; i < reflectors.size(); i++)
    {
        PCSPECTRUM result;
        ISTATUS status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                                   root_spectrum0,
                                                                   reflectors[i],
                                                                   (float_t)0.5,
                                                                   &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)0.5);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)0.5);

        status = SpectrumCompositorAttenuatedAddReflection(compositor,
                                                           root_spectrum0,
                                                           reflectors[i],
                                                           (float_t)2.0,
                                                           &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(reflector_answer[i] * (float_t)2.0);
        answer_wavelength_two.push_back(reflector_answer[i] * (float_t)2.0);
    }

    size_t stop_value = spectra.size();

    for (size_t i = 0; i < stop_value; i++)
    {
        PCSPECTRUM result;
        ISTATUS status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                             spectra[i],
                                                             (float_t)0.5,
                                                             &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(answer_wavelength_one[i] * (float_t)0.5);
        answer_wavelength_two.push_back(answer_wavelength_two[i] * (float_t)0.5);

        status = SpectrumCompositorAttenuateSpectrum(compositor,
                                                     spectra[i],
                                                     (float_t)2.0,
                                                     &result);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        spectra.push_back(result);

        answer_wavelength_one.push_back(answer_wavelength_one[i] * (float_t)2.0);
        answer_wavelength_two.push_back(answer_wavelength_two[i] * (float_t)2.0);
    }

    stop_value = spectra.size();

    for (size_t i = 0; i < stop_value; i++)
    {
        for (size_t j = 0; j < stop_value; j++)
        {
            PCSPECTRUM result;
            ISTATUS status = SpectrumCompositorAddSpectra(compositor,
                                                          spectra[i],
                                                          spectra[j],
                                                          &result);
            ASSERT_EQ(ISTATUS_SUCCESS, status);
            spectra.push_back(result);

            answer_wavelength_one.push_back(answer_wavelength_one[i] + answer_wavelength_one[j]);
            answer_wavelength_two.push_back(answer_wavelength_two[i] + answer_wavelength_two[j]);
        }
    }

    for (size_t i = 0; i < stop_value; i++)
    {
        for (size_t j = 0; j < stop_value; j++)
        {
            PCSPECTRUM result;
            ISTATUS status = SpectrumCompositorAddSpectra(compositor,
                                                          spectra[i],
                                                          spectra[j],
                                                          &result);
            ASSERT_EQ(ISTATUS_SUCCESS, status);
            spectra.push_back(result);

            answer_wavelength_one.push_back(answer_wavelength_one[i] + answer_wavelength_one[j]);
            answer_wavelength_two.push_back(answer_wavelength_two[i] + answer_wavelength_two[j]);
        }
    }

    for (size_t i = 0; i < spectra.size(); i++)
    {
        float_t value;
        ISTATUS status = SpectrumSample(spectra[i], (float_t)1.0, &value);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(answer_wavelength_one[i], value);
        
        status = SpectrumSample(spectra[i], (float_t)2.0, &value);
        ASSERT_EQ(ISTATUS_SUCCESS, status);
        EXPECT_EQ(answer_wavelength_two[i], value);
    }
    
    SpectrumCompositorFree(compositor);

    SpectrumRelease(root_spectrum0);
    SpectrumRelease(root_spectrum1);
    ReflectorRelease(root_reflector0);
    ReflectorRelease(root_reflector1);
}