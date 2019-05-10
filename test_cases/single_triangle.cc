/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    single_triangle.cc

Abstract:

    Integration tests which render a single triangle.

--*/

#include <string>
#include <thread>

#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_camera_toolkit/grid_pixel_sampler.h"
#include "iris_camera_toolkit/orthographic_camera.h"
#include "iris_physx_toolkit/all_light_sampler.h"
#include "iris_physx_toolkit/attenuated_reflector.h"
#include "iris_physx_toolkit/constant_emissive_material.h"
#include "iris_physx_toolkit/directional_light.h"
#include "iris_physx_toolkit/lambertian_bsdf.h"
#include "iris_physx_toolkit/list_scene.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/sample_tracer.h"
#include "iris_physx_toolkit/triangle.h"
#include "googletest/include/gtest/gtest.h"
#include "test_util/pfm.h"
#include "test_util/spectra.h"

//
// Triangle Material
//

typedef struct _TRIANGLE_MATERIAL {
    PREFLECTOR reflectors[3];
} TRIANGLE_MATERIAL, *PTRIANGLE_MATERIAL;

typedef const TRIANGLE_MATERIAL *PCTRIANGLE_MATERIAL;

static
ISTATUS
TriangleMaterialSample(
    _In_ const void *context,
    _In_ POINT3 model_hit_point,
    _In_ VECTOR3 world_surface_normal,
    _In_ const void *additional_data,
    _Inout_ PBSDF_ALLOCATOR bsdf_allocator,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PVECTOR3 world_shading_normal,
    _Out_ PCBSDF *bsdf
    )
{
    PCTRIANGLE_MATERIAL triangle_material = (PCTRIANGLE_MATERIAL)context;
    PCTRIANGLE_ADDITIONAL_DATA triangle_data =
        (PCTRIANGLE_ADDITIONAL_DATA)additional_data;

    PCREFLECTOR reflector;
    ISTATUS status =
        AttenuatedReflector3AllocateWithAllocator(reflector_compositor,
                                                  triangle_material->reflectors[0],
                                                  triangle_data->barycentric_coordinates[0],
                                                  triangle_material->reflectors[1],
                                                  triangle_data->barycentric_coordinates[1],
                                                  triangle_material->reflectors[2],
                                                  triangle_data->barycentric_coordinates[2],
                                                  &reflector);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = LambertianReflectorAllocateWithAllocator(bsdf_allocator,
                                                      reflector,
                                                      bsdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *world_shading_normal = world_surface_normal;

    return ISTATUS_SUCCESS;
}

static
void
TriangleMaterialFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PTRIANGLE_MATERIAL triangle_material = (PTRIANGLE_MATERIAL)context;

    ReflectorRelease(triangle_material->reflectors[0]);
    ReflectorRelease(triangle_material->reflectors[1]);
    ReflectorRelease(triangle_material->reflectors[2]);
}

static const MATERIAL_VTABLE triangle_material_vtable = {
    TriangleMaterialSample,
    TriangleMaterialFree
};

ISTATUS
TriangleMaterialAllocate(
    _In_ PREFLECTOR reflector0,
    _In_ PREFLECTOR reflector1,
    _In_ PREFLECTOR reflector2,
    _Out_ PMATERIAL *material
    )
{
    if (reflector0 == nullptr)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflector1 == nullptr)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (reflector2 == nullptr)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (material == nullptr)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    TRIANGLE_MATERIAL triangle_material;
    triangle_material.reflectors[0] = reflector0;
    triangle_material.reflectors[1] = reflector1;
    triangle_material.reflectors[2] = reflector2;

    ISTATUS status = MaterialAllocate(&triangle_material_vtable,
                                      &triangle_material,
                                      sizeof(TRIANGLE_MATERIAL),
                                      alignof(TRIANGLE_MATERIAL),
                                      material);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ReflectorRetain(reflector0);
    ReflectorRetain(reflector1);
    ReflectorRetain(reflector2);

    return ISTATUS_SUCCESS;
}

//
// Test Functions
//

