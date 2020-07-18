/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    single_sphere.cc

Abstract:

    Integration tests which render a single sphere.

--*/

#include <string>

#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_camera_toolkit/grid_image_sampler.h"
#include "iris_camera_toolkit/pinhole_camera.h"
#include "iris_physx_toolkit/all_light_sampler.h"
#include "iris_physx_toolkit/constant_material.h"
#include "iris_physx_toolkit/lambertian_bsdf.h"
#include "iris_physx_toolkit/list_scene.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/point_light.h"
#include "iris_physx_toolkit/sample_tracer.h"
#include "iris_physx_toolkit/sphere.h"
#include "googletest/include/gtest/gtest.h"
#include "test_util/pfm.h"
#include "test_util/spectra.h"

void
TestRenderSingleThreaded(
    _In_ PSCENE scene,
    _In_ PLIGHT_SAMPLER light_sampler,
    _In_ const std::string& file_name
    )
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
    status = PathTracerAllocate(0, 0, (float_t)0.0, INFINITY, &path_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PCOLOR_INTEGRATOR color_integrator;
    status = XyzColorIntegratorAllocate(&color_integrator);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSAMPLE_TRACER sample_tracer;
    status = PhysxSampleTracerAllocate(path_tracer, &sample_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = IntegratorPrepare(path_tracer,
                               scene,
                               light_sampler,
                               color_integrator,
                               true);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PFRAMEBUFFER framebuffer;
    status = FramebufferAllocate(150, 150, &framebuffer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = IrisCameraRenderSingleThreaded(camera,
                                            nullptr,
                                            image_sampler,
                                            sample_tracer,
                                            rng,
                                            framebuffer,
                                            (float_t)0.001);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    bool equals;
    status = ApproximatelyEqualsPfmFile(framebuffer,
                                        file_name.c_str(),
                                        COLOR_SPACE_XYZ,
                                        (float_t)0.001,
                                        &equals);
    ASSERT_EQ(status, ISTATUS_SUCCESS);
    EXPECT_TRUE(equals);

    CameraFree(camera);
    ImageSamplerFree(image_sampler);
    RandomFree(rng);
    SampleTracerFree(sample_tracer);
    FramebufferFree(framebuffer);
    ColorIntegratorRelease(color_integrator);
}

TEST(SingleSphereTest, TestReflectorRedWorldSphere)
{
    PSPECTRUM spectrum;
    ISTATUS status = XyzSpectrumAllocate((float_t)1.0,
                                         (float_t)0.0,
                                         (float_t)0.0,
                                         &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector;
    status = XyzReflectorAllocate((float_t)1.0,
                                  (float_t)0.0,
                                  (float_t)0.0,
                                  &reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF bsdf;
    status = LambertianBsdfAllocate(reflector, &bsdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = PointLightAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = ConstantMaterialAllocate(bsdf, &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = SphereAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        (float_t)0.5,
        material,
        material,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATRIX matrix = nullptr;
    bool premultiplied = false;

    PSCENE scene;
    status = ListSceneAllocate(&shape,
                               &matrix,
                               &premultiplied,
                               1,
                               &scene);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT_SAMPLER light_sampler;
    status = AllLightSamplerAllocate(&light, 1, &light_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRenderSingleThreaded(scene,
                             light_sampler, 
                             "test_results/sphere_center.pfm");

    SceneRelease(scene);
    LightSamplerRelease(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector);
    LightRelease(light);
    BsdfRelease(bsdf);
    MaterialRelease(material);
    ShapeRelease(shape);
}

TEST(SingleSphereTest, TestReflectorRedTransformedAwaySphere)
{
    PSPECTRUM spectrum;
    ISTATUS status = XyzSpectrumAllocate((float_t)1.0,
                                         (float_t)0.0,
                                         (float_t)0.0,
                                         &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector;
    status = XyzReflectorAllocate((float_t)1.0,
                                  (float_t)0.0,
                                  (float_t)0.0,
                                  &reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF bsdf;
    status = LambertianBsdfAllocate(reflector, &bsdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = PointLightAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = ConstantMaterialAllocate(bsdf, &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = SphereAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        (float_t)0.5,
        material,
        material,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATRIX matrix;
    status = MatrixAllocateTranslation(
        (float_t)0.0, (float_t)0.0, (float_t)1.0, &matrix);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    bool premultiplied = false;

    PSCENE scene;
    status = ListSceneAllocate(&shape,
                               &matrix,
                               &premultiplied,
                               1,
                               &scene);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT_SAMPLER light_sampler;
    status = AllLightSamplerAllocate(&light, 1, &light_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRenderSingleThreaded(scene,
                             light_sampler, 
                             "test_results/sphere_away.pfm");

    SceneRelease(scene);
    LightSamplerRelease(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector);
    LightRelease(light);
    BsdfRelease(bsdf);
    MaterialRelease(material);
    ShapeRelease(shape);
    MatrixRelease(matrix);
}

TEST(SingleSphereTest, TestReflectorRedTransformedUpSphere)
{
    PSPECTRUM spectrum;
    ISTATUS status = XyzSpectrumAllocate((float_t)1.0,
                                         (float_t)0.0,
                                         (float_t)0.0,
                                         &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector;
    status = XyzReflectorAllocate((float_t)1.0,
                                  (float_t)0.0,
                                  (float_t)0.0,
                                  &reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF bsdf;
    status = LambertianBsdfAllocate(reflector, &bsdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = PointLightAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = ConstantMaterialAllocate(bsdf, &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = SphereAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        (float_t)0.5,
        material,
        material,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATRIX matrix;
    status = MatrixAllocateTranslation(
        (float_t)0.0, (float_t)1.0, (float_t)1.0, &matrix);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    bool premultiplied = false;

    PSCENE scene;
    status = ListSceneAllocate(&shape,
                               &matrix,
                               &premultiplied,
                               1,
                               &scene);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT_SAMPLER light_sampler;
    status = AllLightSamplerAllocate(&light, 1, &light_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRenderSingleThreaded(scene,
                             light_sampler, 
                             "test_results/sphere_up.pfm");

    SceneRelease(scene);
    LightSamplerRelease(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector);
    LightRelease(light);
    BsdfRelease(bsdf);
    MaterialRelease(material);
    ShapeRelease(shape);
    MatrixRelease(matrix);
}

TEST(SingleSphereTest, TestReflectorRedTransformedRightSphere)
{
    PSPECTRUM spectrum;
    ISTATUS status = XyzSpectrumAllocate((float_t)1.0,
                                         (float_t)0.0,
                                         (float_t)0.0,
                                         &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector;
    status = XyzReflectorAllocate((float_t)1.0,
                                  (float_t)0.0,
                                  (float_t)0.0,
                                  &reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF bsdf;
    status = LambertianBsdfAllocate(reflector, &bsdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = PointLightAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = ConstantMaterialAllocate(bsdf, &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = SphereAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        (float_t)0.5,
        material,
        material,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATRIX matrix;
    status = MatrixAllocateTranslation(
        (float_t)1.0, (float_t)0.0, (float_t)1.0, &matrix);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    bool premultiplied = false;

    PSCENE scene;
    status = ListSceneAllocate(&shape,
                               &matrix,
                               &premultiplied,
                               1,
                               &scene);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT_SAMPLER light_sampler;
    status = AllLightSamplerAllocate(&light, 1, &light_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRenderSingleThreaded(scene,
                             light_sampler, 
                             "test_results/sphere_right.pfm");

    SceneRelease(scene);
    LightSamplerRelease(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector);
    LightRelease(light);
    BsdfRelease(bsdf);
    MaterialRelease(material);
    ShapeRelease(shape);
    MatrixRelease(matrix);
}