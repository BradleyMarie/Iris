/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    infinite_environmental_light.c

Abstract:

    Implements a infinite environmental light.

--*/

#include <stdalign.h>
#include <stdlib.h>

#include "common/safe_math.h"
#include "iris_physx_toolkit/infinite_environmental_light.h"

//
// Types
//

typedef struct _INFINITE_LIGHT {
    PSPECTRUM_MIPMAP mipmap;
    PMATRIX light_to_world;
    _Field_size_(num_texels) float_t *cdf;
    _Field_size_(num_texels) float_t *pdf;
    _Field_size_(num_texels) float_t *texel_base_u;
    _Field_size_(num_texels) float_t *texel_base_v;
    size_t num_texels;
    size_t row_width;
    float_t texel_width_u;
    float_t texel_width_v;
    float_t width_fp;
    float_t height_fp;
} INFINITE_LIGHT, *PINFINITE_LIGHT;

typedef const INFINITE_LIGHT *PCINFINITE_LIGHT;

//
// Static Functions
//

static
int
FindTexel(
    _In_ const void* to_find,
    _In_ const void* to_examine
    )
{
    const float_t *to_find_float = (const float_t*)to_find;
    const float_t *to_examine_float = (const float_t*)to_examine;

    if (*to_find_float == *to_examine_float)
    {
        return 0;
    }

    if (*to_find_float < *to_examine_float)
    {
        return -1;
    }

    if (*to_find_float < *(to_examine_float + 1))
    {
        return 0;
    }

    return 1;
}

static
inline
void
DirectionToUV(
    _In_ VECTOR3 direction,
    _Out_ float uv[2]
    )
{
    direction = VectorNormalize(direction, NULL, NULL);

    float_t clamped_z =
        IMin(IMax(direction.z, (float_t)-1.0), (float_t)1.0);
    float_t theta = acos(clamped_z);

    float_t phi = atan2(direction.y, direction.x);
    if (phi < (float_t)0.0) {
        phi += iris_two_pi;
    }

    uv[0] = phi * iris_inv_two_pi;
    uv[1] = theta * iris_inv_pi;
}

static
ISTATUS
InfiniteEnvironmentalLightSample(
    _In_ const void *context,
    _In_ VECTOR3 surface_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ PVECTOR3 to_light,
    _Out_ float_t *pdf
    )
{
    PCINFINITE_LIGHT infinite_light = (PCINFINITE_LIGHT)context;

    float_t u;
    ISTATUS status = RandomGenerateFloat(rng,
                                         (float_t)0.0,
                                         (float_t)1.0,
                                         &u);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    const float_t *found = bsearch(&u,
                                   infinite_light->cdf,
                                   infinite_light->num_texels,
                                   sizeof(float_t),
                                   FindTexel);
    assert(found != NULL);

    size_t index = found - infinite_light->cdf;
    u = (u - *found) / infinite_light->pdf[index];
    u = infinite_light->texel_base_u[index] + u * infinite_light->texel_width_u;

    float_t v;
    status = RandomGenerateFloat(rng,
                                 (float_t)0.0,
                                 (float_t)1.0,
                                 &v);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    v = infinite_light->texel_base_v[index] + v * infinite_light->texel_width_v;

    status = SpectrumMipmapLookup(infinite_light->mipmap,
                                  u,
                                  v,
                                  compositor,
                                  spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    float_t phi = u * iris_two_pi;
    float_t theta = (v - (float_t)0.5) * iris_pi;

    float_t sin_phi, cos_phi;
    SinCos(phi, &sin_phi, &cos_phi);

    float_t sin_theta, cos_theta;
    SinCos(theta, &sin_theta, &cos_theta);

    VECTOR3 model_to_light = VectorCreate(cos_phi * sin_theta,
                                          sin_phi * sin_theta,
                                          cos_theta);

    *to_light = VectorMatrixMultiply(infinite_light->light_to_world,
                                     model_to_light);
    *pdf = infinite_light->pdf[index];

    return ISTATUS_SUCCESS;
}

static
ISTATUS
InfiniteEnvironmentalLightComputeEmissive(
    _In_ const void *context,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    )
{
    PCINFINITE_LIGHT infinite_light = (PCINFINITE_LIGHT)context;

    VECTOR3 model_to_light =
        VectorMatrixInverseMultiply(infinite_light->light_to_world,
                                    to_light);

    float_t uv[2];
    DirectionToUV(model_to_light, uv);

    // TODO: Add filtered lookup support
    ISTATUS status = SpectrumMipmapLookup(infinite_light->mipmap,
                                          uv[0],
                                          uv[1],
                                          compositor,
                                          spectrum);

    return status;
}

static
ISTATUS
InfiniteEnvironmentalLightComputeEmissiveWithPdf(
    _In_ const void *context,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    )
{
    PCINFINITE_LIGHT infinite_light = (PCINFINITE_LIGHT)context;

    VECTOR3 model_to_light =
        VectorMatrixInverseMultiply(infinite_light->light_to_world,
                                    to_light);

    float_t uv[2];
    DirectionToUV(model_to_light, uv);

    // TODO: Add filtered lookup support
    ISTATUS status = SpectrumMipmapLookup(infinite_light->mipmap,
                                          uv[0],
                                          uv[1],
                                          compositor,
                                          spectrum);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    size_t x = uv[0] * infinite_light->width_fp;
    size_t y = uv[1] * infinite_light->height_fp;

    *pdf = infinite_light->pdf[x + y * infinite_light->row_width];

    return status;
}

static
void
InfiniteEnvironmentalLightFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PINFINITE_LIGHT infinite_light = (PINFINITE_LIGHT)context;

    SpectrumMipmapFree(infinite_light->mipmap);
    MatrixRelease(infinite_light->light_to_world);
    free(infinite_light->cdf);
    free(infinite_light->pdf);
    free(infinite_light->texel_base_u);
    free(infinite_light->texel_base_v);
}

