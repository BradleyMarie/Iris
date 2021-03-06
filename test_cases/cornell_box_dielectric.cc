/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

   cornell_box_dielectric.cc

Abstract:

   Integration tests which render a Cornell box with a dielectric sphere.

--*/

#include <string>
#include <vector>

#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_camera_toolkit/grid_image_sampler.h"
#include "iris_camera_toolkit/pinhole_camera.h"
#include "iris_physx_toolkit/bsdfs/fresnel.h"
#include "iris_physx_toolkit/bsdfs/lambertian.h"
#include "iris_physx_toolkit/materials/constant.h"
#include "iris_physx_toolkit/scenes/kd_tree.h"
#include "iris_physx_toolkit/shapes/sphere.h"
#include "iris_physx_toolkit/shapes/triangle.h"
#include "iris_physx_toolkit/attenuated_reflector.h"
#include "iris_physx_toolkit/cie_color_integrator.h"
#include "iris_physx_toolkit/color_spectra.h"
#include "iris_physx_toolkit/constant_emissive_material.h"
#include "iris_physx_toolkit/interpolated_spectrum.h"
#include "iris_physx_toolkit/one_light_sampler.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/sample_tracer.h"
#include "iris_physx_toolkit/uniform_reflector.h"
#include "googletest/include/gtest/gtest.h"
#include "test_util/cornell_box.h"
#include "test_util/pfm.h"
#include "test_util/quad.h"

