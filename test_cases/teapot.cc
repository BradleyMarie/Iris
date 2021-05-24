/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

   teapot.cc

Abstract:

   Integration tests which render the Utah teapot.

--*/

#include <string>
#include <thread>
#include <vector>

#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_camera_toolkit/grid_image_sampler.h"
#include "iris_camera_toolkit/pinhole_camera.h"
#include "iris_physx_toolkit/all_light_sampler.h"
#include "iris_physx_toolkit/attenuated_reflector.h"
#include "iris_physx_toolkit/constant_material.h"
#include "iris_physx_toolkit/lambertian_bsdf.h"
#include "iris_physx_toolkit/kd_tree_scene.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/point_light.h"
#include "iris_physx_toolkit/sample_tracer.h"
#include "iris_physx_toolkit/triangle_mesh_normal_map.h"
#include "iris_physx_toolkit/triangle_mesh.h"
#include "googletest/include/gtest/gtest.h"
#include "test_util/teapot.h"
#include "test_util/pfm.h"
#include "test_util/spectra.h"

//
// Tests
//

void
TestRenderSingleThreaded(
    _In_ PSCENE scene,
    _In_ PLIGHT_SAMPLER light_sampler,
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

    PIMAGE_SAMPLER image_sampler;
    status = GridImageSamplerAllocate(1, 1, false, 1, 1, false, &image_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PRANDOM rng;
    status = PermutedCongruentialRandomAllocate(
        0x853c49e6748fea9bULL,
        0xda3e39cb94b95bdbULL,
        &rng);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PINTEGRATOR path_tracer;
    status = PathTracerAllocate(0, 0, (float_t)0.00, INFINITY, &path_tracer);
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
                               color_integrator);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PFRAMEBUFFER framebuffer;
    status = FramebufferAllocate(256, 256, &framebuffer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = IrisCameraRender(camera,
                              nullptr,
                              image_sampler,
                              sample_tracer,
                              rng,
                              framebuffer,
                              nullptr,
                              (float_t)0.01,
                              1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    bool equals;
    status = ApproximatelyEqualsPfmFile(framebuffer,
                                        file_name.c_str(),
                                        COLOR_SPACE_XYZ,
                                        (float_t)0.01,
                                        &equals);
    ASSERT_EQ(status, ISTATUS_SUCCESS);
    EXPECT_TRUE(equals);

    CameraFree(camera);
    ImageSamplerFree(image_sampler);
    RandomFree(rng);
    SampleTracerFree(sample_tracer);
    ColorIntegratorRelease(color_integrator);
    FramebufferFree(framebuffer);
}

TEST(TeapotTest, FlatShadedTeapot)
{
    PSPECTRUM spectrum;
    ISTATUS status = XyzSpectrumAllocate((float_t)32.0,
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
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-5.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT_SAMPLER light_sampler;
    status = AllLightSamplerAllocate(&light, 1, &light_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = ConstantMaterialAllocate(bsdf, &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shapes[TEAPOT_FACE_COUNT] = { nullptr };
    PMATRIX transforms[TEAPOT_FACE_COUNT] = { nullptr };
    bool premultiplied[TEAPOT_FACE_COUNT] = { false };

    size_t triangles_allocated;
    status = TriangleMeshAllocate(
        teapot_vertices,
        TEAPOT_VERTEX_COUNT,
        teapot_face_vertices,
        TEAPOT_FACE_COUNT,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        material,
        nullptr,
        nullptr,
        nullptr,
        shapes,
        &triangles_allocated);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSCENE scene;
    status = KdTreeSceneAllocate(shapes,
                                 transforms,
                                 premultiplied,
                                 triangles_allocated,
                                 nullptr,
                                 &scene);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRenderSingleThreaded(scene,
                             light_sampler,
                             "test_results/teapot_flat.pfm");

    for (size_t i = 0; i < triangles_allocated; i++)
    {
        ShapeRelease(shapes[i]);
    }

    SpectrumRelease(spectrum);
    ReflectorRelease(reflector);
    BsdfRelease(bsdf);
    MaterialRelease(material);
    LightRelease(light);
    SceneRelease(scene);
    LightSamplerRelease(light_sampler);
}

TEST(TeapotTest, SmoothShadedTeapot)
{
    PSPECTRUM spectrum;
    ISTATUS status = XyzSpectrumAllocate((float_t)32.0,
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
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-5.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT_SAMPLER light_sampler;
    status = AllLightSamplerAllocate(&light, 1, &light_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    ConstantMaterialAllocate(bsdf, &material);

    PSHAPE shapes[TEAPOT_FACE_COUNT] = { nullptr };
    PMATRIX transforms[TEAPOT_FACE_COUNT] = { nullptr };
    bool premultiplied[TEAPOT_FACE_COUNT] = { false };

    PNORMAL_MAP front_normal_map;
    status = TriangleMeshNormalMapAllocate(teapot_normals,
                                           TEAPOT_VERTEX_COUNT,
                                           &front_normal_map,
                                           nullptr);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    size_t triangles_allocated;
    status = TriangleMeshAllocate(
        teapot_vertices,
        TEAPOT_VERTEX_COUNT,
        teapot_face_vertices,
        TEAPOT_FACE_COUNT,
        nullptr,
        nullptr,
        front_normal_map,
        nullptr,
        material,
        nullptr,
        nullptr,
        nullptr,
        shapes,
        &triangles_allocated);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSCENE scene;
    status = KdTreeSceneAllocate(shapes,
                                 transforms,
                                 premultiplied,
                                 triangles_allocated,
                                 nullptr,
                                 &scene);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRenderSingleThreaded(scene,
                             light_sampler,
                             "test_results/teapot_smooth.pfm");

    for (size_t i = 0; i < triangles_allocated; i++)
    {
        ShapeRelease(shapes[i]);
    }

    SpectrumRelease(spectrum);
    ReflectorRelease(reflector);
    BsdfRelease(bsdf);
    NormalMapRelease(front_normal_map);
    MaterialRelease(material);
    LightRelease(light);
    SceneRelease(scene);
    LightSamplerRelease(light_sampler);
}