//
// Static Variables
//

static const ENVIRONMENTAL_LIGHT_VTABLE infinite_light_vtable = {
    InfiniteEnvironmentalLightSample,
    InfiniteEnvironmentalLightComputeEmissive,
    InfiniteEnvironmentalLightComputeEmissiveWithPdf,
    InfiniteEnvironmentalLightFree
};

//
// Functions
//

ISTATUS
InfiniteEnvironmentalLightAllocate(
    _In_ PSPECTRUM_MIPMAP mipmap,
    _In_opt_ PMATRIX light_to_world,
    _In_ PCOLOR_INTEGRATOR color_integrator,
    _Out_ PENVIRONMENTAL_LIGHT *environmental_light,
    _Out_ PLIGHT *light
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (environmental_light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (light == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    size_t levels, width, height;
    SpectrumMipmapGetDimensions(mipmap, &levels, &width, &height);

    size_t num_texels = width * height;

    size_t texel_list_size;
    bool success = CheckedAddSizeT(num_texels, 1, &texel_list_size);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    INFINITE_LIGHT infinite_light;
    infinite_light.mipmap = mipmap;
    infinite_light.light_to_world = light_to_world;
    infinite_light.texel_width_u = (float_t)1.0 / (float_t)width;
    infinite_light.texel_width_v = (float_t)1.0 / (float_t)height;
    infinite_light.row_width = width;
    infinite_light.width_fp = (float_t)width;
    infinite_light.height_fp = (float_t)height;

    infinite_light.cdf = (float_t*)calloc(texel_list_size, sizeof(float_t));

    if (infinite_light.cdf == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    infinite_light.pdf = (float_t*)calloc(texel_list_size, sizeof(float_t));

    if (infinite_light.pdf == NULL)
    {
        free(infinite_light.cdf);
        return ISTATUS_ALLOCATION_FAILED;
    }

    infinite_light.texel_base_u = (float_t*)calloc(texel_list_size,
                                                   sizeof(float_t));

    if (infinite_light.texel_base_u == NULL)
    {
        free(infinite_light.pdf);
        free(infinite_light.cdf);
        return ISTATUS_ALLOCATION_FAILED;
    }

    infinite_light.texel_base_v = (float_t*)calloc(texel_list_size,
                                                   sizeof(float_t));

    if (infinite_light.texel_base_v == NULL)
    {
        free(infinite_light.texel_base_u);
        free(infinite_light.pdf);
        free(infinite_light.cdf);
        return ISTATUS_ALLOCATION_FAILED;
    }

    infinite_light.num_texels = texel_list_size;

    float_t running_total = (float_t)0.0;
    for (size_t y = 0; y < height; y++)
    {
        float_t theta =
            (((float_t)y + (float_t)0.5) / (float_t)height) * iris_pi;
        float_t sin_theta = sin(theta);
        for (size_t x = 0; x < width; x++)
        {
            PCSPECTRUM spectrum;
            SpectrumMipmapTexelLookup(mipmap,
                                      0,
                                      x,
                                      y,
                                      &spectrum);

            COLOR3 color;
            ISTATUS status =
                ColorIntegratorComputeSpectrumColor(color_integrator,
                                                    spectrum,
                                                    &color);

            if (status != ISTATUS_SUCCESS)
            {
                return status;
            }

            color = ColorConvert(color, COLOR_SPACE_XYZ);
            float_t luma = color.values[1] * sin_theta;

            infinite_light.cdf[x + y * width] = running_total;
            infinite_light.pdf[x + y * width] = luma;
            infinite_light.texel_base_u[x + y * width] =
                (float_t)x / (float_t)width;
            infinite_light.texel_base_v[x + y * width] =
                (float_t)y / (float_t)height;

            running_total += luma;
        }
    }

    if (running_total == (float_t)0.0)
    {
        running_total = (float_t)1.0;
    }

    for (size_t i = 0; i < num_texels; i++)
    {
        infinite_light.cdf[i] /= running_total;
        infinite_light.pdf[i] /= running_total;
    }

    infinite_light.cdf[texel_list_size - 1] = (float_t)1.0;
    infinite_light.pdf[texel_list_size - 1] =
        infinite_light.pdf[texel_list_size - 2];
    infinite_light.texel_base_u[texel_list_size - 1] =
        infinite_light.texel_base_u[texel_list_size - 2];
    infinite_light.texel_base_v[texel_list_size - 1] =
        infinite_light.texel_base_v[texel_list_size - 2];

    ISTATUS status = EnvironmentalLightAllocate(&infinite_light_vtable,
                                                &infinite_light,
                                                sizeof(INFINITE_LIGHT),
                                                alignof(INFINITE_LIGHT),
                                                environmental_light,
                                                light);

    if (status != ISTATUS_SUCCESS)
    {
        free(infinite_light.texel_base_v);
        free(infinite_light.texel_base_u);
        free(infinite_light.pdf);
        free(infinite_light.cdf);
        return status;
    }

    MatrixRetain(light_to_world);

    return ISTATUS_SUCCESS;
}