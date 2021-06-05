/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    environmental_light.cc

Abstract:

    Integration test which renders an Iris Physx scene with an environmental
    light.

--*/

#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_camera_toolkit/grid_image_sampler.h"
#include "iris_camera_toolkit/pinhole_camera.h"
#include "iris_physx_toolkit/all_light_sampler.h"
#include "iris_physx_toolkit/color_spectra.h"
#include "iris_physx_toolkit/infinite_environmental_light.h"
#include "iris_physx_toolkit/kd_tree_scene.h"
#include "iris_physx_toolkit/list_scene.h"
#include "iris_physx_toolkit/mipmap.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/sample_tracer.h"
#include "googletest/include/gtest/gtest.h"
#include "test_util/pfm.h"

TEST(EnvironmentalLight, Blue)
{
    PCOLOR_EXTRAPOLATOR color_extrapolator;
    ISTATUS status = ColorColorExtrapolatorAllocate(COLOR_SPACE_XYZ,
                                                    &color_extrapolator);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PCOLOR_INTEGRATOR color_integrator;
    status = ColorColorIntegratorAllocate(COLOR_SPACE_XYZ, &color_integrator);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    float_t values[3] = { (float_t)0.0, (float_t)0.0, (float_t)1.0 }; 
    COLOR3 color = ColorCreate(COLOR_SPACE_XYZ, values);

    PSPECTRUM_MIPMAP mipmap;
    status = SpectrumMipmapAllocate(&color,
                                    1,
                                    1,
                                    TEXTURE_FILTERING_ALGORITHM_NONE,
                                    (float_t)1.0,
                                    WRAP_MODE_BLACK,
                                    color_extrapolator,
                                    &mipmap);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PENVIRONMENTAL_LIGHT environmental_light;
    PLIGHT light;
    status = InfiniteEnvironmentalLightAllocate(mipmap,
                                                NULL,
                                                color_integrator,
                                                &environmental_light,
                                                &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PCAMERA camera;
    status = PinholeCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)1.0,
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PIMAGE_SAMPLER image_sampler;
    status =
        GridImageSamplerAllocate(1, 1, false, 1, 1, false, &image_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PRANDOM rng;
    status = PermutedCongruentialRandomAllocate(
        0x853c49e6748fea9bULL,
        0xda3e39cb94b95bdbULL,
        &rng);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PINTEGRATOR path_tracer;
    status = PathTracerAllocate(8, 8, (float_t)0.0, INFINITY, &path_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape = nullptr;
    PMATRIX matrix = nullptr;
    bool premultiplied = false;

    PSCENE scene;
    status = ListSceneAllocate(&shape,
                               &matrix,
                               &premultiplied,
                               0,
                               environmental_light,
                               &scene);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT_SAMPLER light_sampler;
    status = AllLightSamplerAllocate(&light, 1, &light_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSAMPLE_TRACER sample_tracer;
    status = PhysxSampleTracerAllocate(path_tracer, &sample_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = IntegratorPrepare(path_tracer,
                               scene,
                               light_sampler,
                               color_integrator);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PFRAMEBUFFER framebuffer;
    status = FramebufferAllocate(500, 500, &framebuffer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = IrisCameraRenderSingleThreaded(camera,
                                            nullptr,
                                            image_sampler,
                                            sample_tracer,
                                            rng,
                                            framebuffer,
                                            nullptr,
                                            (float_t)0.0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    bool equals;
    status = ExactlyEqualsPfmFile(framebuffer,
                                  "test_results/blue.pfm",
                                  COLOR_SPACE_XYZ,
                                  &equals);
    ASSERT_EQ(status, ISTATUS_SUCCESS);
    EXPECT_TRUE(equals);

    ColorExtrapolatorFree(color_extrapolator);
    EnvironmentalLightRelease(environmental_light);
    LightRelease(light);
    CameraFree(camera);
    ImageSamplerFree(image_sampler);
    RandomFree(rng);
    SceneRelease(scene);
    LightSamplerRelease(light_sampler);
    SampleTracerFree(sample_tracer);
    FramebufferFree(framebuffer);
    ColorIntegratorRelease(color_integrator);
}