/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    light.c

Abstract:

    Definitions for the light interface.

--*/

#include <stdalign.h>

#include "iris_physx/area_light.h"
#include "iris_physx/emissive_material_internal.h"
#include "iris_physx/hit_tester.h"
#include "iris_physx/shape_internal.h"
#include "iris_physx/visibility_tester_internal.h"

//
// Types
//

typedef struct _AREA_LIGHT {
    PCEMISSIVE_MATERIAL emissive_material;
    PRAY_TRACER_PROCESS_HIT_ROUTINE process_hit_routine;
    PSHAPE trace_shape;
    PSHAPE light_shape;
    PMATRIX model_to_world;
    uint32_t face;
} AREA_LIGHT, *PAREA_LIGHT;

typedef const AREA_LIGHT *PCAREA_LIGHT;

typedef struct _AREA_LIGHT_AND_RESULTS {
    PCAREA_LIGHT area_light;
    PCRAY ray;
    PCSPECTRUM *spectrum;
    float_t *distance;
} AREA_LIGHT_AND_RESULTS, *PAREA_LIGHT_AND_RESULTS;

//
// Static Functions
//

static
ISTATUS
VisibilityTesterTraceSingleShape(
    _In_opt_ const void *context,
    _Inout_ PSHAPE_HIT_TESTER hit_tester,
    _In_ RAY ray
    )
{
    PCSHAPE shape = (PCSHAPE)context;

    PHIT_TESTER_TEST_GEOMETRY_ROUTINE test_routine =
        (PHIT_TESTER_TEST_GEOMETRY_ROUTINE)((const void ***)shape)[0][0];
    const void *shape_context = ((const void **)shape)[1];
    ISTATUS status = HitTesterTestWorldGeometry(hit_tester,
                                                test_routine,
                                                shape_context,
                                                shape);

    return status;
}

