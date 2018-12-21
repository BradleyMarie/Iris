/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    light.c

Abstract:

    Definitions for the light interface.

--*/

#include <stdalign.h>

#include "iris_physx/area_light.h"
#include "iris_physx/emissive_material_internal.h"
#include "iris_physx/shape_internal.h"
#include "iris_physx/visibility_tester_internal.h"

//
// Types
//

typedef struct _AREA_LIGHT {
    PCEMISSIVE_MATERIAL emissive_material;
    PMATRIX model_to_world;
    PSHAPE shape;
    uint32_t face;
} AREA_LIGHT, *PAREA_LIGHT;

typedef const AREA_LIGHT *PCAREA_LIGHT;

typedef struct _AREA_LIGHT_AND_RESULTS {
    PCAREA_LIGHT area_light;
    PCSPECTRUM *spectrum;
    PPOINT3 hit_point;
} AREA_LIGHT_AND_RESULTS, *PAREA_LIGHT_AND_RESULTS;

//
// Static Functions
//

static
ISTATUS
VisibilityTesterProcessHitAreaLight(
    _Inout_opt_ void *context, 
    _In_ PCHIT_CONTEXT hit_context,
    _In_ PCMATRIX model_to_world,
    _In_ POINT3 model_hit_point,
    _In_ POINT3 world_hit_point
    )
{
    PAREA_LIGHT_AND_RESULTS area_light_and_results =
        (PAREA_LIGHT_AND_RESULTS)context;

    if (area_light_and_results->area_light->shape == (PSHAPE)hit_context->data &&
        area_light_and_results->area_light->model_to_world == model_to_world &&
        area_light_and_results->area_light->face == hit_context->front_face)
    {
        ISTATUS status = 
            EmissiveMaterialSample(area_light_and_results->area_light->emissive_material,
                                   model_hit_point,
                                   hit_context->additional_data,
                                   area_light_and_results->spectrum);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        *(area_light_and_results->hit_point) = world_hit_point;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
VisibilityTesterTestAreaLight(
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _In_ RAY ray,
    _In_ PCAREA_LIGHT area_light,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ PPOINT3 hit_point
    )
{
    assert(visibility_tester != NULL);
    assert(RayValidate(ray));
    assert(spectrum != NULL);
    assert(hit_point != NULL);

    AREA_LIGHT_AND_RESULTS area_light_and_results;
    area_light_and_results.area_light = area_light;
    area_light_and_results.spectrum = spectrum;
    area_light_and_results.hit_point = hit_point;

    *spectrum = NULL;
    ISTATUS status = 
        RayTracerTraceClosestHitWithCoordinates(visibility_tester->ray_tracer,
                                                ray,
                                                visibility_tester->epsilon,
                                                visibility_tester->trace_routine,
                                                visibility_tester->trace_context,
                                                VisibilityTesterProcessHitAreaLight,
                                                &area_light_and_results);

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
    ISTATUS status = ShapeSampleFaceBySolidAngle(area_light->shape,
                                                 hit_point,
                                                 area_light->face,
                                                 rng,
                                                 &sampled_point,
                                                 pdf);

    sampled_point = PointMatrixMultiply(area_light->model_to_world,
                                        sampled_point);

    VECTOR3 direction_to_light = PointSubtract(sampled_point, hit_point);

    float_t dp = VectorDotProduct(direction_to_light, surface_normal);

    if (dp < (float_t)0.0)
    {
        *spectrum = NULL;
        *to_light = direction_to_light;
        return ISTATUS_SUCCESS;
    }

    direction_to_light = VectorNormalize(direction_to_light, NULL, NULL);

    RAY ray_to_light = RayCreate(hit_point, direction_to_light);

    POINT3 point_on_light;
    status = VisibilityTesterTestAreaLight(visibility_tester,
                                           ray_to_light,
                                           area_light,
                                           spectrum,
                                           &point_on_light);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *to_light = direction_to_light;

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

    POINT3 point_on_light;
    ISTATUS status = VisibilityTesterTestAreaLight(visibility_tester,
                                                   *to_light,
                                                   area_light,
                                                   spectrum,
                                                   &point_on_light);

    return status;
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

    POINT3 point_on_light;
    ISTATUS status = VisibilityTesterTestAreaLight(visibility_tester,
                                                   *to_light,
                                                   area_light,
                                                   spectrum,
                                                   &point_on_light);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (*spectrum == NULL)
    {
        *pdf = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    VECTOR3 to_light_vector = PointSubtract(point_on_light, to_light->origin);
    float_t distance_squared = VectorDotProduct(to_light_vector,
                                                to_light_vector);

    status = ShapeComputePdfBySolidAngle(area_light->shape,
                                         to_light,
                                         distance_squared,
                                         area_light->face,
                                         pdf);

    return ISTATUS_SUCCESS;
}

static
void
AreaLightFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PAREA_LIGHT area_light = (PAREA_LIGHT)context;

    MatrixRelease(area_light->model_to_world);
    ShapeRelease(area_light->shape);
}

//
// Static Variables
//

static const LIGHT_VTABLE area_light_vtable = {
    AreaLightSample,
    AreaLightComputeEmissive,
    AreaLightComputeEmissiveWithPdf,
    AreaLightFree
};

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
        shape->vtable->sample_face_by_solid_angle_routine == NULL ||
        shape->vtable->compute_pdf_by_solid_angle_routine == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    PCEMISSIVE_MATERIAL emissive_material;
    ISTATUS status = ShapeGetEmissiveMaterial(shape, face, &emissive_material);

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
    area_light.model_to_world = model_to_world;
    area_light.shape = shape;
    area_light.face = face;

    status = LightAllocate(&area_light_vtable,
                           &area_light,
                           sizeof(AREA_LIGHT),
                           alignof(AREA_LIGHT),
                           light);

    ShapeRetain(shape);
    MatrixRetain(model_to_world);

    return status;
}