void
TestRenderSingleThreaded(
    _In_ PCCAMERA camera,
    _In_ PSCENE scene,
    _In_ PLIGHT_SAMPLER light_sampler,
    _In_ const std::string& file_name
    )
{
    PIMAGE_SAMPLER image_sampler;
    ISTATUS status =
        GridImageSamplerAllocate(1, 1, false, 1, 1, false, &image_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PRANDOM rng;
    status = PermutedCongruentialRandomAllocate(
        0x853c49e6748fea9bULL,
        0xda3e39cb94b95bdbULL,
        &rng);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PINTEGRATOR path_tracer;
    status = PathTracerAllocate(3, 5, (float_t)0.05, INFINITY, &path_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PCOLOR_INTEGRATOR color_integrator;
    status = ColorColorIntegratorAllocate(COLOR_SPACE_XYZ, &color_integrator);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = IntegratorPrepare(path_tracer,
                               scene,
                               light_sampler,
                               color_integrator);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSAMPLE_TRACER sample_tracer;
    status = PhysxSampleTracerAllocate(path_tracer, &sample_tracer);
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
                                            (float_t)0.01);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    bool equals;
    status = ApproximatelyEqualsPfmFile(framebuffer,
                                        file_name.c_str(),
                                        COLOR_SPACE_LINEAR_SRGB,
                                        (float_t)0.01,
                                        &equals);
    ASSERT_EQ(status, ISTATUS_SUCCESS);
    EXPECT_TRUE(equals);

    ImageSamplerFree(image_sampler);
    RandomFree(rng);
    ColorIntegratorRelease(color_integrator);
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

TEST(CornellBoxDielectricTest, CornellBox)
{
    PCOLOR_INTEGRATOR color_integrator;
    ISTATUS status = CieColorIntegratorAllocate(&color_integrator);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PCOLOR_EXTRAPOLATOR color_extrapolator;
    status = ColorColorExtrapolatorAllocate(COLOR_SPACE_XYZ,
                                            &color_extrapolator);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR perfect_reflector;
    status = UniformReflectorAllocate((float_t)1.0, &perfect_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF glass_front_bsdf;
    status = SpecularDielectricBsdfAllocate(perfect_reflector,
                                            perfect_reflector,
                                            (float_t)1.0,
                                            (float_t)1.5,
                                            &glass_front_bsdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL glass_front_material;
    status = ConstantMaterialAllocate(glass_front_bsdf, &glass_front_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF glass_back_bsdf;
    status = SpecularDielectricBsdfAllocate(perfect_reflector,
                                            perfect_reflector,
                                            (float_t)1.5,
                                            (float_t)1.0,
                                            &glass_back_bsdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL glass_back_material;
    status = ConstantMaterialAllocate(glass_back_bsdf, &glass_back_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR interpolated_white_reflector;
    status = InterpolatedReflectorAllocate(cornell_box_wall_wavelengths,
                                           cornell_box_white_wall_samples,
                                           CORNELL_BOX_WALL_SAMPLES,
                                           &interpolated_white_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    COLOR3 color;
    status = ColorIntegratorComputeReflectorColor(color_integrator,
                                                  interpolated_white_reflector,
                                                  &color);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR white_reflector;
    status = ColorExtrapolatorComputeReflector(color_extrapolator,
                                               color,
                                               &white_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    ReflectorRelease(interpolated_white_reflector);

    PBSDF white_bsdf;
    status = LambertianBsdfAllocate(white_reflector, &white_bsdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL white_material;
    status = ConstantMaterialAllocate(white_bsdf, &white_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR interpolated_red_reflector;
    status = InterpolatedReflectorAllocate(cornell_box_wall_wavelengths,
                                           cornell_box_red_wall_samples,
                                           CORNELL_BOX_WALL_SAMPLES,
                                           &interpolated_red_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = ColorIntegratorComputeReflectorColor(color_integrator,
                                                  interpolated_red_reflector,
                                                  &color);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR red_reflector;
    status = ColorExtrapolatorComputeReflector(color_extrapolator,
                                               color,
                                               &red_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    ReflectorRelease(interpolated_red_reflector);

    PBSDF red_bsdf;
    status = LambertianBsdfAllocate(red_reflector, &red_bsdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL red_material;
    status = ConstantMaterialAllocate(red_bsdf, &red_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR interpolated_green_reflector;
    status = InterpolatedReflectorAllocate(cornell_box_wall_wavelengths,
                                           cornell_box_green_wall_samples,
                                           CORNELL_BOX_WALL_SAMPLES,
                                           &interpolated_green_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = ColorIntegratorComputeReflectorColor(color_integrator,
                                                  interpolated_green_reflector,
                                                  &color);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR green_reflector;
    status = ColorExtrapolatorComputeReflector(color_extrapolator,
                                               color,
                                               &green_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    ReflectorRelease(interpolated_green_reflector);

    PBSDF green_bsdf;
    status = LambertianBsdfAllocate(green_reflector, &green_bsdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL green_material;
    status = ConstantMaterialAllocate(green_bsdf, &green_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSPECTRUM interpolated_light_spectrum;
    status = InterpolatedSpectrumAllocate(cornell_box_light_wavelengths,
                                          cornell_box_light_samples,
                                          CORNELL_BOX_LIGHT_SAMPLES,
                                          &interpolated_light_spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = ColorIntegratorComputeSpectrumColor(color_integrator,
                                                 interpolated_light_spectrum,
                                                 &color);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSPECTRUM light_spectrum;
    status = ColorExtrapolatorComputeSpectrum(color_extrapolator,
                                              color,
                                              &light_spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    SpectrumRelease(interpolated_light_spectrum);

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

    PSHAPE sphere;
    POINT3 center = PointCreate((float_t)-186.0, (float_t)82.0, (float_t)168.0);
    status = SphereAllocate(center,
                            (float_t)82.0,
                            glass_front_material,
                            glass_back_material,
                            &sphere);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    shapes.push_back(sphere);

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

    TestRenderSingleThreaded(camera,
                             scene,
                             light_sampler,
                             "test_results/cornell_box_with_dielectric.pfm");

    for (PSHAPE shape : shapes)
    {
        ShapeRelease(shape);
    }

    EmissiveMaterialRelease(light_material);
    SpectrumRelease(light_spectrum);
    ReflectorRelease(perfect_reflector);
    ReflectorRelease(white_reflector);
    ReflectorRelease(red_reflector);
    ReflectorRelease(green_reflector);
    BsdfRelease(glass_front_bsdf);
    BsdfRelease(glass_back_bsdf);
    BsdfRelease(white_bsdf);
    BsdfRelease(red_bsdf);
    BsdfRelease(green_bsdf);
    MaterialRelease(glass_front_material);
    MaterialRelease(glass_back_material);
    MaterialRelease(white_material);
    MaterialRelease(red_material);
    MaterialRelease(green_material);
    LightRelease(light0);
    LightRelease(light1);
    SceneRelease(scene);
    LightSamplerRelease(light_sampler);
    CameraFree(camera);
    ColorExtrapolatorFree(color_extrapolator);
    ColorIntegratorRelease(color_integrator);
}