/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

   cornell_box.cc

Abstract:

   Integration tests which render a single triangle.

--*/

#include <string>

#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_camera_toolkit/grid_pixel_sampler.h"
#include "iris_camera_toolkit/pfm_writer.h"
#include "iris_camera_toolkit/pinhole_camera.h"
#include "iris_physx_toolkit/attenuated_reflector.h"
#include "iris_physx_toolkit/cie_color_integrator.h"
#include "iris_physx_toolkit/constant_emissive_material.h"
#include "iris_physx_toolkit/constant_material.h"
#include "iris_physx_toolkit/interpolated_spectrum.h"
#include "iris_physx_toolkit/lambertian_brdf.h"
#include "iris_physx_toolkit/list_scene.h"
#include "iris_physx_toolkit/one_light_sampler.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/sample_tracer.h"
#include "iris_physx_toolkit/triangle.h"
#include "googletest/include/gtest/gtest.h"
#include "test_util/cornell_box.h"
#include "test_util/pfm.h"
#include "test_util/quad.h"

void
TestRenderSingleThreaded(
    _In_ PCCAMERA camera,
    _In_ PCLIST_SCENE scene,
    _In_ PONE_LIGHT_SAMPLER light_sampler,
    _In_ PCOLOR_INTEGRATOR color_integrator,
    _In_ const std::string& file_name
    )
{
    PPIXEL_SAMPLER pixel_sampler;
    ISTATUS status =
        GridPixelSamplerAllocate(1, 1, false, 1, 1, false, &pixel_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PRANDOM rng;
    status = PermutedCongruentialRandomAllocate(
        0x853c49e6748fea9bULL,
        0xda3e39cb94b95bdbULL,
        &rng);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PINTEGRATOR path_tracer;
    status = PathTracerAllocate(0, 0, (float_t)0.0, &path_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSAMPLE_TRACER sample_tracer;
    status = PhysxSampleTracerAllocate(
        path_tracer,
        ListSceneTraceCallback,
        scene,
        OneLightSamplerSampleLightsCallback,
        light_sampler,
        color_integrator,
        &sample_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PFRAMEBUFFER framebuffer;
    status = FramebufferAllocate(500, 500, &framebuffer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = IrisCameraRender(camera,
                              pixel_sampler,
                              sample_tracer,
                              rng,
                              framebuffer,
                              (float_t)0.01,
                              1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    bool equals;
    status = ExactlyEqualsPfmFile(framebuffer,
                                  file_name.c_str(),
                                  PFM_PIXEL_FORMAT_SRGB,
                                  &equals);
    ASSERT_EQ(status, ISTATUS_SUCCESS);
    EXPECT_TRUE(equals);

    if (!equals)
    {
        status = WriteToPfmFile(framebuffer,
                                file_name.c_str(),
                                PFM_PIXEL_FORMAT_SRGB);
        ASSERT_EQ(status, ISTATUS_SUCCESS);
    }

    PixelSamplerFree(pixel_sampler);
    RandomFree(rng);
    SampleTracerFree(sample_tracer);
    FramebufferFree(framebuffer);
}

static
void
AddQuadToScene(
    _In_ POINT3 v0,
    _In_ POINT3 v1,
    _In_ POINT3 v2,
    _In_ POINT3 v3,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _Inout_ PLIST_SCENE scene
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

    status = ListSceneAddShape(scene, shape0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = ListSceneAddShape(scene, shape1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    ShapeRelease(shape0);
    ShapeRelease(shape1);
}

TEST(CornellBoxTest, CornellBox)
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

    status = ColorIntegratorPrecomputeReflectorColor(color_integrator,
                                                     white_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBRDF white_brdf;
    status = LambertianBrdfAllocate(white_reflector, &white_brdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL white_material;
    status = ConstantMaterialAllocate(white_brdf, &white_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR red_reflector;
    status = InterpolatedReflectorAllocate(cornell_box_wall_wavelengths,
                                           cornell_box_red_wall_samples,
                                           CORNELL_BOX_WALL_SAMPLES,
                                           &red_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = ColorIntegratorPrecomputeReflectorColor(color_integrator,
                                                     red_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBRDF red_brdf;
    status = LambertianBrdfAllocate(red_reflector, &red_brdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL red_material;
    status = ConstantMaterialAllocate(red_brdf, &red_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR green_reflector;
    status = InterpolatedReflectorAllocate(cornell_box_wall_wavelengths,
                                           cornell_box_green_wall_samples,
                                           CORNELL_BOX_WALL_SAMPLES,
                                           &green_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = ColorIntegratorPrecomputeReflectorColor(color_integrator,
                                                     green_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBRDF green_brdf;
    status = LambertianBrdfAllocate(green_reflector, &green_brdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL green_material;
    status = ConstantMaterialAllocate(green_brdf, &green_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSPECTRUM light_spectrum;
    status = InterpolatedSpectrumAllocate(cornell_box_light_wavelengths,
                                          cornell_box_light_samples,
                                          CORNELL_BOX_LIGHT_SAMPLES,
                                          &light_spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = ColorIntegratorPrecomputeSpectrumColor(color_integrator,
                                                    light_spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL light_material;
    status = ConstantEmissiveMaterialAllocate(light_spectrum, &light_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIST_SCENE scene;
    status = ListSceneAllocate(&scene);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PONE_LIGHT_SAMPLER light_sampler;
    status = OneLightSamplerAllocate(&light_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

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

    status = ListSceneAddShape(scene, light_shape0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = ListSceneAddShape(scene, light_shape1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light0;
    status = AreaLightAllocate(light_shape0,
                               TRIANGLE_BACK_FACE,
                               nullptr,
                               &light0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = OneLightSamplerAddLight(light_sampler, light0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light1;
    status = AreaLightAllocate(light_shape1,
                               TRIANGLE_BACK_FACE,
                               nullptr,
                               &light1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = OneLightSamplerAddLight(light_sampler, light1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    AddQuadToScene(
        cornell_box_ceiling[0],
        cornell_box_ceiling[1],
        cornell_box_ceiling[2],
        cornell_box_ceiling[3],
        white_material,
        white_material,
        scene);

    AddQuadToScene(
        cornell_box_back_wall[0],
        cornell_box_back_wall[1],
        cornell_box_back_wall[2],
        cornell_box_back_wall[3],
        white_material,
        white_material,
        scene);
                
    AddQuadToScene(
        cornell_box_floor[0],
        cornell_box_floor[1],
        cornell_box_floor[2],
        cornell_box_floor[3],
        white_material,
        white_material,
        scene);

    AddQuadToScene(
        cornell_box_left_wall[0],
        cornell_box_left_wall[1],
        cornell_box_left_wall[2],
        cornell_box_left_wall[3],
        red_material,
        red_material,
        scene);

    AddQuadToScene(
        cornell_box_right_wall[0],
        cornell_box_right_wall[1],
        cornell_box_right_wall[2],
        cornell_box_right_wall[3],
        green_material,
        green_material,
        scene);

    for (size_t i = 0; i < 5; i++)
    {
        AddQuadToScene(
            cornell_box_short_box[i][0],
            cornell_box_short_box[i][1],
            cornell_box_short_box[i][2],
            cornell_box_short_box[i][3],
            white_material,
            white_material,
            scene);
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
            scene);
    }

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

    TestRenderSingleThreaded(camera,
                             scene,
                             light_sampler,
                             color_integrator,
                             "test_results/cornell_box.pfm");

    SpectrumRelease(light_spectrum);
    ReflectorRelease(white_reflector);
    ReflectorRelease(red_reflector);
    ReflectorRelease(green_reflector);
    BrdfRelease(white_brdf);
    BrdfRelease(red_brdf);
    BrdfRelease(green_brdf);
    MaterialRelease(white_material);
    MaterialRelease(red_material);
    MaterialRelease(green_material);
    ShapeRelease(light_shape0);
    ShapeRelease(light_shape1);
    LightRelease(light0);
    LightRelease(light1);
    ListSceneFree(scene);
    OneLightSamplerFree(light_sampler);
    CameraFree(camera);
    ColorIntegratorFree(color_integrator);
}