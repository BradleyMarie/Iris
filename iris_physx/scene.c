/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    scene.c

Abstract:

    A structured representation of the scenes in a scene.

--*/

#include <stdalign.h>
#include <string.h>

#include "common/alloc.h"
#include "iris_physx/emissive_material_internal.h"
#include "iris_physx/scene.h"
#include "iris_physx/scene_internal.h"
#include "iris_physx/visibility_tester_internal.h"

//
// Types
//

typedef struct _OMNIDIRECTIONAL_LIGHT {
    PEMISSIVE_MATERIAL material;
} OMNIDIRECTIONAL_LIGHT, *POMNIDIRECTIONAL_LIGHT;

typedef const OMNIDIRECTIONAL_LIGHT *PCOMNIDIRECTIONAL_LIGHT;

//
// static Constants
//

static const float_t inv_4_pi =
    (float_t)0.0795774715459476678844418816862571810172298228702282243738336720;

//
// Static Functions
//

static
inline
ISTATUS
SampleSphereUniformly(
    _Inout_ PRANDOM rng,
    _Out_ PVECTOR3 result
    )
{
    assert(rng != NULL);
    assert(result != NULL);

    float_t z;
    ISTATUS status = RandomGenerateFloat(rng, (float_t)-1.0, (float_t)1.0, &z);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t phi;
    status = RandomGenerateFloat(rng, -iris_pi, iris_pi, &phi);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t r = sqrt(IMax((float_t)0.0, (float_t)1.0 - z * z));

    float_t sin_phi, cos_phi;
    SinCos(phi, &sin_phi, &cos_phi);

    float_t x = r * cos_phi;
    float_t y = r * sin_phi;

    *result = VectorCreate(x, y, z);

    return ISTATUS_SUCCESS;
}

static
ISTATUS
OmnidirectionalLightSample(
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
    PCOMNIDIRECTIONAL_LIGHT omnidirectional_light = (PCOMNIDIRECTIONAL_LIGHT)context;

    ISTATUS status = SampleSphereUniformly(rng, to_light);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    RAY ray_to_light = RayCreate(hit_point, *to_light);

    bool visible;
    status = VisibilityTesterTestAnyDistanceInline(visibility_tester,
                                                   ray_to_light,
                                                   &visible);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!visible)
    {
        *spectrum = NULL;
        return ISTATUS_SUCCESS;
    }


    POINT3 as_point = PointCreate(to_light->x, to_light->y, to_light->z);
    status = EmissiveMaterialSample(omnidirectional_light->material,
                                    as_point,
                                    NULL,
                                    spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *pdf = inv_4_pi;

    return ISTATUS_SUCCESS;
}

static
ISTATUS
OmnidirectionalLightComputeEmissive(
    _In_ const void *context,
    _In_ PCRAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    PCOMNIDIRECTIONAL_LIGHT omnidirectional_light = (PCOMNIDIRECTIONAL_LIGHT)context;

    bool visible;
    ISTATUS status = VisibilityTesterTestAnyDistanceInline(visibility_tester,
                                                           *to_light,
                                                           &visible);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!visible)
    {
        *spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    POINT3 point = PointCreate(to_light->direction.x,
                               to_light->direction.y,
                               to_light->direction.z);

    status = EmissiveMaterialSample(omnidirectional_light->material,
                                    point,
                                    NULL,
                                    spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    return ISTATUS_SUCCESS;
}

static
ISTATUS
OmnidirectionalLightComputeEmissiveWithPdf(
    _In_ const void *context,
    _In_ PCRAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    )
{
    PCOMNIDIRECTIONAL_LIGHT omnidirectional_light = (PCOMNIDIRECTIONAL_LIGHT)context;

    bool visible;
    ISTATUS status = VisibilityTesterTestAnyDistanceInline(visibility_tester,
                                                           *to_light,
                                                           &visible);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    if (!visible)
    {
        *spectrum = NULL;
        return ISTATUS_SUCCESS;
    }

    POINT3 point = PointCreate(to_light->direction.x,
                               to_light->direction.y,
                               to_light->direction.z);

    status = EmissiveMaterialSample(omnidirectional_light->material,
                                    point,
                                    NULL,
                                    spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    *pdf = inv_4_pi;

    return ISTATUS_SUCCESS;
}

static
void
OmnidirectionalLightFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    POMNIDIRECTIONAL_LIGHT omnidirectional_light = (POMNIDIRECTIONAL_LIGHT)context;

    EmissiveMaterialRelease(omnidirectional_light->material);
}

//
// Static Variables
//

static const LIGHT_VTABLE omnidirectional_light_vtable = {
    OmnidirectionalLightSample,
    OmnidirectionalLightComputeEmissive,
    OmnidirectionalLightComputeEmissiveWithPdf,
    OmnidirectionalLightFree
};

static
ISTATUS
OmnidirectionalLightAllocate(
    _In_opt_ PEMISSIVE_MATERIAL material,
    _Out_ PLIGHT *light
    )
{
    assert(light != NULL);

    if (material == NULL)
    {
        *light = NULL;
        return ISTATUS_SUCCESS;
    }

    OMNIDIRECTIONAL_LIGHT omnidirectional_light;
    omnidirectional_light.material = material;

    ISTATUS status = LightAllocate(&omnidirectional_light_vtable,
                                   &omnidirectional_light,
                                   sizeof(OMNIDIRECTIONAL_LIGHT),
                                   alignof(OMNIDIRECTIONAL_LIGHT),
                                   light);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    EmissiveMaterialRetain(material);

    return ISTATUS_SUCCESS;
}

//
// Functions
//

ISTATUS
SceneAllocate(
    _In_ PCSCENE_VTABLE vtable,
    _In_opt_ PEMISSIVE_MATERIAL background,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PSCENE *scene,
    _Out_ PLIGHT *background_light
    )
{
    if (vtable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (data_size != 0)
    {
        if (data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }
        
        if (data_alignment == 0 ||
            (data_alignment & (data_alignment - 1)) != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;    
        }
        
        if (data_size % data_alignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (scene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (background_light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    PLIGHT light;
    ISTATUS status = OmnidirectionalLightAllocate(background, &light);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    void *data_allocation;
    bool success = AlignedAllocWithHeader(sizeof(SCENE),
                                          alignof(SCENE),
                                          (void **)scene,
                                          data_size,
                                          data_alignment,
                                          &data_allocation);

    if (!success)
    {
        LightRelease(light);
        return ISTATUS_ALLOCATION_FAILED;
    }

    (*scene)->vtable = vtable;
    (*scene)->data = data_allocation;
    (*scene)->background = background;
    (*scene)->reference_count = 1;

    if (data_size != 0)
    {
        memcpy(data_allocation, data, data_size);
    }

    *background_light = light;

    EmissiveMaterialRetain(background);

    return ISTATUS_SUCCESS;
}

void
SceneRetain(
    _In_opt_ PSCENE scene
    )
{
    if (scene == NULL)
    {
        return;
    }

    atomic_fetch_add(&scene->reference_count, 1);
}

void
SceneRelease(
    _In_opt_ _Post_invalid_ PSCENE scene
    )
{
    if (scene == NULL)
    {
        return;
    }

    if (atomic_fetch_sub(&scene->reference_count, 1) == 1)
    {
        if (scene->vtable->free_routine != NULL)
        {
            scene->vtable->free_routine(scene->data);
        }

        free(scene);
    }
}