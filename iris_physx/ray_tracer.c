/*++

Copyright (c) 2018 Brad Weinberger

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
    PSHAPE_RAY_TRACER shape_ray_tracer;
    PCSPECTRUM light;
    PCBRDF brdf;
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
        shape->vtable->compute_face_area_routine != NULL &&
        shape->vtable->sample_face_routine != NULL)
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

    // TODO: Make this a function in multiply
    if (model_to_world != NULL)
    {
        VECTOR3 world_surface_normal;
        world_surface_normal = 
            VectorMatrixInverseTransposedMultiply(model_to_world,
                                                  model_surface_normal);
        process_context->surface_normal = VectorNormalize(world_surface_normal,
                                                          NULL,
                                                          NULL);
    }
    else
    {
        // TODO: Decide if it is safe to assume this is pre-normalized
        process_context->surface_normal = model_surface_normal;
    }

    status = MaterialSample(material,
                            model_hit_point,
                            process_context->surface_normal,
                            hit_context->additional_data,
                            &process_context->shape_ray_tracer->brdf_allocator,
                            &process_context->shape_ray_tracer->reflector_allocator,
                            &process_context->shading_normal,
                            &process_context->brdf);

    return status;
}

//
// Functions
//

ISTATUS
ShapeRayTracerTrace(
    _Inout_ PSHAPE_RAY_TRACER ray_tracer,
    _In_ RAY ray,
    _Outptr_result_maybenull_ PCSPECTRUM *light,
    _Outptr_result_maybenull_ PCBRDF *brdf,
    _Out_ PPOINT3 hit_point,
    _Out_ PVECTOR3 surface_normal,
    _Out_ PVECTOR3 shading_normal
    )
{
    if (ray_tracer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!RayValidate(ray))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (brdf == NULL)
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
    context.shape_ray_tracer = ray_tracer;
    context.light = NULL;
    context.brdf = NULL;

    ISTATUS status =
        RayTracerTraceClosestHitWithCoordinates(ray_tracer->ray_tracer,
                                                ray,
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
    *brdf = context.brdf;
    *hit_point = context.hit_point;
    *surface_normal = context.surface_normal;
    *shading_normal = context.shading_normal;

    return ISTATUS_SUCCESS;
}