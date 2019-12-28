/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    mipmap.h

Abstract:

    Creates a mipmap.

--*/

#include "iris_physx_toolkit/mipmap.h"

#include <stdlib.h>

#include "common/safe_math.h"

//
// Types
//

struct _SPECTRUM_MIPMAP {
    _Field_size_(width * height) PREFLECTOR *textels;
    float_t width_fp;
    float_t height_fp;
    size_t width;
    size_t height;
};

//
// Functions
//

ISTATUS
SpectrumMipmapAllocate(
    _In_reads_(height * width) float_t textels[][3],
    _In_ size_t width,
    _In_ size_t height,
    _In_ PCRGB_INTERPOLATOR rgb_interpolator,
    _Inout_opt_ PCOLOR_INTEGRATOR color_integrator,
    _Out_ PSPECTRUM_MIPMAP *mipmap
    )
{
    if (textels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (rgb_interpolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (color_integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    size_t num_pixels;
    bool success = CheckedMultiplySizeT(width, height, &num_pixels);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PSPECTRUM_MIPMAP result = (PSPECTRUM_MIPMAP)malloc(sizeof(SPECTRUM_MIPMAP));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PREFLECTOR *spectra = (PREFLECTOR*)calloc(num_pixels, sizeof(PREFLECTOR));

    if (spectra == NULL)
    {
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    result->textels = spectra;
    result->width = width;
    result->height = height;
    result->width_fp = (float_t)width;
    result->height_fp = (float_t)height;

    for (size_t i = 0; i < num_pixels; i++)
    {
        if (!isfinite(textels[i][0]) || textels[i][0] < (float_t)0.0)
        {
            SpectrumMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_01;
        }

        if (!isfinite(textels[i][1]) || textels[i][1] < (float_t)0.0)
        {
            SpectrumMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_02;
        }

        if (!isfinite(textels[i][2]) || textels[i][2] < (float_t)0.0)
        {
            SpectrumMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_03;
        }

        ISTATUS status = RgbInterpolatorAllocateReflector(rgb_interpolator,
                                                          textels[i][0],
                                                          textels[i][1],
                                                          textels[i][2],
                                                          result->textels + i);

        if (status != ISTATUS_SUCCESS)
        {
            assert(status == ISTATUS_ALLOCATION_FAILED);
            SpectrumMipmapFree(result);
            return status;
        }

        if (color_integrator == NULL)
        {
            continue;
        }

        status = ColorIntegratorPrecomputeReflectorColor(color_integrator,
                                                         result->textels[i]);

        if (status != ISTATUS_SUCCESS)
        {
            assert(status == ISTATUS_ALLOCATION_FAILED);
            SpectrumMipmapFree(result);
            return status;
        }
    }

    *mipmap = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
SpectrumMipmapLookup(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(s))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(t))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (isnan(dsdx))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (isnan(dsdy))
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (isnan(dtdx))
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (isnan(dtdy))
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    float_t x = floor(mipmap->width_fp * s);

    if (x < (float_t)0.0 || mipmap->width <= x)
    {
        *reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    float_t y = floor(mipmap->height_fp * t);

    if (y < (float_t)0.00 || mipmap->height <= y)
    {
        *reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    *reflector = mipmap->textels[(size_t)y * mipmap->width + (size_t)x];

    return ISTATUS_SUCCESS;
}

void
SpectrumMipmapFree(
    _In_opt_ _Post_invalid_ PSPECTRUM_MIPMAP mipmap
    )
{
    if (mipmap == NULL)
    {
        return;
    }

    size_t num_pixels = mipmap->width * mipmap->height;
    for (size_t i = 0; i < num_pixels; i++)
    {
        ReflectorRelease(mipmap->textels[i]);
    }

    free(mipmap->textels);
    free(mipmap);
}

//
// Types
//

struct _FLOAT_MIPMAP {
    _Field_size_(width * height) float_t *textels;
    float_t width_fp;
    float_t height_fp;
    size_t width;
    size_t height;
};

//
// Functions
//

ISTATUS
FloatMipmapAllocate(
    _In_reads_(height * width) float_t textels[],
    _In_ size_t width,
    _In_ size_t height,
    _Out_ PFLOAT_MIPMAP *mipmap
    )
{
    if (textels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    size_t num_pixels;
    bool success = CheckedMultiplySizeT(width, height, &num_pixels);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    PFLOAT_MIPMAP result = (PFLOAT_MIPMAP)malloc(sizeof(FLOAT_MIPMAP));

    if (result == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    float_t *spectra = (float_t*)calloc(num_pixels, sizeof(float_t));

    if (spectra == NULL)
    {
        free(result);
        return ISTATUS_ALLOCATION_FAILED;
    }

    result->textels = spectra;
    result->width = width;
    result->height = height;
    result->width_fp = (float_t)width;
    result->height_fp = (float_t)height;

    for (size_t i = 0; i < num_pixels; i++)
    {
        if (!isfinite(textels[i]) || textels[i] < (float_t)0.0)
        {
            FloatMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_01;
        }

        if (!isfinite(textels[i]) || textels[i] < (float_t)0.0)
        {
            FloatMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_02;
        }

        if (!isfinite(textels[i]) || textels[i] < (float_t)0.0)
        {
            FloatMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_03;
        }

        result->textels[i] = textels[i];
    }

    *mipmap = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
FloatMipmapLookup(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _Out_ float_t *value
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (!isfinite(s))
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (!isfinite(t))
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (isnan(dsdx))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (isnan(dsdy))
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (isnan(dtdx))
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (isnan(dtdy))
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (value == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    float_t x = floor(mipmap->width_fp * s);

    if (x < (float_t)0.0 || mipmap->width <= x)
    {
        *value = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    float_t y = floor(mipmap->height_fp * t);

    if (y < (float_t)0.00 || mipmap->height <= y)
    {
        *value = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    *value = mipmap->textels[(size_t)y * mipmap->width + (size_t)x];

    return ISTATUS_SUCCESS;
}

void
FloatMipmapFree(
    _In_opt_ _Post_invalid_ PFLOAT_MIPMAP mipmap
    )
{
    if (mipmap == NULL)
    {
        return;
    }

    free(mipmap->textels);
    free(mipmap);
}