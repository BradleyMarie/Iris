/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

   teapot.cc

Abstract:

   Integration tests which render the Utah teapot.

--*/

#include <string>
#include <thread>
#include <vector>

#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_camera_toolkit/grid_pixel_sampler.h"
#include "iris_camera_toolkit/pinhole_camera.h"
#include "iris_physx_toolkit/all_light_sampler.h"
#include "iris_physx_toolkit/attenuated_reflector.h"
#include "iris_physx_toolkit/constant_material.h"
#include "iris_physx_toolkit/lambertian_bsdf.h"
#include "iris_physx_toolkit/kd_tree_scene.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/point_light.h"
#include "iris_physx_toolkit/sample_tracer.h"
#include "iris_physx_toolkit/triangle_mesh.h"
#include "googletest/include/gtest/gtest.h"
#include "test_util/teapot.h"
#include "test_util/pfm.h"
#include "test_util/spectra.h"

//
// Smooth Material
//

typedef struct _SMOOTH_MATERIAL {
    PBSDF bsdf;
} SMOOTH_MATERIAL, *PSMOOTH_MATERIAL;

typedef const SMOOTH_MATERIAL *PCSMOOTH_MATERIAL;

static
ISTATUS
SmoothMaterialSample(
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
    PCSMOOTH_MATERIAL smooth_material = (PCSMOOTH_MATERIAL)context;
    PCTRIANGLE_MESH_ADDITIONAL_DATA triangle_data =
        (PCTRIANGLE_MESH_ADDITIONAL_DATA)additional_data;

    const size_t* normals = teapot_face_normals[triangle_data->mesh_face_index];

    *world_shading_normal = VectorScale(teapot_normals[normals[0]],
                                        triangle_data->barycentric_coordinates[0]);

    *world_shading_normal = VectorAddScaled(*world_shading_normal,
                                            teapot_normals[normals[1]],
                                            triangle_data->barycentric_coordinates[1]);

    *world_shading_normal = VectorAddScaled(*world_shading_normal,
                                            teapot_normals[normals[2]],
                                            triangle_data->barycentric_coordinates[2]);

    *world_shading_normal = VectorNormalize(*world_shading_normal,
                                            nullptr,
                                            nullptr);

    *bsdf = smooth_material->bsdf;

    return ISTATUS_SUCCESS;
}

static
void
SmoothMaterialFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PSMOOTH_MATERIAL smooth_material = (PSMOOTH_MATERIAL)context;

    BsdfRelease(smooth_material->bsdf);
}

static const MATERIAL_VTABLE smooth_material_vtable = {
    SmoothMaterialSample,
    SmoothMaterialFree
};

void
SmoothMaterialAllocate(
    _In_ PBSDF bsdf,
    _Out_ PMATERIAL *material
    )
{
    ASSERT_TRUE(bsdf);
    ASSERT_TRUE(material);

    SMOOTH_MATERIAL smooth_material;
    smooth_material.bsdf = bsdf;

    ISTATUS status = MaterialAllocate(&smooth_material_vtable,
                                      &smooth_material,
                                      sizeof(SMOOTH_MATERIAL),
                                      alignof(SMOOTH_MATERIAL),
                                      material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    BsdfRetain(bsdf);
}

//
// Tests
//

void
TestRenderSingleThreaded(
    _In_ PCSCENE scene,
    _In_ PCLIGHT_SAMPLER light_sampler,
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
        scene,
        light_sampler,
        color_integrator,
        &sample_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PFRAMEBUFFER framebuffer;
    status = FramebufferAllocate(256, 256, &framebuffer);
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
                                        PFM_PIXEL_FORMAT_XYZ,
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

TEST(TeapotTest, FlatShadedTeapot)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate((float_t)32.0,
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

    PBSDF bsdf;
    status = LambertianReflectorAllocate(reflector, &bsdf);
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
        teapot_normals,
        TEAPOT_VERTEX_COUNT,
        teapot_face_vertices,
        TEAPOT_FACE_COUNT,
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
    SceneFree(scene);
    LightSamplerFree(light_sampler);
}

TEST(TeapotTest, SmoothShadedTeapot)
{
    PSPECTRUM spectrum;
    ISTATUS status = TestSpectrumAllocate((float_t)32.0,
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

    PBSDF bsdf;
    status = LambertianReflectorAllocate(reflector, &bsdf);
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
    SmoothMaterialAllocate(bsdf, &material);

    PSHAPE shapes[TEAPOT_FACE_COUNT] = { nullptr };
    PMATRIX transforms[TEAPOT_FACE_COUNT] = { nullptr };
    bool premultiplied[TEAPOT_FACE_COUNT] = { false };

    size_t triangles_allocated;
    status = TriangleMeshAllocate(
        teapot_vertices,
        teapot_normals,
        TEAPOT_VERTEX_COUNT,
        teapot_face_vertices,
        TEAPOT_FACE_COUNT,
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
    MaterialRelease(material);
    LightRelease(light);
    SceneFree(scene);
    LightSamplerFree(light_sampler);
}