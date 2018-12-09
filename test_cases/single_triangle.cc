/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    single_triangle.cc

Abstract:

    Integration tests which render a single triangle.

--*/

#include <string>

#include "iris_advanced_toolkit/pcg_random.h"
#include "iris_camera_toolkit/grid_pixel_sampler.h"
#include "iris_camera_toolkit/orthographic_camera.h"
#include "iris_camera_toolkit/pfm_writer.h"
#include "iris_physx_toolkit/all_light_sampler.h"
#include "iris_physx_toolkit/attenuated_reflector.h"
#include "iris_physx_toolkit/lambertian_brdf.h"
#include "iris_physx_toolkit/list_scene.h"
#include "iris_physx_toolkit/path_tracer.h"
#include "iris_physx_toolkit/point_light.h"
#include "iris_physx_toolkit/sample_tracer.h"
#include "iris_physx_toolkit/triangle.h"
#include "googletest/include/gtest/gtest.h"
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
    _Inout_ PBRDF_ALLOCATOR brdf_allocator,
    _Inout_ PREFLECTOR_ALLOCATOR reflector_allocator,
    _Out_ PVECTOR3 world_shading_normal,
    _Out_ PCBRDF *brdf
    )
{
    PCTRIANGLE_MATERIAL triangle_material = (PCTRIANGLE_MATERIAL)context;
    PCTRIANGLE_ADDITIONAL_DATA triangle_data =
        (PCTRIANGLE_ADDITIONAL_DATA)additional_data;

    PCREFLECTOR reflector;
    ISTATUS status =
        AttenuatedReflector3AllocateWithAllocator(reflector_allocator,
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

    status = LambertianBrdfAllocateWithAllocator(brdf_allocator,
                                                 reflector,
                                                 brdf);

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
    if (reflector0 == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (reflector1 == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (reflector2 == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (material == NULL)
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
TestRenderSingleThreaded(
    _In_ PCLIST_SCENE scene,
    _In_ PALL_LIGHT_SAMPLER light_sampler,
    _In_ const std::string& file_name
    )
{
    PCAMERA camera;
    ISTATUS status = OrthographicCameraAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        VectorCreate((float_t)0.0, (float_t)0.0, (float_t)1.0),
        VectorCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        (float_t)2.0,
        (float_t)2.0,
        &camera);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PPIXEL_SAMPLER pixel_sampler;
    status =
        GridPixelSamplerAllocate(500, 500, false, 1, 1, false, &pixel_sampler);
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

    PTONE_MAPPER tone_mapper;
    status = TestToneMapperAllocate(&tone_mapper);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSAMPLE_TRACER sample_tracer;
    status = PhysxSampleTracerAllocate(
        path_tracer,
        ListSceneTraceCallback,
        scene,
        AllLightSamplerPrepareSamplesCallback,
        AllLightSamplerNextSampleCallback, 
        light_sampler,
        tone_mapper,
        &sample_tracer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PFRAMEBUFFER framebuffer;
    status = FramebufferAllocate(500, 500, &framebuffer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = IrisCameraRender(
        (float_t)0.001, camera, pixel_sampler, sample_tracer, rng, framebuffer);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = WriteToPfmFile(framebuffer,
                            file_name.c_str(),
                            PFM_PIXEL_FORMAT_XYZ);
    EXPECT_EQ(status, ISTATUS_SUCCESS);

    CameraFree(camera);
    PixelSamplerFree(pixel_sampler);
    RandomFree(rng);
    SampleTracerFree(sample_tracer);
    FramebufferFree(framebuffer);
}

TEST(SingleTriangleTest, TestXYTriangle)
{
    PLIST_SCENE scene;
    ISTATUS status = ListSceneAllocate(&scene);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PALL_LIGHT_SAMPLER light_sampler;
    status = AllLightSamplerAllocate(&light_sampler);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSPECTRUM spectrum;
    status = TestSpectrumAllocate((float_t)1.0,
                                  (float_t)1.0,
                                  (float_t)1.0,
                                  &spectrum);
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
    status = PointLightAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)-1.0),
        spectrum,
        &light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    PSHAPE shape;
    status = TriangleAllocate(
        PointCreate((float_t)0.0, (float_t)0.0, (float_t)0.0),
        PointCreate((float_t)0.0, (float_t)1.0, (float_t)0.0),
        PointCreate((float_t)1.0, (float_t)0.0, (float_t)0.0),
        material,
        material,
        &shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = ListSceneAddShape(scene, shape);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    status = AllLightSamplerAddLight(light_sampler, light);
    ASSERT_EQ(status, ISTATUS_SUCCESS);

    TestRenderSingleThreaded(scene,
                             light_sampler, 
                             "TestXYTriangle.pfm");

    ListSceneFree(scene);
    AllLightSamplerFree(light_sampler);
    SpectrumRelease(spectrum);
    ReflectorRelease(reflector0);
    ReflectorRelease(reflector1);
    ReflectorRelease(reflector2);
    LightRelease(light);
    MaterialRelease(material);
    ShapeRelease(shape);
}