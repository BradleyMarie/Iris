/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    empty_physx.cc

Abstract:

    Integration test which renders an empty Iris Physx scene.

--*/

#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_camera_toolkit/grid_pixel_sampler.h"
#include "iris_camera_toolkit/pinhole_camera.h"
#include "iris_physx_camera_toolkit/physx_sample_tracer.h"
#include "iris_physx_toolkit/all_light_sampler.h"
#include "iris_physx_toolkit/list_scene.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "googletest/include/gtest/gtest.h"
#include "test/spectra.h"

TEST(EmptyPhysx, ListScene)
{
    PCAMERA camera;
    ISTATUS status = PinholeCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)1.0,
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PPIXEL_SAMPLER pixel_sampler;
    status =
        GridPixelSamplerAllocate(500, 500, false, 1, 1, false, &pixel_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PRANDOM rng;
    status = PermutedCongruentialRandomAllocate(
        0x853c49e6748fea9bULL,
        0xda3e39cb94b95bdbULL,
        &rng);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PINTEGRATOR path_tracer;
    status = PathTracerAllocate(8, 8, (float_t)0.0, &path_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIST_SCENE scene;
    status = ListSceneAllocate(&scene);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PALL_LIGHT_SAMPLER light_sampler;
    status = AllLightSamplerAllocate(&light_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PTONE_MAPPER tone_mapper;
    status = TestToneMapperAllocate(&tone_mapper);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSAMPLE_TRACER sample_tracer;
    status = PhysxSampleTracerAllocate(
        path_tracer,
        ListSceneTraceCallback,
        scene,
        AllLightSamplerPrepareSamplesCallback,
        AllLightSamplerNextSampleCallback, 
        light_sampler,
        tone_mapper,
        &sample_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PFRAMEBUFFER framebuffer;
    status = FramebufferAllocate(500, 500, &framebuffer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = IrisCameraRender(
        (float_t)0.0, camera, pixel_sampler, sample_tracer, rng, framebuffer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    for (size_t i = 0; i < 500; i++)
    {
        for (size_t j = 0; j < 500; j++)
        {
            COLOR3 pixel;
            status = FramebufferGetPixel(framebuffer, i, j, &pixel);
            ASSERT_EQ(status, ISTATUS_SUCCESS);
            EXPECT_EQ((float_t)0.0, pixel.x);
            EXPECT_EQ((float_t)0.0, pixel.y);
            EXPECT_EQ((float_t)0.0, pixel.z);
        }
    }

    CameraFree(camera);
    PixelSamplerFree(pixel_sampler);
    RandomFree(rng);
    ListSceneFree(scene);
    AllLightSamplerFree(light_sampler);
    SampleTracerFree(sample_tracer);
    FramebufferFree(framebuffer);
}