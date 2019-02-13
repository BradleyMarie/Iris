/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

   teapot.cc

Abstract:

   Integration tests which render the Utah teapot.

--*/

#include <string>
#include <thread>

#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_camera_toolkit/grid_pixel_sampler.h"
#include "iris_camera_toolkit/pinhole_camera.h"
#include "iris_physx_toolkit/all_light_sampler.h"
#include "iris_physx_toolkit/attenuated_reflector.h"
#include "iris_physx_toolkit/constant_material.h"
#include "iris_physx_toolkit/lambertian_brdf.h"
#include "iris_physx_toolkit/list_scene.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/point_light.h"
#include "iris_physx_toolkit/sample_tracer.h"
#include "iris_physx_toolkit/triangle.h"
#include "googletest/include/gtest/gtest.h"
#include "test_util/teapot.h"
#include "test_util/pfm.h"
#include "test_util/spectra.h"

void
TestRenderSingleThreaded(
    _In_ PCLIST_SCENE scene,
    _In_ PALL_LIGHT_SAMPLER light_sampler,
    _In_ const std::string& file_name
    )
{
    PCAMERA camera;
    ISTATUS status = PinholeCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-5.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)1.0,
        (float_t)1.5,
        (float_t)1.5,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PPIXEL_SAMPLER pixel_sampler;
    status = GridPixelSamplerAllocate(1, 1, false, 1, 1, false, &pixel_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PRANDOM rng;
    status = PermutedCongruentialRandomAllocate(
        0x853c49e6748fea9bULL,
        0xda3e39cb94b95bdbULL,
        &rng);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PINTEGRATOR path_tracer;
    status = PathTracerAllocate(0, 0, (float_t)0.00, &path_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PCOLOR_INTEGRATOR color_integrator;
    status = TestColorIntegratorAllocate(&color_integrator);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSAMPLE_TRACER sample_tracer;
    status = PhysxSampleTracerAllocate(
        path_tracer,
        ListSceneTraceCallback,
        scene,
        AllLightSamplerSampleLightsCallback,
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
                              std::thread::hardware_concurrency());
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = WriteToPfmFile(framebuffer,
                            ("/mnt/c/Users/Brad/Documents/" + file_name).c_str(),
                            PFM_PIXEL_FORMAT_XYZ);
    EXPECT_EQ(status, ISTATUS_SUCCESS);

    bool equals;
    status = ApproximatelyEqualsPfmFile(framebuffer,
                                        file_name.c_str(),
                                        PFM_PIXEL_FORMAT_SRGB,
                                        (float_t)0.01,
                                        &equals);
    ASSERT_EQ(status, ISTATUS_SUCCESS);
    EXPECT_TRUE(equals);

    CameraFree(camera);
    PixelSamplerFree(pixel_sampler);
    RandomFree(rng);
    SampleTracerFree(sample_tracer);
    ColorIntegratorFree(color_integrator);
    FramebufferFree(framebuffer);
}

static
void
AddTeapotToScene(
    _In_opt_ PMATRIX model_to_world,
    _In_opt_ PMATERIAL front_material,
    _In_opt_ PMATERIAL back_material,
    _Inout_ PLIST_SCENE scene
    )
{
    for (size_t i = 0; i < TEAPOT_FACE_COUNT; i++)
    {
        PSHAPE shape;
        ISTATUS status = TriangleAllocate(
            teapot_vertices[teapot_faces[i].vertex0],
            teapot_vertices[teapot_faces[i].vertex1],
            teapot_vertices[teapot_faces[i].vertex2],
            front_material,
            back_material,
            &shape);
        // Ignore failures for now
        // ASSERT_EQ(status, ISTATUS_SUCCESS);

        status = ListSceneAddTransformedShape(scene, shape, model_to_world);
        ASSERT_EQ(status, ISTATUS_SUCCESS);

        ShapeRelease(shape);
    }
}

TEST(TeapotTest, FlatShadedTeapot)
{
    PLIST_SCENE scene;
    ISTATUS status = ListSceneAllocate(&scene);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PALL_LIGHT_SAMPLER light_sampler;
    status = AllLightSamplerAllocate(&light_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSPECTRUM spectrum;
    status = TestSpectrumAllocate((float_t)32.0,
                                  (float_t)0.0,
                                  (float_t)0.0,
                                  &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBRDF brdf;
    status = LambertianBrdfAllocate(reflector, &brdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = PointLightAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-5.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = AllLightSamplerAddLight(light_sampler, light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = ConstantMaterialAllocate(brdf, &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    AddTeapotToScene(
        NULL,
        material,
        material,
        scene);

    TestRenderSingleThreaded(scene,
                             light_sampler,
                             "test_results/teapot_flat.pfm");

    SpectrumRelease(spectrum);
    ReflectorRelease(reflector);
    BrdfRelease(brdf);
    MaterialRelease(material);
    LightRelease(light);
    ListSceneFree(scene);
    AllLightSamplerFree(light_sampler);
}