void
TestRender(
    _In_ PCCAMERA camera,
    _In_ PCSCENE scene,
    _In_ PCLIGHT_SAMPLER light_sampler,
    _In_ const std::string& file_name,
    _In_ float_t epsilon
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

    PCOLOR_INTEGRATOR color_integrator;
    status = TestColorIntegratorAllocate(&color_integrator);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSAMPLE_TRACER sample_tracer;
    status = PhysxSampleTracerAllocate(
        path_tracer,
        scene,
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
                              (float_t)0.001,
                              1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    bool equals;
    status = ApproximatelyEqualsPfmFile(framebuffer,
                                        file_name.c_str(),
                                        PFM_PIXEL_FORMAT_XYZ,
                                        epsilon,
                                        &equals);
    ASSERT_EQ(status, ISTATUS_SUCCESS);
    EXPECT_TRUE(equals);

    PixelSamplerFree(pixel_sampler);
    RandomFree(rng);
    SampleTracerFree(sample_tracer);
    FramebufferFree(framebuffer);
    ColorIntegratorFree(color_integrator);
}

//
// Test Data
//

static const float_t pi = (float_t)3.1415926535897932384626433832;

//
// Tests
//

TEST(SingleTriangleTest, TestXYTriangleFrontWithMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)1.001, (float_t)0.0),
        PointCreate((float_t)1.001, (float_t)0.0, (float_t)0.0),
        material,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_triangle_xy_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestXYTriangleFrontNoMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)1.001, (float_t)0.0),
        PointCreate((float_t)1.001, (float_t)0.0, (float_t)0.0),
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestXYTriangleBackWithMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)1.001, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)1.001, (float_t)0.0),
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_triangle_xy_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestXYTriangleBackNoMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)1.001, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)1.001, (float_t)0.0),
        material,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestXYTriangleBehind)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-4.0),
        PointCreate((float_t)0.0, (float_t)1.0, (float_t)-4.0),
        PointCreate((float_t)1.0, (float_t)0.0, (float_t)-4.0),
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestXZTriangleFrontWithMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)-1.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        PointCreate((float_t)-1.001, (float_t)0.0, (float_t)0.0),
        material,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_triangle_xz_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestXZTriangleFrontNoMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)-1.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        PointCreate((float_t)-1.001, (float_t)0.0, (float_t)0.0),
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestXZTriangleBackWithMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)-1.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)-1.001, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_triangle_xz_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestXZTriangleBackNoMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)-1.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)-1.001, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        material,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestXZTriangleBehind)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)-1.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)-4.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)-4.0, (float_t)1.0),
        PointCreate((float_t)-1.0, (float_t)-4.0, (float_t)0.0),
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestYZTriangleFrontWithMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)-1.0, (float_t)0.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)-1.001, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        material,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_triangle_yz_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestYZTriangleFrontNoMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)-1.0, (float_t)0.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)-1.001, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestYZTriangleBackWithMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)-1.0, (float_t)0.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        PointCreate((float_t)0.0, (float_t)-1.001, (float_t)0.0),
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_triangle_yz_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestYZTriangleBackNoMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)-1.0, (float_t)0.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        PointCreate((float_t)0.0, (float_t)-1.001, (float_t)0.0),
        material,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleTriangleTest, TestYZTriangleBehind)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)-1.0, (float_t)0.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)-4.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)-4.0, (float_t)-1.0, (float_t)0.0),
        PointCreate((float_t)-4.0, (float_t)0.0, (float_t)1.0),
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXYTriangleFrontWithMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)1.001, (float_t)0.0),
        PointCreate((float_t)1.001, (float_t)0.0, (float_t)0.0),
        material,
        nullptr,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_triangle_xy_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXYTriangleFrontNoMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)1.001, (float_t)0.0),
        PointCreate((float_t)1.001, (float_t)0.0, (float_t)0.0),
        nullptr,
        material,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXYTriangleBackWithMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)1.001, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)1.001, (float_t)0.0),
        nullptr,
        material,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_triangle_xy_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXYTriangleBackNoMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)1.001, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)1.001, (float_t)0.0),
        material,
        nullptr,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXYTriangleBehind)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-4.0),
        PointCreate((float_t)0.0, (float_t)1.0, (float_t)-4.0),
        PointCreate((float_t)1.0, (float_t)0.0, (float_t)-4.0),
        material,
        material,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXYTriangleFrontWithLight)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate((float_t)1.0,
                                  (float_t)1.0,
                                  (float_t)1.0,
                                  &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL emissive_material;
    status = ConstantEmissiveMaterialAllocate(spectrum, &emissive_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)1.001, (float_t)0.0),
        PointCreate((float_t)1.001, (float_t)0.0, (float_t)0.0),
        nullptr,
        nullptr,
        emissive_material,
        nullptr,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = AreaLightAllocate(
        shape,
        TRIANGLE_FRONT_FACE,
        &light);
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_emissive_triangle_xy_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    LightRelease(light);
    EmissiveMaterialRelease(emissive_material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXYTriangleFrontNoLight)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL emissive_material;
    status = ConstantEmissiveMaterialAllocate(spectrum, &emissive_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)1.001, (float_t)0.0),
        PointCreate((float_t)1.001, (float_t)0.0, (float_t)0.0),
        nullptr,
        nullptr,
        nullptr,
        emissive_material,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = AreaLightAllocate(
        shape,
        TRIANGLE_BACK_FACE,
        &light);
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    LightRelease(light);
    EmissiveMaterialRelease(emissive_material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXYTriangleBackWithLight)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate((float_t)1.0,
                                  (float_t)1.0,
                                  (float_t)1.0,
                                  &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL emissive_material;
    status = ConstantEmissiveMaterialAllocate(spectrum, &emissive_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)1.001, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)1.001, (float_t)0.0),
        nullptr,
        nullptr,
        nullptr,
        emissive_material,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = AreaLightAllocate(
        shape,
        TRIANGLE_BACK_FACE,
        &light);
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_emissive_triangle_xy_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    LightRelease(light);
    EmissiveMaterialRelease(emissive_material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXYTriangleBackNoLight)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL emissive_material;
    status = ConstantEmissiveMaterialAllocate(spectrum, &emissive_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)1.001, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)1.001, (float_t)0.0),
        nullptr,
        nullptr,
        emissive_material,
        nullptr,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = AreaLightAllocate(
        shape,
        TRIANGLE_FRONT_FACE,
        &light);
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-3.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    LightRelease(light);
    EmissiveMaterialRelease(emissive_material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXZTriangleFrontWithMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)-1.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        PointCreate((float_t)-1.001, (float_t)0.0, (float_t)0.0),
        material,
        nullptr,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_triangle_xz_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXZTriangleFrontNoMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)-1.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        PointCreate((float_t)-1.001, (float_t)0.0, (float_t)0.0),
        nullptr,
        material,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXZTriangleBackWithMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)-1.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)-1.001, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        nullptr,
        material,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_triangle_xz_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXZTriangleBackNoMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)-1.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)-1.001, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        material,
        nullptr,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXZTriangleFrontWithLight)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate((float_t)1.0,
                                  (float_t)1.0,
                                  (float_t)1.0,
                                  &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL emissive_material;
    status = ConstantEmissiveMaterialAllocate(spectrum, &emissive_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        PointCreate((float_t)-1.001, (float_t)0.0, (float_t)0.0),
        nullptr,
        nullptr,
        emissive_material,
        nullptr,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = AreaLightAllocate(
        shape,
        TRIANGLE_FRONT_FACE,
        &light);
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_emissive_triangle_xz_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    LightRelease(light);
    EmissiveMaterialRelease(emissive_material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXZTriangleFrontNoLight)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL emissive_material;
    status = ConstantEmissiveMaterialAllocate(spectrum, &emissive_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        PointCreate((float_t)-1.001, (float_t)0.0, (float_t)0.0),
        nullptr,
        nullptr,
        nullptr,
        emissive_material,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = AreaLightAllocate(
        shape,
        TRIANGLE_BACK_FACE,
        &light);
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    LightRelease(light);
    EmissiveMaterialRelease(emissive_material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXZTriangleBackWithLight)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate((float_t)1.0,
                                  (float_t)1.0,
                                  (float_t)1.0,
                                  &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL emissive_material;
    status = ConstantEmissiveMaterialAllocate(spectrum, &emissive_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)-1.001, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        nullptr,
        nullptr,
        nullptr,
        emissive_material,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = AreaLightAllocate(
        shape,
        TRIANGLE_BACK_FACE,
        &light);
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_emissive_triangle_xz_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    LightRelease(light);
    EmissiveMaterialRelease(emissive_material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXZTriangleBackNoLight)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL emissive_material;
    status = ConstantEmissiveMaterialAllocate(spectrum, &emissive_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)-1.001, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        nullptr,
        nullptr,
        emissive_material,
        nullptr,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = AreaLightAllocate(
        shape,
        TRIANGLE_FRONT_FACE,
        &light);
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    LightRelease(light);
    EmissiveMaterialRelease(emissive_material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestXZTriangleBehind)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)0.0, (float_t)-1.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)-4.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)-4.0, (float_t)1.0),
        PointCreate((float_t)-1.0, (float_t)-4.0, (float_t)0.0),
        material,
        material,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)-3.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestYZTriangleFrontWithMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)-1.0, (float_t)0.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)-1.001, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        material,
        nullptr,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_triangle_yz_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestYZTriangleFrontNoMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)-1.0, (float_t)0.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)-1.001, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        nullptr,
        material,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestYZTriangleBackWithMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)-1.0, (float_t)0.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        PointCreate((float_t)0.0, (float_t)-1.001, (float_t)0.0),
        nullptr,
        material,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_triangle_yz_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestYZTriangleBackNoMaterial)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)-1.0, (float_t)0.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        PointCreate((float_t)0.0, (float_t)-1.001, (float_t)0.0),
        material,
        nullptr,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestYZTriangleBehind)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector0;
    status = TestReflectorAllocate((float_t)1.0,
                                   (float_t)0.0,
                                   (float_t)0.0,
                                   &reflector0);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector1;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)1.0,
                                   (float_t)0.0,
                                   &reflector1);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PREFLECTOR reflector2;
    status = TestReflectorAllocate((float_t)0.0,
                                   (float_t)0.0,
                                   (float_t)1.0,
                                   &reflector2);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PMATERIAL material;
    status = TriangleMaterialAllocate(reflector0,
                                      reflector1,
                                      reflector2,
                                      &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = DirectionalLightAllocate(
        VectorCreate((float_t)-1.0, (float_t)0.0, (float_t)0.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)-4.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)-4.0, (float_t)-1.0, (float_t)0.0),
        PointCreate((float_t)-4.0, (float_t)0.0, (float_t)1.0),
        material,
        material,
        nullptr,
        nullptr,
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestYZTriangleFrontWithLight)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate((float_t)1.0,
                                  (float_t)1.0,
                                  (float_t)1.0,
                                  &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL emissive_material;
    status = ConstantEmissiveMaterialAllocate(spectrum, &emissive_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)-1.001, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        nullptr,
        nullptr,
        emissive_material,
        nullptr,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = AreaLightAllocate(
        shape,
        TRIANGLE_FRONT_FACE,
        &light);
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_emissive_triangle_yz_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    LightRelease(light);
    EmissiveMaterialRelease(emissive_material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestYZTriangleFrontNoLight)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL emissive_material;
    status = ConstantEmissiveMaterialAllocate(spectrum, &emissive_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)-1.001, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        nullptr,
        nullptr,
        nullptr,
        emissive_material,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = AreaLightAllocate(
        shape,
        TRIANGLE_BACK_FACE,
        &light);
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    LightRelease(light);
    EmissiveMaterialRelease(emissive_material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestYZTriangleBackWithLight)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate((float_t)1.0,
                                  (float_t)1.0,
                                  (float_t)1.0,
                                  &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL emissive_material;
    status = ConstantEmissiveMaterialAllocate(spectrum, &emissive_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        PointCreate((float_t)0.0, (float_t)-1.001, (float_t)0.0),
        nullptr,
        nullptr,
        nullptr,
        emissive_material,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = AreaLightAllocate(
        shape,
        TRIANGLE_BACK_FACE,
        &light);
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/single_emissive_triangle_yz_triangle.pfm",
               (float_t)0.01);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    LightRelease(light);
    EmissiveMaterialRelease(emissive_material);
    ShapeRelease(shape);
    CameraFree(camera);
}

TEST(SingleEmissiveTriangleTest, TestYZTriangleBackNoLight)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate(pi, pi, pi, &spectrum);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PEMISSIVE_MATERIAL emissive_material;
    status = ConstantEmissiveMaterialAllocate(spectrum, &emissive_material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = EmissiveTriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)-1.001, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)1.001),
        nullptr,
        nullptr,
        emissive_material,
        nullptr,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = AreaLightAllocate(
        shape,
        TRIANGLE_FRONT_FACE,
        &light);
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

    PCAMERA camera;
    status = OrthographicCameraAllocate(
        PointCreate((float_t)-3.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRender(camera,
               scene,
               light_sampler,
               "test_results/blank.pfm",
               (float_t)0.0);

    SceneFree(scene);
    LightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    LightRelease(light);
    EmissiveMaterialRelease(emissive_material);
    ShapeRelease(shape);
    CameraFree(camera);
}