/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    mipmap.h

Abstract:

    Creates a mipmap.

--*/

#include "iris_physx_toolkit/mipmap.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "common/safe_math.h"

//
// Types
//

struct _REFLECTOR_MIPMAP {
    _Field_size_(width * height) PREFLECTOR *textels;
    WRAP_MODE wrap_mode;
    float_t width_fp;
    float_t height_fp;
    size_t width;
    size_t height;
};

//
// Static Functions
//

static
bool
ReflectorMipmapAllocate(
    _In_ size_t width,
    _In_ size_t height,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PREFLECTOR_MIPMAP *mipmap
    )
{
    assert(width != 0);
    assert(height != 0);
    assert(mipmap != NULL);
    assert(wrap_mode == WRAP_MODE_REPEAT ||
           wrap_mode == WRAP_MODE_BLACK ||
           wrap_mode == WRAP_MODE_CLAMP);

    size_t num_pixels;
    bool success = CheckedMultiplySizeT(width, height, &num_pixels);

    if (!success)
    {
        return false;
    }

    PREFLECTOR_MIPMAP result =
        (PREFLECTOR_MIPMAP)malloc(sizeof(REFLECTOR_MIPMAP));

    if (result == NULL)
    {
        return false;
    }

    PREFLECTOR *spectra = (PREFLECTOR*)calloc(num_pixels, sizeof(PREFLECTOR));

    if (spectra == NULL)
    {
        free(result);
        return false;
    }

    result->textels = spectra;
    result->wrap_mode = wrap_mode;
    result->width_fp = (float_t)width;
    result->height_fp = (float_t)height;
    result->width = width;
    result->height = height;

    *mipmap = result;

    return true;
}

//
// Functions
//

