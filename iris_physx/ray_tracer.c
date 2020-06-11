/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    ray_tracer.c

Abstract:

    Definitions for the ray_tracer interface.

--*/

#include "iris_physx/emissive_material_internal.h"
#include "iris_physx/material_internal.h"
#include "iris_physx/ray_tracer.h"
#include "iris_physx/ray_tracer_internal.h"
#include "iris_physx/shape.h"
#include "iris_physx/shape_internal.h"

//
// Types
//

typedef struct _SHAPE_RAY_TRACER_PROCESS_HIT_CONTEXT {
    PCRAY_DIFFERENTIAL ray_differential;
    PSHAPE_RAY_TRACER shape_ray_tracer;
    PCSPECTRUM light;
    PCBSDF bsdf;
    POINT3 hit_point;
    VECTOR3 surface_normal;
    VECTOR3 shading_normal;
} SHAPE_RAY_TRACER_PROCESS_HIT_CONTEXT, *PSHAPE_RAY_TRACER_PROCESS_HIT_CONTEXT;

//
// Static Functions
//

ISTATUS 
ShapeRayTracerProcessHit(
    _Inout_opt_ void *context, 
    _In_ PCHIT_CONTEXT hit_context,
    _In_ PCMATRIX model_to_world,
    _In_ POINT3 model_hit_point,
    _In_ POINT3 world_hit_point
    )
{
    PSHAPE_RAY_TRACER_PROCESS_HIT_CONTEXT process_context = 
        (PSHAPE_RAY_TRACER_PROCESS_HIT_CONTEXT)context;

    PSHAPE shape = (PSHAPE)hit_context->data;

    if (shape->vtable->get_emissive_material_routine != NULL &&
        shape->vtable->sample_face_routine != NULL &&
        shape->vtable->compute_pdf_by_solid_angle_routine != NULL)
    {
        PCEMISSIVE_MATERIAL emissive_material;
        ISTATUS status = ShapeGetEmissiveMaterial(shape,
                                                  hit_context->front_face,
                                                  &emissive_material);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }

        if (emissive_material != NULL)
        {
            status = EmissiveMaterialSample(emissive_material,
                                            model_hit_point,
                                            hit_context->additional_data,
                                            &process_context->light);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }
        }
    }

    process_context->hit_point = world_hit_point;

    PCMATERIAL material;
    ISTATUS status = ShapeGetMaterial(shape,
                                      hit_context->front_face,
                                      &material);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (material == NULL)
    {
        return ISTATUS_SUCCESS;
    }

    VECTOR3 model_surface_normal;
    status = ShapeComputeNormal(shape,
                                model_hit_point,
                                hit_context->front_face,
                                &model_surface_normal);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!VectorValidate(model_surface_normal))
    {
        return ISTATUS_INVALID_RESULT;
    }

    const void *texture_coordinates;
    status = ShapeComputeTextureCoordinates(shape,
                                            model_to_world,
                                            process_context->ray_differential,
                                            hit_context->distance,
                                            hit_context->front_face,
                                            hit_context->additional_data,
                                            &process_context->shape_ray_tracer->texture_coordinate_allocator,
                                            &texture_coordinates);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    VECTOR3 model_shading_normal;
    status = ShapeComputeShadingNormal(shape,
                                       model_hit_point,
                                       model_surface_normal,
                                       hit_context->front_face,
                                       hit_context->additional_data,
                                       texture_coordinates,
                                       &model_shading_normal);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!VectorValidate(model_shading_normal))
    {
        return ISTATUS_INVALID_RESULT;
    }

    status = MaterialSample(material,
                            model_hit_point,
                            hit_context->additional_data,
                            texture_coordinates,
                            &process_context->shape_ray_tracer->bsdf_allocator,
                            &process_context->shape_ray_tracer->reflector_compositor,
                            &process_context->bsdf);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    // TODO: Make this a function in multiply
    if (model_to_world != NULL)
    {
        VECTOR3 world_surface_normal, world_shading_normal;
        world_surface_normal = 
            VectorMatrixInverseTransposedMultiply(model_to_world,
                                                  model_surface_normal);
        world_shading_normal =
            VectorMatrixInverseTransposedMultiply(model_to_world,
                                                  model_shading_normal);
        process_context->surface_normal = VectorNormalize(world_surface_normal,
                                                          NULL,
                                                          NULL);
        process_context->shading_normal = VectorNormalize(world_shading_normal,
                                                          NULL,
                                                          NULL);
    }
    else
    {
        // TODO: Decide if it is safe to assume this is pre-normalized
        process_context->surface_normal = model_surface_normal;
        process_context->shading_normal = model_shading_normal;
    }

    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
ShapeRayTracerTrace(
    _Inout_ PSHAPE_RAY_TRACER ray_tracer,
    _In_ RAY_DIFFERENTIAL ray_differential,
    _Outptr_result_maybenull_ PCSPECTRUM *light,
    _Outptr_result_maybenull_ PCBSDF *bsdf,
    _Out_ PPOINT3 hit_point,
    _Out_ PVECTOR3 surface_normal,
    _Out_ PVECTOR3 shading_normal
    )
{
    if (ray_tracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!RayDifferentialValidate(ray_differential))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (bsdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (hit_point == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (surface_normal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (shading_normal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    SHAPE_RAY_TRACER_PROCESS_HIT_CONTEXT context;
    context.ray_differential = &ray_differential;
    context.shape_ray_tracer = ray_tracer;
    context.light = NULL;
    context.bsdf = NULL;

    ISTATUS status =
        RayTracerTraceClosestHitWithCoordinates(ray_tracer->ray_tracer,
                                                ray_differential.ray,
                                                ray_tracer->minimum_distance,
                                                ray_tracer->trace_routine,
                                                ray_tracer->trace_context,
                                                ShapeRayTracerProcessHit,
                                                &context);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *light = context.light;
    *bsdf = context.bsdf;
    *hit_point = context.hit_point;
    *surface_normal = context.surface_normal;
    *shading_normal = context.shading_normal;

    return ISTATUS_SUCCESS;
}