/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

   cornell_box.cc

Abstract:

   Integration tests which render a Cornell box.

--*/

#include <string>
#include <thread>
#include <vector>

#include "iris_advanced_toolkit/halton_sequence.h"
#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_advanced_toolkit/sobol_sequence.h"
#include "iris_camera_toolkit/grid_image_sampler.h"
#include "iris_camera_toolkit/low_discrepancy_image_sampler.h"
#include "iris_camera_toolkit/pinhole_camera.h"
#include "iris_physx_toolkit/bsdfs/lambertian.h"
#include "iris_physx_toolkit/shapes/triangle.h"
#include "iris_physx_toolkit/attenuated_reflector.h"
#include "iris_physx_toolkit/cie_color_integrator.h"
#include "iris_physx_toolkit/constant_emissive_material.h"
#include "iris_physx_toolkit/materials/constant.h"
#include "iris_physx_toolkit/interpolated_spectrum.h"
#include "iris_physx_toolkit/kd_tree_scene.h"
#include "iris_physx_toolkit/one_light_sampler.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/sample_tracer.h"
#include "googletest/include/gtest/gtest.h"
#include "test_util/cornell_box.h"
#include "test_util/equality.h"
#include "test_util/quad.h"

static
void
AddQuadToScene(
    _In_ POINT3 v0,
    _In_ POINT3 v1,
    _In_ POINT3 v2,
    _In_ POINT3 v3,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _Inout_ std::vector<PSHAPE> *shapes
    )
{
    PSHAPE shape0, shape1;
    ISTATUS status = EmissiveQuadAllocate(
        v0,
        v1,
        v2,
        v3,
        front_material,
        back_material,
        nullptr,
        nullptr,
        &shape0,
        &shape1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    shapes->push_back(shape0);
    shapes->push_back(shape1);
}

void
TestRender(
    _In_ PRANDOM rng0,
    _In_ PRANDOM rng1,
    _In_ PIMAGE_SAMPLER image_sampler
    )
{

    PCOLOR_INTEGRATOR color_integrator;
    ISTATUS status = CieColorIntegratorAllocate(&color_integrator);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR white_reflector;
    status = InterpolatedReflectorAllocate(cornell_box_wall_wavelengths,
                                           cornell_box_white_wall_samples,
                                           CORNELL_BOX_WALL_SAMPLES,
                                           &white_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF white_bsdf;
    status = LambertianBsdfAllocate(white_reflector, &white_bsdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL white_material;
    status = ConstantMaterialAllocate(white_bsdf, &white_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR red_reflector;
    status = InterpolatedReflectorAllocate(cornell_box_wall_wavelengths,
                                           cornell_box_red_wall_samples,
                                           CORNELL_BOX_WALL_SAMPLES,
                                           &red_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF red_bsdf;
    status = LambertianBsdfAllocate(red_reflector, &red_bsdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL red_material;
    status = ConstantMaterialAllocate(red_bsdf, &red_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR green_reflector;
    status = InterpolatedReflectorAllocate(cornell_box_wall_wavelengths,
                                           cornell_box_green_wall_samples,
                                           CORNELL_BOX_WALL_SAMPLES,
                                           &green_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF green_bsdf;
    status = LambertianBsdfAllocate(green_reflector, &green_bsdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL green_material;
    status = ConstantMaterialAllocate(green_bsdf, &green_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSPECTRUM light_spectrum;
    status = InterpolatedSpectrumAllocate(cornell_box_light_wavelengths,
                                          cornell_box_light_samples,
                                          CORNELL_BOX_LIGHT_SAMPLES,
                                          &light_spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL light_material;
    status = ConstantEmissiveMaterialAllocate(light_spectrum, &light_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    std::vector<PSHAPE> shapes;

    PSHAPE light_shape0, light_shape1;
    status = EmissiveQuadAllocate(
        cornell_box_light[0],
        cornell_box_light[1],
        cornell_box_light[2],
        cornell_box_light[3],
        nullptr,
        nullptr,
        nullptr,
        light_material,
        &light_shape0,
        &light_shape1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    shapes.push_back(light_shape0);
    shapes.push_back(light_shape1);

    PLIGHT light0;
    status = AreaLightAllocate(light_shape0,
                               TRIANGLE_BACK_FACE,
                               nullptr,
                               &light0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light1;
    status = AreaLightAllocate(light_shape1,
                               TRIANGLE_BACK_FACE,
                               nullptr,
                               &light1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT lights[2] = { light0, light1 };
    PLIGHT_SAMPLER light_sampler;
    status = OneLightSamplerAllocate(lights, 2, &light_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    AddQuadToScene(
        cornell_box_ceiling[0],
        cornell_box_ceiling[1],
        cornell_box_ceiling[2],
        cornell_box_ceiling[3],
        white_material,
        white_material,
        &shapes);

    AddQuadToScene(
        cornell_box_back_wall[0],
        cornell_box_back_wall[1],
        cornell_box_back_wall[2],
        cornell_box_back_wall[3],
        white_material,
        white_material,
        &shapes);
                
    AddQuadToScene(
        cornell_box_floor[0],
        cornell_box_floor[1],
        cornell_box_floor[2],
        cornell_box_floor[3],
        white_material,
        white_material,
        &shapes);

    AddQuadToScene(
        cornell_box_left_wall[0],
        cornell_box_left_wall[1],
        cornell_box_left_wall[2],
        cornell_box_left_wall[3],
        red_material,
        red_material,
        &shapes);

    AddQuadToScene(
        cornell_box_right_wall[0],
        cornell_box_right_wall[1],
        cornell_box_right_wall[2],
        cornell_box_right_wall[3],
        green_material,
        green_material,
        &shapes);

    for (size_t i = 0; i < 5; i++)
    {
        AddQuadToScene(
            cornell_box_short_box[i][0],
            cornell_box_short_box[i][1],
            cornell_box_short_box[i][2],
            cornell_box_short_box[i][3],
            white_material,
            white_material,
            &shapes);
    }

    for (size_t i = 0; i < 5; i++)
    {
        AddQuadToScene(
            cornell_box_tall_box[i][0],
            cornell_box_tall_box[i][1],
            cornell_box_tall_box[i][2],
            cornell_box_tall_box[i][3],
            white_material,
            white_material,
            &shapes);
    }

    PSCENE scene;
    status = KdTreeSceneAllocate(shapes.data(),
                                 nullptr,
                                 nullptr,
                                 shapes.size(),
                                 nullptr,
                                 &scene);
    EXPECT_EQ(ISTATUS_SUCCESS, status);

    PCAMERA camera;
    status = PinholeCameraAllocate(
        cornell_box_camera_location,
        cornell_box_camera_direction,
        cornell_box_camera_up,
        cornell_box_focal_length,
        cornell_box_camera_width,
        cornell_box_camera_height,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PINTEGRATOR path_tracer;
    status = PathTracerAllocate(3, 5, (float_t)0.05, INFINITY, &path_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSAMPLE_TRACER sample_tracer;
    status = PhysxSampleTracerAllocate(path_tracer, &sample_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = IntegratorPrepare(path_tracer,
                               scene,
                               light_sampler,
                               color_integrator);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PFRAMEBUFFER framebuffer0;
    status = FramebufferAllocate(100, 100, &framebuffer0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = IrisCameraRenderSingleThreaded(camera,
                                            nullptr,
                                            image_sampler,
                                            sample_tracer,
                                            rng0,
                                            framebuffer0,
                                            nullptr,
                                            (float_t)0.01);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PFRAMEBUFFER framebuffer1;
    status = FramebufferAllocate(100, 100, &framebuffer1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    size_t num_threads = std::max(2u, std::thread::hardware_concurrency());
    status = IrisCameraRender(camera,
                              nullptr,
                              image_sampler,
                              sample_tracer,
                              rng1,
                              framebuffer1,
                              nullptr,
                              (float_t)0.01,
                              num_threads);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    for (size_t i = 0; i < 100; i++)
    {
        for (size_t j = 0; j < 100; j++)
        {
            COLOR3 color0;
            status = FramebufferGetPixel(framebuffer0, i, j, &color0);
            ASSERT_EQ(status, ISTATUS_SUCCESS);

            COLOR3 color1;
            status = FramebufferGetPixel(framebuffer1, i, j, &color1);
            ASSERT_EQ(status, ISTATUS_SUCCESS);

            EXPECT_EQ(color0, color1);
        }
    }

    SampleTracerFree(sample_tracer);
    FramebufferFree(framebuffer0);
    FramebufferFree(framebuffer1);

    for (PSHAPE shape : shapes)
    {
        ShapeRelease(shape);
    }

    EmissiveMaterialRelease(light_material);
    SpectrumRelease(light_spectrum);
    ReflectorRelease(white_reflector);
    ReflectorRelease(red_reflector);
    ReflectorRelease(green_reflector);
    BsdfRelease(white_bsdf);
    BsdfRelease(red_bsdf);
    BsdfRelease(green_bsdf);
    MaterialRelease(white_material);
    MaterialRelease(red_material);
    MaterialRelease(green_material);
    LightRelease(light0);
    LightRelease(light1);
    SceneRelease(scene);
    LightSamplerRelease(light_sampler);
    CameraFree(camera);
    ColorIntegratorRelease(color_integrator);
}

TEST(DeterministicTest, PcgGridSampler)
{
    PRANDOM rng0;
    ISTATUS status = PermutedCongruentialRandomAllocate(
        0x853c49e6748fea9bULL,
        0xda3e39cb94b95bdbULL,
        &rng0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PRANDOM rng1;
    status = PermutedCongruentialRandomAllocate(
        0x853c49e6748fea9bULL,
        0xda3e39cb94b95bdbULL,
        &rng1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PIMAGE_SAMPLER pixel_sampler;
    status =
        GridImageSamplerAllocate(1, 1, false, 1, 1, false, &pixel_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(rng0, rng1, pixel_sampler);

    RandomFree(rng0);
    RandomFree(rng1);
    ImageSamplerFree(pixel_sampler);
}

TEST(DeterministicTest, PcgHalton)
{
    PRANDOM rng0;
    ISTATUS status = PermutedCongruentialRandomAllocate(
        0x853c49e6748fea9bULL,
        0xda3e39cb94b95bdbULL,
        &rng0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PRANDOM rng1;
    status = PermutedCongruentialRandomAllocate(
        0x853c49e6748fea9bULL,
        0xda3e39cb94b95bdbULL,
        &rng1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLOW_DISCREPANCY_SEQUENCE sequence;
    status = HaltonSequenceAllocate(&sequence);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PIMAGE_SAMPLER pixel_sampler;
    status =
        LowDiscrepancyImageSamplerAllocate(sequence, 1, &pixel_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(rng0, rng1, pixel_sampler);

    RandomFree(rng0);
    RandomFree(rng1);
    ImageSamplerFree(pixel_sampler);
}

TEST(DeterministicTest, PcgSobol)
{
    PRANDOM rng0;
    ISTATUS status = PermutedCongruentialRandomAllocate(
        0x853c49e6748fea9bULL,
        0xda3e39cb94b95bdbULL,
        &rng0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PRANDOM rng1;
    status = PermutedCongruentialRandomAllocate(
        0x853c49e6748fea9bULL,
        0xda3e39cb94b95bdbULL,
        &rng1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLOW_DISCREPANCY_SEQUENCE sequence;
    status = SobolSequenceAllocate(&sequence);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PIMAGE_SAMPLER pixel_sampler;
    status =
        LowDiscrepancyImageSamplerAllocate(sequence, 1, &pixel_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(rng0, rng1, pixel_sampler);

    RandomFree(rng0);
    RandomFree(rng1);
    ImageSamplerFree(pixel_sampler);
}