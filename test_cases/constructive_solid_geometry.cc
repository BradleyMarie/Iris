/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    constructive_solid_geometry.cc

Abstract:

    Integration tests which render CSG shapes.

--*/

#include <string>

#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_camera_toolkit/grid_pixel_sampler.h"
#include "iris_camera_toolkit/pinhole_camera.h"
#include "iris_physx_toolkit/all_light_sampler.h"
#include "iris_physx_toolkit/constant_material.h"
#include "iris_physx_toolkit/constructive_solid_geometry.h"
#include "iris_physx_toolkit/lambertian_bsdf.h"
#include "iris_physx_toolkit/list_scene.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/point_light.h"
#include "iris_physx_toolkit/sample_tracer.h"
#include "iris_physx_toolkit/sphere.h"
#include "googletest/include/gtest/gtest.h"
#include "test_util/pfm.h"
#include "test_util/quad.h"
#include "test_util/spectra.h"

void
AllocateCubeFace(
    _In_ POINT3 v0,
    _In_ POINT3 v1,
    _In_ POINT3 v2,
    _In_ POINT3 v3,
    _In_opt_ PMATERIAL material,
    _Out_ PSHAPE *cube
    )
{
    PSHAPE shape0, shape1;
    ISTATUS status = EmissiveQuadAllocate(
        v0,
        v1,
        v2,
        v3,
        material,
        material,
        nullptr,
        nullptr,
        &shape0,
        &shape1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = UnionAllocate(shape0, shape1, cube);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    ShapeRelease(shape0);
    ShapeRelease(shape1);
}

void
AllocateCubeFromTriangles(
    _In_ PMATERIAL material,
    _Out_ PSHAPE *cube
    )
{
    POINT3 points[] = {
        PointCreate((float_t)-0.25, (float_t)-0.25, (float_t)-0.25),
        PointCreate((float_t)0.25, (float_t)-0.25, (float_t)-0.25),
        PointCreate((float_t)-0.25, (float_t)0.25, (float_t)-0.25),
        PointCreate((float_t)-0.25, (float_t)-0.25, (float_t)0.25),
        PointCreate((float_t)0.25, (float_t)0.25, (float_t)-0.25),
        PointCreate((float_t)-0.25, (float_t)0.25, (float_t)0.25),
        PointCreate((float_t)0.25, (float_t)-0.25, (float_t)0.25),
        PointCreate((float_t)0.25, (float_t)0.25, (float_t)0.25),
    };

    // Front
    PSHAPE shape0;
    AllocateCubeFace(points[0],
                     points[1],
                     points[4],
                     points[2],
                     material,
                     &shape0);

    // Back
    PSHAPE shape1;
    AllocateCubeFace(points[3],
                     points[6],
                     points[7],
                     points[5],
                     material,
                     &shape1);

    ISTATUS status = UnionAllocate(shape0, shape1, cube);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    ShapeRelease(shape0);
    ShapeRelease(shape1);

    // Left
    AllocateCubeFace(points[0],
                     points[2],
                     points[5],
                     points[3],
                     material,
                     &shape0);

    status = UnionAllocate(shape0, *cube, cube);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    ShapeRelease(shape0);

    // Right
    AllocateCubeFace(points[1],
                     points[4],
                     points[7],
                     points[6],
                     material,
                     &shape0);

    status = UnionAllocate(shape0, *cube, cube);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    ShapeRelease(shape0);

    // Bottom
    AllocateCubeFace(points[0],
                     points[1],
                     points[6],
                     points[3],
                     material,
                     &shape0);

    status = UnionAllocate(shape0, *cube, cube);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    ShapeRelease(shape0);

    // Top
    AllocateCubeFace(points[2],
                     points[4],
                     points[7],
                     points[5],
                     material,
                     &shape0);

    status = UnionAllocate(shape0, *cube, cube);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    ShapeRelease(shape0);
}

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

    PPIXEL_SAMPLER pixel_sampler;
    status =
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
    status = FramebufferAllocate(500, 500, &framebuffer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = IrisCameraRenderSingleThreaded(camera,
                                            nullptr,
                                            pixel_sampler,
                                            sample_tracer,
                                            rng,
                                            framebuffer,
                                            (float_t)0.001);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    bool equals;
    status = ApproximatelyEqualsPfmFile(framebuffer,
                                        file_name.c_str(),
                                        PFM_PIXEL_FORMAT_XYZ,
                                        (float_t)0.01,
                                        &equals);
    ASSERT_EQ(status, ISTATUS_SUCCESS);
    EXPECT_TRUE(equals);

    CameraFree(camera);
    PixelSamplerFree(pixel_sampler);
    RandomFree(rng);
    SampleTracerFree(sample_tracer);
    FramebufferFree(framebuffer);
    ColorIntegratorRelease(color_integrator);
}

TEST(ConstructiveSolidGeometryTest, CubeSphereDifference)
{
    PSPECTRUM spectrum;
    ISTATUS status = XyzSpectrumAllocate((float_t)0.0,
                                         (float_t)1.0,
                                         (float_t)1.0,
                                         &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = XyzReflectorAllocate((float_t)0.0,
                                  (float_t)1.0,
                                  (float_t)0.0,
                                  &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = XyzReflectorAllocate((float_t)0.0,
                                  (float_t)0.0,
                                  (float_t)1.0,
                                  &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF bsdf0;
    status = LambertianReflectorAllocate(reflector0, &bsdf0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF bsdf1;
    status = LambertianReflectorAllocate(reflector1, &bsdf1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = PointLightAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material0;
    status = ConstantMaterialAllocate(bsdf0, &material0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material1;
    status = ConstantMaterialAllocate(bsdf1, &material1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape0;
    AllocateCubeFromTriangles(material0, &shape0);

    PSHAPE shape1;
    status = SphereAllocate(
        PointCreate((float_t)0.25, (float_t)0.25, (float_t)-0.25),
        (float_t)0.25,
        material1,
        material1,
        &shape1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape2;
    status = DifferenceAllocate(shape0, shape1, &shape2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATRIX matrix = nullptr;
    bool premultiplied = false;

    PSCENE scene;
    status = ListSceneAllocate(&shape2,
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
                             "test_results/csg_cube_sphere_difference.pfm");

    SceneRelease(scene);
    LightSamplerRelease(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    LightRelease(light);
    BsdfRelease(bsdf0);
    BsdfRelease(bsdf1);
    MaterialRelease(material0);
    MaterialRelease(material1);
    ShapeRelease(shape0);
    ShapeRelease(shape1);
    ShapeRelease(shape2);
}

TEST(ConstructiveSolidGeometryTest, SphereIntersection)
{
    PSPECTRUM spectrum;
    ISTATUS status = XyzSpectrumAllocate((float_t)0.0,
                                         (float_t)1.0,
                                         (float_t)1.0,
                                         &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = XyzReflectorAllocate((float_t)0.0,
                                  (float_t)1.0,
                                  (float_t)0.0,
                                  &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = XyzReflectorAllocate((float_t)0.0,
                                  (float_t)0.0,
                                  (float_t)1.0,
                                  &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF bsdf0;
    status = LambertianReflectorAllocate(reflector0, &bsdf0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF bsdf1;
    status = LambertianReflectorAllocate(reflector1, &bsdf1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = PointLightAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material0;
    status = ConstantMaterialAllocate(bsdf0, &material0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material1;
    status = ConstantMaterialAllocate(bsdf1, &material1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape0;
    status = SphereAllocate(
        PointCreate((float_t)-0.25, (float_t)0.0, (float_t)0.0),
        (float_t)0.5,
        material0,
        material0,
        &shape0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape1;
    status = SphereAllocate(
        PointCreate((float_t)0.25, (float_t)0.0, (float_t)0.0),
        (float_t)0.5,
        material1,
        material1,
        &shape1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape2;
    status = IntersectionAllocate(shape0, shape1, &shape2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATRIX matrix = nullptr;
    bool premultiplied = false;

    PSCENE scene;
    status = ListSceneAllocate(&shape2,
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
                             "test_results/csg_sphere_intersection.pfm");

    SceneRelease(scene);
    LightSamplerRelease(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    LightRelease(light);
    BsdfRelease(bsdf0);
    BsdfRelease(bsdf1);
    MaterialRelease(material0);
    MaterialRelease(material1);
    ShapeRelease(shape0);
    ShapeRelease(shape1);
    ShapeRelease(shape2);
}

TEST(ConstructiveSolidGeometryTest, SphereUnion)
{
    PSPECTRUM spectrum;
    ISTATUS status = XyzSpectrumAllocate((float_t)0.0,
                                         (float_t)1.0,
                                         (float_t)1.0,
                                         &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = XyzReflectorAllocate((float_t)0.0,
                                  (float_t)1.0,
                                  (float_t)0.0,
                                  &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = XyzReflectorAllocate((float_t)0.0,
                                  (float_t)0.0,
                                  (float_t)1.0,
                                  &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF bsdf0;
    status = LambertianReflectorAllocate(reflector0, &bsdf0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF bsdf1;
    status = LambertianReflectorAllocate(reflector1, &bsdf1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = PointLightAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material0;
    status = ConstantMaterialAllocate(bsdf0, &material0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material1;
    status = ConstantMaterialAllocate(bsdf1, &material1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape0;
    status = SphereAllocate(
        PointCreate((float_t)-0.25, (float_t)0.0, (float_t)0.0),
        (float_t)0.5,
        material0,
        material0,
        &shape0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape1;
    status = SphereAllocate(
        PointCreate((float_t)0.25, (float_t)0.0, (float_t)0.0),
        (float_t)0.5,
        material1,
        material1,
        &shape1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape2;
    status = UnionAllocate(shape0, shape1, &shape2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATRIX matrix = nullptr;
    bool premultiplied = false;

    PSCENE scene;
    status = ListSceneAllocate(&shape2,
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
                             "test_results/csg_sphere_union.pfm");

    SceneRelease(scene);
    LightSamplerRelease(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    LightRelease(light);
    BsdfRelease(bsdf0);
    BsdfRelease(bsdf1);
    MaterialRelease(material0);
    MaterialRelease(material1);
    ShapeRelease(shape0);
    ShapeRelease(shape1);
    ShapeRelease(shape2);
}

TEST(ConstructiveSolidGeometryTest, RoundedCube)
{
    PSPECTRUM spectrum;
    ISTATUS status = XyzSpectrumAllocate((float_t)0.0,
                                         (float_t)1.0,
                                         (float_t)1.0,
                                         &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = XyzReflectorAllocate((float_t)0.0,
                                  (float_t)1.0,
                                  (float_t)0.0,
                                  &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = XyzReflectorAllocate((float_t)0.0,
                                  (float_t)0.0,
                                  (float_t)1.0,
                                  &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF bsdf0;
    status = LambertianReflectorAllocate(reflector0, &bsdf0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PBSDF bsdf1;
    status = LambertianReflectorAllocate(reflector1, &bsdf1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = PointLightAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material0;
    status = ConstantMaterialAllocate(bsdf0, &material0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material1;
    status = ConstantMaterialAllocate(bsdf1, &material1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape0;
    AllocateCubeFromTriangles(material0, &shape0);

    PSHAPE shape1;
    status = SphereAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        (float_t)0.37,
        material1,
        material1,
        &shape1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape2;
    status = IntersectionAllocate(shape0, shape1, &shape2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    const float_t pi = (float_t)3.1415926535897932384626433832;

    PMATRIX matrix0;
    status = MatrixAllocateRotation(
        pi / (float_t)4.0, (float_t)0.0, (float_t)1.0, (float_t)0.0, &matrix0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATRIX matrix1;
    status = MatrixAllocateRotation(
        pi / (float_t)5.0, (float_t)1.0, (float_t)0.0, (float_t)1.0, &matrix1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATRIX matrix2;
    status = MatrixAllocateProduct(matrix0, matrix1, &matrix2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    bool premultiplied = false;

    PSCENE scene;
    status = ListSceneAllocate(&shape2,
                               &matrix2,
                               &premultiplied,
                               1,
                               &scene);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT_SAMPLER light_sampler;
    status = AllLightSamplerAllocate(&light, 1, &light_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRenderSingleThreaded(scene,
                             light_sampler,
                             "test_results/csg_rounded_cube.pfm");

    SceneRelease(scene);
    LightSamplerRelease(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    LightRelease(light);
    BsdfRelease(bsdf0);
    BsdfRelease(bsdf1);
    MaterialRelease(material0);
    MaterialRelease(material1);
    ShapeRelease(shape0);
    ShapeRelease(shape1);
    ShapeRelease(shape2);
    MatrixRelease(matrix0);
    MatrixRelease(matrix1);
    MatrixRelease(matrix2);
}