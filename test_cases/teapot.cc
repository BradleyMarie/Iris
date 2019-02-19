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
#include "iris_physx_toolkit/lambertian_bsdf.h"
#include "iris_physx_toolkit/list_scene.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/point_light.h"
#include "iris_physx_toolkit/sample_tracer.h"
#include "iris_physx_toolkit/triangle.h"
#include "googletest/include/gtest/gtest.h"
#include "test_util/teapot.h"
#include "test_util/pfm.h"
#include "test_util/spectra.h"

//
// Smooth Material
//

typedef struct _SMOOTH_MATERIAL {
    PBSDF bsdf;
    VECTOR3 normals[3];
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
    PCTRIANGLE_ADDITIONAL_DATA smooth_data =
        (PCTRIANGLE_ADDITIONAL_DATA)additional_data;

    *world_shading_normal = VectorScale(smooth_material->normals[0], 
                                        smooth_data->barycentric_coordinates[0]);

    *world_shading_normal = VectorAddScaled(*world_shading_normal,
                                            smooth_material->normals[1], 
                                            smooth_data->barycentric_coordinates[1]);

    *world_shading_normal = VectorAddScaled(*world_shading_normal,
                                            smooth_material->normals[2], 
                                            smooth_data->barycentric_coordinates[2]);

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
    _In_ VECTOR3 normal0,
    _In_ VECTOR3 normal1,
    _In_ VECTOR3 normal2,
    _In_ PBSDF bsdf,
    _Out_ PMATERIAL *material
    )
{
    ASSERT_TRUE(VectorValidate(normal0));
    ASSERT_TRUE(VectorValidate(normal1));
    ASSERT_TRUE(VectorValidate(normal2));
    ASSERT_TRUE(bsdf);
    ASSERT_TRUE(material);

    SMOOTH_MATERIAL smooth_material;
    smooth_material.normals[0] = normal0;
    smooth_material.normals[1] = normal1;
    smooth_material.normals[2] = normal2;
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
    status = FramebufferAllocate(256, 256, &framebuffer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = IrisCameraRender(camera,
                              pixel_sampler,
                              sample_tracer,
                              rng,
                              framebuffer,
                              (float_t)0.01,
                              std::thread::hardware_concurrency());
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

    PBSDF bsdf;
    status = LambertianReflectorAllocate(reflector, &bsdf);
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
    status = ConstantMaterialAllocate(bsdf, &material);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    for (size_t i = 0; i < TEAPOT_FACE_COUNT; i++)
    {
        PSHAPE shape;
        ISTATUS status = TriangleAllocate(
            teapot_vertices[teapot_faces[i].vertex0],
            teapot_vertices[teapot_faces[i].vertex1],
            teapot_vertices[teapot_faces[i].vertex2],
            material,
            nullptr,
            &shape);
        ASSERT_EQ(status, ISTATUS_SUCCESS);

        status = ListSceneAddTransformedShape(scene, shape, nullptr);
        ASSERT_EQ(status, ISTATUS_SUCCESS);

        ShapeRelease(shape);
    }

    TestRenderSingleThreaded(scene,
                             light_sampler,
                             "test_results/teapot_flat.pfm");

    SpectrumRelease(spectrum);
    ReflectorRelease(reflector);
    BsdfRelease(bsdf);
    MaterialRelease(material);
    LightRelease(light);
    ListSceneFree(scene);
    AllLightSamplerFree(light_sampler);
}

TEST(TeapotTest, SmoothShadedTeapot)
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

    PBSDF bsdf;
    status = LambertianReflectorAllocate(reflector, &bsdf);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PLIGHT light;
    status = PointLightAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-5.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = AllLightSamplerAddLight(light_sampler, light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    for (size_t i = 0; i < TEAPOT_FACE_COUNT; i++)
    {
        VECTOR3 normal0 = teapot_normals[teapot_faces[i].normal0];
        normal0 = VectorNormalize(normal0, nullptr, nullptr);
        VECTOR3 normal1 = teapot_normals[teapot_faces[i].normal1];
        normal1 = VectorNormalize(normal1, nullptr, nullptr);
        VECTOR3 normal2 = teapot_normals[teapot_faces[i].normal2];
        normal2 = VectorNormalize(normal2, nullptr, nullptr);
        PMATERIAL material;
        SmoothMaterialAllocate(
            normal0,
            normal1,
            normal2,
            bsdf,
            &material);

        PSHAPE shape;
        ISTATUS status = TriangleAllocate(
            teapot_vertices[teapot_faces[i].vertex0],
            teapot_vertices[teapot_faces[i].vertex1],
            teapot_vertices[teapot_faces[i].vertex2],
            material,
            nullptr,
            &shape);
        ASSERT_EQ(status, ISTATUS_SUCCESS);

        status = ListSceneAddTransformedShape(scene, shape, nullptr);
        ASSERT_EQ(status, ISTATUS_SUCCESS);

        MaterialRelease(material);
        ShapeRelease(shape);
    }

    TestRenderSingleThreaded(scene,
                             light_sampler,
                             "test_results/teapot_smooth.pfm");

    SpectrumRelease(spectrum);
    ReflectorRelease(reflector);
    BsdfRelease(bsdf);
    LightRelease(light);
    ListSceneFree(scene);
    AllLightSamplerFree(light_sampler);
}