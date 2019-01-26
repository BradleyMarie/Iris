/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    reflector_compositor_test.cc

Abstract:

    Unit tests for reflector_compositor.c

--*/

extern "C" {
#include "iris_physx/reflector_compositor.h"
#include "iris_physx/reflector_compositor_test_util.h"
}

#include <limits>
#include <vector>

#include "googletest/include/gtest/gtest.h"

TEST(ReflectorCompositorTest, ReflectorCompositorAttenuateReflectorErrors)
{
    PREFLECTOR_COMPOSITOR compositor = ReflectorCompositorCreate();
    ASSERT_TRUE(compositor != NULL);

    PREFLECTOR reflector = (PREFLECTOR)(void*)(uintptr_t)1;

    PCREFLECTOR output;
    ISTATUS status = ReflectorCompositorAttenuateReflector(NULL,
                                                           reflector,
                                                           2.0,
                                                           &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   reflector,
                                                   -1.0,
                                                   &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   reflector,
                                                   INFINITY,
                                                   &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   reflector,
                                                   -INFINITY,
                                                   &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   reflector,
                                                   std::numeric_limits<float_t>::quiet_NaN(),
                                                   &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_02, status);

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   reflector,
                                                   2.0,
                                                   NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    ReflectorCompositorFree(compositor);
}

TEST(ReflectorCompositorTest, ReflectorCompositorAttenuatedAddReflectorsErrors)
{
    PREFLECTOR_COMPOSITOR compositor = ReflectorCompositorCreate();
    ASSERT_TRUE(compositor != NULL);

    PREFLECTOR reflector = (PREFLECTOR)(void*)(uintptr_t)1;

    PCREFLECTOR output;
    ISTATUS status = ReflectorCompositorAttenuatedAddReflectors(NULL,
                                                                reflector,
                                                                reflector,
                                                                2.0,
                                                                &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_00, status);

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        reflector,
                                                        reflector,
                                                        -1.0,
                                                        &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        reflector,
                                                        reflector,
                                                        INFINITY,
                                                        &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        reflector,
                                                        reflector,
                                                        -INFINITY,
                                                        &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        reflector,
                                                        reflector,
                                                        std::numeric_limits<float_t>::quiet_NaN(),
                                                        &output);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_03, status);

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        reflector,
                                                        reflector,
                                                        2.0,
                                                        NULL);
    EXPECT_EQ(ISTATUS_INVALID_ARGUMENT_04, status);

    ReflectorCompositorFree(compositor);
}

struct CutoffContext {
    float_t cutoff;
    float_t reflectance;
};

ISTATUS
CutoffReflectorRoutine(
    _In_ const void *data,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    )
{
    const CutoffContext *context = static_cast<const CutoffContext*>(data);
    if (wavelength > context->cutoff)
    {
        *reflectance = (float_t)0.0;
    }
    else
    {
        *reflectance = context->reflectance;
    }

    return ISTATUS_SUCCESS;
}

const REFLECTOR_VTABLE cutoff_vtable = {
    CutoffReflectorRoutine,
    NULL
};

PREFLECTOR
CutoffReflectorCreate(
    _In_ float_t reflectance,
    _In_ float_t cutoff
    )
{
    CutoffContext context = { cutoff, reflectance };

    PREFLECTOR result;
    ISTATUS status = ReflectorAllocate(&cutoff_vtable,
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

TEST(ReflectorCompositor, ReflectorCompositorAttenuatedAddReflectors)
{
    PREFLECTOR_COMPOSITOR compositor = ReflectorCompositorCreate();
    ASSERT_TRUE(compositor != NULL);

    PREFLECTOR root_reflector0 = CutoffReflectorCreate((float_t)0.5,
                                                       (float_t)1.5);
    ASSERT_TRUE(NULL != root_reflector0);

    PREFLECTOR root_reflector1 = AttenuatingReflectorCreate((float_t)0.5);
    ASSERT_TRUE(NULL != root_reflector1);

    PCREFLECTOR result;
    ISTATUS status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                                NULL,
                                                                NULL,
                                                                (float_t)0.5,
                                                                &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(NULL, result);

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        NULL,
                                                        root_reflector1,
                                                        (float_t)0.0,
                                                        &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(NULL, result);

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        root_reflector0,
                                                        NULL,
                                                        (float_t)0.5,
                                                        &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(root_reflector0, result);

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        root_reflector0,
                                                        root_reflector1,
                                                        (float_t)0.0,
                                                        &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(root_reflector0, result);

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        NULL,
                                                        root_reflector1,
                                                        (float_t)1.0,
                                                        &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(root_reflector1, result);

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        root_reflector0,
                                                        root_reflector1,
                                                        (float_t)0.5,
                                                        &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t value;
    status = ReflectorReflect(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.75, value);

    status = ReflectorReflect(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.25, value);

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        NULL,
                                                        root_reflector0,
                                                        (float_t)0.5,
                                                        &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = ReflectorReflect(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.25, value);

    status = ReflectorReflect(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, value);

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   root_reflector1,
                                                   (float_t)0.5,
                                                   &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        root_reflector0,
                                                        result,
                                                        (float_t)0.5,
                                                        &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = ReflectorReflect(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.625, value);

    status = ReflectorReflect(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.125, value);

    ReflectorRelease(root_reflector0);
    ReflectorRelease(root_reflector0);

    ReflectorCompositorFree(compositor);
}

TEST(ReflectorCompositor, ReflectorCompositorAttenuateReflector)
{
    PREFLECTOR_COMPOSITOR compositor = ReflectorCompositorCreate();
    ASSERT_TRUE(compositor != NULL);

    PREFLECTOR root_reflector0 = CutoffReflectorCreate((float_t)1.0,
                                                       (float_t)1.5);
    ASSERT_TRUE(NULL != root_reflector0);

    PCREFLECTOR result;
    ISTATUS status = ReflectorCompositorAttenuateReflector(compositor,
                                                           NULL,
                                                           (float_t)0.5,
                                                           &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(NULL, result);

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   root_reflector0,
                                                   (float_t)0.0,
                                                   &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    ASSERT_EQ(NULL, result);

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   root_reflector0,
                                                   (float_t)0.5,
                                                   &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    float_t value;
    status = ReflectorReflect(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.5, value);

    status = ReflectorReflect(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, value);

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   result,
                                                   (float_t)0.5,
                                                   &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);

    status = ReflectorReflect(result, (float_t)1.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.25, value);

    status = ReflectorReflect(result, (float_t)2.0, &value);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ((float_t)0.0, value);

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   result,
                                                   (float_t)4.0,
                                                   &result);
    ASSERT_EQ(ISTATUS_SUCCESS, status);
    EXPECT_EQ(root_reflector0, result);

    ReflectorRelease(root_reflector0);
    ReflectorRelease(root_reflector0);

    ReflectorCompositorFree(compositor);
}