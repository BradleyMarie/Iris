/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

   cornell_box_dielectric.cc

Abstract:

   Integration tests which render a Cornell box with a dielectric sphere.

--*/

#include <string>
#include <vector>

#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_camera_toolkit/grid_pixel_sampler.h"
#include "iris_camera_toolkit/pinhole_camera.h"
#include "iris_physx_toolkit/attenuated_reflector.h"
#include "iris_physx_toolkit/cie_color_integrator.h"
#include "iris_physx_toolkit/constant_emissive_material.h"
#include "iris_physx_toolkit/constant_material.h"
#include "iris_physx_toolkit/fresnel_bsdf.h"
#include "iris_physx_toolkit/interpolated_spectrum.h"
#include "iris_physx_toolkit/kd_tree_scene.h"
#include "iris_physx_toolkit/lambertian_bsdf.h"
#include "iris_physx_toolkit/one_light_sampler.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/sample_tracer.h"
#include "iris_physx_toolkit/sphere.h"
#include "iris_physx_toolkit/triangle.h"
#include "iris_physx_toolkit/uniform_reflector.h"
#include "googletest/include/gtest/gtest.h"
#include "test_util/cornell_box.h"
#include "test_util/pfm.h"
#include "test_util/quad.h"

void
TestRenderSingleThreaded(
    _In_ PCCAMERA camera,
    _In_ PCSCENE scene,
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
    status = PathTracerAllocate(3, 5, (float_t)0.05, &path_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSAMPLE_TRACER sample_tracer;
    status = PhysxSampleTracerAllocate(
        path_tracer,
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
    status = ApproximatelyEqualsPfmFile(framebuffer,
                                        file_name.c_str(),
                                        PFM_PIXEL_FORMAT_SRGB,
                                        (float_t)0.01,
                                        &equals);
    ASSERT_EQ(status, ISTATUS_SUCCESS);
    EXPECT_TRUE(equals);

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

    PREFLECTOR perfect_reflector;
    status = UniformReflectorAllocate((float_t)1.0, &perfect_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = ColorIntegratorPrecomputeReflectorColor(color_integrator,
                                                     perfect_reflector);
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

    PREFLECTOR white_reflector;
    status = InterpolatedReflectorAllocate(cornell_box_wall_wavelengths,
                                           cornell_box_white_wall_samples,
                                           CORNELL_BOX_WALL_SAMPLES,
                                           &white_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = ColorIntegratorPrecomputeReflectorColor(color_integrator,
                                                     white_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF white_bsdf;
    status = LambertianReflectorAllocate(white_reflector, &white_bsdf);
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

    status = ColorIntegratorPrecomputeReflectorColor(color_integrator,
                                                     red_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF red_bsdf;
    status = LambertianReflectorAllocate(red_reflector, &red_bsdf);
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

    status = ColorIntegratorPrecomputeReflectorColor(color_integrator,
                                                     green_reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF green_bsdf;
    status = LambertianReflectorAllocate(green_reflector, &green_bsdf);
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

    status = ColorIntegratorPrecomputeSpectrumColor(color_integrator,
                                                    light_spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL light_material;
    status = ConstantEmissiveMaterialAllocate(light_spectrum, &light_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PONE_LIGHT_SAMPLER light_sampler;
    status = OneLightSamplerAllocate(&light_sampler);
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
                               &light0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = OneLightSamplerAddLight(light_sampler, light0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light1;
    status = AreaLightAllocate(light_shape1,
                               TRIANGLE_BACK_FACE,
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

    PMATRIX *transforms = new PMATRIX[shapes.size()];
    bool *premultiplied = new bool[shapes.size()];
    for (size_t i = 0; i < shapes.size(); i++)
    {
        transforms[i] = nullptr;
        premultiplied[i] = false;
    }

    PSCENE scene;
    status = KdTreeSceneAllocate(shapes.data(),
                                 transforms,
                                 premultiplied,
                                 shapes.size(),
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
                             color_integrator,
                             "test_results/cornell_box_with_dielectric.pfm");

    for (PSHAPE shape : shapes)
    {
        ShapeRelease(shape);
    }

    delete[] transforms;
    delete[] premultiplied;

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
    SceneFree(scene);
    OneLightSamplerFree(light_sampler);
    CameraFree(camera);
    ColorIntegratorFree(color_integrator);
}