ISTATUS
ReflectorMipmapAllocateFromFloats(
    _In_reads_(height * width) float_t textels[][3],
    _In_ size_t width,
    _In_ size_t height,
    _In_ WRAP_MODE wrap_mode,
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _Out_ PREFLECTOR_MIPMAP *mipmap
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

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    PREFLECTOR_MIPMAP result;
    bool success = ReflectorMipmapAllocate(width, height, wrap_mode, &result);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < width * height; i++)
    {
        if (!isfinite(textels[i][0]) || textels[i][0] < (float_t)0.0)
        {
            ReflectorMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        if (!isfinite(textels[i][1]) || textels[i][1] < (float_t)0.0)
        {
            ReflectorMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        if (!isfinite(textels[i][2]) || textels[i][2] < (float_t)0.0)
        {
            ReflectorMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        ISTATUS status = ColorExtrapolatorComputeReflector(color_extrapolator,
                                                           textels[i],
                                                           result->textels + i);

        if (status != ISTATUS_SUCCESS)
        {
            ReflectorMipmapFree(result);
            return status;
        }
    }

    *mipmap = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
ReflectorMipmapAllocateFromBytes(
    _In_reads_(height * width) unsigned char textels[][3],
    _In_ size_t width,
    _In_ size_t height,
    _In_ WRAP_MODE wrap_mode,
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _Out_ PREFLECTOR_MIPMAP *mipmap
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

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    PREFLECTOR_MIPMAP result;
    bool success = ReflectorMipmapAllocate(width, height, wrap_mode, &result);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < width * height; i++)
    {
        float_t rgb[3];
        rgb[0] = (float_t)textels[i][0] / (float_t)UCHAR_MAX;
        rgb[1] = (float_t)textels[i][1] / (float_t)UCHAR_MAX;
        rgb[2] = (float_t)textels[i][2] / (float_t)UCHAR_MAX;

        ISTATUS status = ColorExtrapolatorComputeReflector(color_extrapolator,
                                                           rgb,
                                                           result->textels + i);

        if (status != ISTATUS_SUCCESS)
        {
            ReflectorMipmapFree(result);
            return status;
        }
    }

    *mipmap = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
ReflectorMipmapLookup(
    _In_ PCREFLECTOR_MIPMAP mipmap,
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

    if (mipmap->wrap_mode == WRAP_MODE_REPEAT)
    {
#if FLT_EVAL_METHOD	== 0
        float s_intpart, t_intpart;
        s = modff(s, &s_intpart);
        t = modff(t, &t_intpart);
#elif FLT_EVAL_METHOD == 1
        double s_intpart, t_intpart;
        s = modf(s, &s_intpart);
        t = modf(t, &t_intpart);
#elif FLT_EVAL_METHOD == 2
        long double s_intpart, t_intpart;
        s = modfl(s, &s_intpart);
        t = modfl(t, &t_intpart);
#endif

        if (s < (float_t)0.0)
        {
            s = (float_t)1.0 - s;
        }

        if (t < (float_t)0.0)
        {
            t = (float_t)1.0 - t;
        }
    }
    else if (mipmap->wrap_mode == WRAP_MODE_CLAMP)
    {
        s = fmin(fmax((float_t)0.0, s), (float_t)1.0);
        t = fmin(fmax((float_t)0.0, t), (float_t)1.0);
    }
    else if (s < (float_t)0.0 || (float_t)1.0 < s ||
             t < (float_t)0.0 || (float_t)1.0 < t)
    {
        assert(mipmap->wrap_mode == WRAP_MODE_BLACK);
        *reflector = NULL;
        return ISTATUS_SUCCESS;
    }

    size_t x = (size_t)floor(mipmap->width_fp * s);

    if (x == mipmap->width)
    {
        x -= 1;
    }

    size_t y = (size_t)floor(mipmap->height_fp * t);

    if (y == mipmap->height)
    {
        y -= 1;
    }

    *reflector = mipmap->textels[y * mipmap->width + x];

    return ISTATUS_SUCCESS;
}

ISTATUS
ReflectorMipmapCacheColors(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ PCOLOR_CACHE color_cache
    )
{
    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (color_cache == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    size_t num_pixels = mipmap->width * mipmap->height;
    for (size_t i = 0; i < num_pixels; i++)
    {
        ISTATUS status = ColorCacheAddReflector(color_cache,
                                                mipmap->textels[i]);

        if (status != ISTATUS_SUCCESS)
        {
            return status;
        }
    }

    return ISTATUS_SUCCESS;
}

void
ReflectorMipmapFree(
    _In_opt_ _Post_invalid_ PREFLECTOR_MIPMAP mipmap
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
    WRAP_MODE wrap_mode;
    float_t width_fp;
    float_t height_fp;
    size_t width;
    size_t height;
};

//
// Static Functions
//

static
bool
FloatMipmapAllocate(
    _In_ size_t width,
    _In_ size_t height,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_MIPMAP *mipmap
    )
{
    assert(width != 0);
    assert(height != 0);
    assert(mipmap != NULL);
    assert(wrap_mode == WRAP_MODE_REPEAT ||
           wrap_mode == WRAP_MODE_BLACK ||
           wrap_mode == WRAP_MODE_CLAMP);

    size_t num_pixels;
    bool success = CheckedMultiplySizeT(width, height, &num_pixels);

    if (!success)
    {
        return false;
    }

    PFLOAT_MIPMAP result = (PFLOAT_MIPMAP)malloc(sizeof(FLOAT_MIPMAP));

    if (result == NULL)
    {
        return false;
    }

    float_t *spectra = (float_t*)calloc(num_pixels, sizeof(float_t));

    if (spectra == NULL)
    {
        free(result);
        return false;
    }

    result->textels = spectra;
    result->wrap_mode = wrap_mode;
    result->width_fp = (float_t)width;
    result->height_fp = (float_t)height;
    result->width = width;
    result->height = height;

    *mipmap = result;

    return true;
}

//
// Functions
//

ISTATUS
FloatMipmapAllocateFromFloats(
    _In_reads_(height * width) float_t textels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ WRAP_MODE wrap_mode,
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

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    PFLOAT_MIPMAP result;
    bool success = FloatMipmapAllocate(width, height, wrap_mode, &result);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < width * height; i++)
    {
        if (!isfinite(textels[i]) || textels[i] < (float_t)0.0)
        {
            FloatMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        result->textels[i] = textels[i];
    }

    *mipmap = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
FloatMipmapAllocateFromBytes(
    _In_reads_(height * width) unsigned char textels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ WRAP_MODE wrap_mode,
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

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    PFLOAT_MIPMAP result;
    bool success = FloatMipmapAllocate(width, height, wrap_mode, &result);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < width * height; i++)
    {
        result->textels[i] = (float_t)textels[i] / (float_t)UCHAR_MAX;
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

    if (mipmap->wrap_mode == WRAP_MODE_REPEAT)
    {
#if FLT_EVAL_METHOD	== 0
        float s_intpart, t_intpart;
        s = modff(s, &s_intpart);
        t = modff(t, &t_intpart);
#elif FLT_EVAL_METHOD == 1
        double s_intpart, t_intpart;
        s = modf(s, &s_intpart);
        t = modf(t, &t_intpart);
#elif FLT_EVAL_METHOD == 2
        long double s_intpart, t_intpart;
        s = modfl(s, &s_intpart);
        t = modfl(t, &t_intpart);
#endif

        if (s < (float_t)0.0)
        {
            s = (float_t)1.0 - s;
        }

        if (t < (float_t)0.0)
        {
            t = (float_t)1.0 - t;
        }
    }
    else if (mipmap->wrap_mode == WRAP_MODE_CLAMP)
    {
        s = fmin(fmax((float_t)0.0, s), (float_t)1.0);
        t = fmin(fmax((float_t)0.0, t), (float_t)1.0);
    }
    else if (s < (float_t)0.0 || (float_t)1.0 < s ||
             t < (float_t)0.0 || (float_t)1.0 < t)
    {
        assert(mipmap->wrap_mode == WRAP_MODE_BLACK);
        *value = (float_t)0.0;
        return ISTATUS_SUCCESS;
    }

    size_t x = (size_t)floor(mipmap->width_fp * s);

    if (x == mipmap->width)
    {
        x -= 1;
    }

    size_t y = (size_t)floor(mipmap->height_fp * t);

    if (y == mipmap->height)
    {
        y -= 1;
    }

    *value = mipmap->textels[y * mipmap->width + x];

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