static
ISTATUS
VisibilityTesterProcessHitAreaLight(
    _Inout_opt_ void *context, 
    _In_ PCHIT_CONTEXT hit_context
    )
{
    PAREA_LIGHT_AND_RESULTS area_light_and_results =
        (PAREA_LIGHT_AND_RESULTS)context;

    if (hit_context->front_face != area_light_and_results->area_light->face)
    {
        return ISTATUS_SUCCESS;
    }

    POINT3 hit_point = RayEndpoint(*area_light_and_results->ray,
                                   hit_context->distance);

    ISTATUS status =
        EmissiveMaterialSample(area_light_and_results->area_light->emissive_material,
                               hit_point,
                               hit_context->additional_data,
                               area_light_and_results->spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *(area_light_and_results->distance) = hit_context->distance;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
VisibilityTesterProcessHitNestedAreaLight(
    _Inout_opt_ void *context,
    _In_ PCHIT_CONTEXT hit_context
    )
{
    PAREA_LIGHT_AND_RESULTS area_light_and_results =
        (PAREA_LIGHT_AND_RESULTS)context;

    if (hit_context->data != area_light_and_results->area_light->light_shape ||
        hit_context->front_face != area_light_and_results->area_light->face)
    {
        return ISTATUS_SUCCESS;
    }

    POINT3 hit_point = RayEndpoint(*area_light_and_results->ray,
                                   hit_context->distance);

    ISTATUS status =
        EmissiveMaterialSample(area_light_and_results->area_light->emissive_material,
                               hit_point,
                               hit_context->additional_data,
                               area_light_and_results->spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *(area_light_and_results->distance) = hit_context->distance;

    return ISTATUS_SUCCESS;
}

static
inline
ISTATUS
VisibilityTesterTestSingleAreaLight(
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _In_ RAY ray,
    _In_ PCAREA_LIGHT area_light,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *distance
    )
{
    assert(visibility_tester != NULL);
    assert(RayValidate(ray));
    assert(spectrum != NULL);
    assert(distance != NULL);

    AREA_LIGHT_AND_RESULTS area_light_and_results;
    area_light_and_results.area_light = area_light;
    area_light_and_results.ray = &ray;
    area_light_and_results.spectrum = spectrum;
    area_light_and_results.distance = distance;

    *spectrum = NULL;
    ISTATUS status =
        RayTracerTraceClosestHit(visibility_tester->ray_tracer,
                                 ray,
                                 visibility_tester->epsilon,
                                 VisibilityTesterTraceSingleShape,
                                 area_light->trace_shape,
                                 area_light->process_hit_routine,
                                 &area_light_and_results);

    return status;
}

static
inline
ISTATUS
AreaLightComputeEmissiveWithPdfInternal(
    _In_ const void *context,
    _In_ RAY model_to_light,
    _In_ RAY world_to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    )
{
    assert(context != NULL);
    assert(RayValidate(model_to_light));
    assert(RayValidate(world_to_light));
    assert(visibility_tester != NULL);
    assert(compositor != NULL);
    assert(spectrum != NULL);
    assert(pdf != NULL);

    PCAREA_LIGHT area_light = (PCAREA_LIGHT)context;

    float_t distance;
    ISTATUS status = VisibilityTesterTestSingleAreaLight(visibility_tester,
                                                         model_to_light,
                                                         area_light,
                                                         spectrum,
                                                         &distance);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (*spectrum == NULL)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    status = ShapeComputePdfBySolidAngle(area_light->light_shape,
                                         &model_to_light,
                                         distance,
                                         area_light->face,
                                         pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (*pdf <= (float_t)0.0 || isinf(*pdf))
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    bool visible;
    status = VisibilityTesterTestInline(visibility_tester,
                                        world_to_light,
                                        distance,
                                        &visible);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!visible)
    {
        *pdf = (float_t)0.0;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AreaLightComputeEmissive(
    _In_ const void *context,
    _In_ PCRAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    PCAREA_LIGHT area_light = (PCAREA_LIGHT)context;

    RAY model_to_light = RayMatrixInverseMultiply(area_light->model_to_world,
                                                  *to_light);

    float_t distance;
    ISTATUS status = VisibilityTesterTestSingleAreaLight(visibility_tester,
                                                         model_to_light,
                                                         area_light,
                                                         spectrum,
                                                         &distance);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (*spectrum == NULL)
    {
        *spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    float_t pdf;
    status = ShapeComputePdfBySolidAngle(area_light->light_shape,
                                         &model_to_light,
                                         distance,
                                         area_light->face,
                                         &pdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (pdf <= (float_t)0.0 || isinf(pdf))
    {
        *spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    bool visible;
    status = VisibilityTesterTestInline(visibility_tester,
                                        *to_light,
                                        distance,
                                        &visible);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!visible)
    {
        *spectrum = NULL;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
AreaLightComputeEmissiveWithPdf(
    _In_ const void *context,
    _In_ PCRAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    )
{
    PCAREA_LIGHT area_light = (PCAREA_LIGHT)context;

    RAY model_to_light = RayMatrixInverseMultiply(area_light->model_to_world,
                                                  *to_light);

    ISTATUS status = AreaLightComputeEmissiveWithPdfInternal(context,
                                                             model_to_light,
                                                             *to_light,
                                                             visibility_tester,
                                                             compositor,
                                                             spectrum,
                                                             pdf);

    return status;
}

static
ISTATUS
AreaLightSample(
    _In_ const void *context,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 surface_normal,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PRANDOM rng,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ PVECTOR3 to_light,
    _Out_ float_t *pdf
    )
{
    PCAREA_LIGHT area_light = (PCAREA_LIGHT)context;

    POINT3 sampled_point;
    ISTATUS status = ShapeSampleFace(area_light->light_shape,
                                     area_light->face,
                                     rng,
                                     &sampled_point);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    POINT3 world_sampled_point;
    world_sampled_point = PointMatrixMultiply(area_light->model_to_world,
                                              sampled_point);

    *to_light = PointSubtract(world_sampled_point, hit_point);
    *to_light = VectorNormalize(*to_light, NULL, NULL);

    RAY world_ray_to_light = RayCreate(hit_point, *to_light);
    RAY model_ray_to_light =
        RayMatrixInverseMultiply(area_light->model_to_world, world_ray_to_light);

    status = AreaLightComputeEmissiveWithPdfInternal(context,
                                                     world_ray_to_light,
                                                     model_ray_to_light,
                                                     visibility_tester,
                                                     compositor,
                                                     spectrum,
                                                     pdf);
    return status;
}

static
ISTATUS
AreaLightCacheColors(
    _In_ const void *context,
    _Inout_ PCOLOR_CACHE color_cache
    )
{
    return ISTATUS_SUCCESS;
}

static
void
AreaLightFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PAREA_LIGHT area_light = (PAREA_LIGHT)context;

    ShapeRelease(area_light->trace_shape);
    ShapeRelease(area_light->light_shape);
    MatrixRelease(area_light->model_to_world);
}

//
// Static Variables
//

static const LIGHT_VTABLE area_light_vtable = {
    AreaLightSample,
    AreaLightComputeEmissive,
    AreaLightComputeEmissiveWithPdf,
    AreaLightCacheColors,
    AreaLightFree
};

//
// Static Functions
//

static
ISTATUS
AreaLightAllocateInternal(
    _In_ PSHAPE trace_shape,
    _In_ PSHAPE light_shape,
    _In_ uint32_t face,
    _In_opt_ PMATRIX model_to_world,
    _Out_ PLIGHT *light
    )
{
    assert(trace_shape != NULL);
    assert(light_shape != NULL);
    assert(light != NULL);

    PCEMISSIVE_MATERIAL emissive_material;
    ISTATUS status = ShapeGetEmissiveMaterial(light_shape,
                                              face,
                                              &emissive_material);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (emissive_material == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
    }

    AREA_LIGHT area_light;
    area_light.emissive_material = emissive_material;
    area_light.trace_shape = trace_shape;
    area_light.light_shape = light_shape;
    area_light.model_to_world = model_to_world;
    area_light.face = face;

    if (trace_shape == light_shape)
    {
        area_light.process_hit_routine = VisibilityTesterProcessHitAreaLight;
    }
    else
    {
        area_light.process_hit_routine =
            VisibilityTesterProcessHitNestedAreaLight;
    }

    status = LightAllocate(&area_light_vtable,
                           &area_light,
                           sizeof(AREA_LIGHT),
                           alignof(AREA_LIGHT),
                           light);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    ShapeRetain(trace_shape);
    ShapeRetain(light_shape);
    MatrixRetain(model_to_world);

    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
AreaLightAllocate(
    _In_ PSHAPE shape,
    _In_ uint32_t face,
    _In_opt_ PMATRIX model_to_world,
    _Out_ PLIGHT *light
    )
{
    if (shape == NULL ||
        shape->vtable->get_emissive_material_routine == NULL ||
        shape->vtable->sample_face_routine == NULL ||
        shape->vtable->compute_pdf_by_solid_angle_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_COMBINATION_03;
    }

    ISTATUS status = AreaLightAllocateInternal(shape,
                                               shape,
                                               face,
                                               model_to_world,
                                               light);

    return status;
}

ISTATUS
NestedAreaLightAllocate(
    _In_ PSHAPE trace_shape,
    _In_ PSHAPE light_shape,
    _In_ uint32_t face,
    _In_opt_ PMATRIX model_to_world,
    _Out_ PLIGHT *light
    )
{
    if (trace_shape == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (light_shape == NULL ||
        light_shape->vtable->get_emissive_material_routine == NULL ||
        light_shape->vtable->sample_face_routine == NULL ||
        light_shape->vtable->compute_pdf_by_solid_angle_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    ISTATUS status = AreaLightAllocateInternal(trace_shape,
                                               light_shape,
                                               face,
                                               model_to_world,
                                               light);

    return status;
}