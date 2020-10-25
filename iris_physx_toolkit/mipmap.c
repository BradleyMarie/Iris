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

typedef struct _REFLECTOR_MIPMAP_LEVEL {
    _Field_size_(width * height) PREFLECTOR *texels;
    size_t width;
    size_t height;
    float_t width_fp;
    float_t height_fp;
    float_t texel_width;
    float_t texel_height;
} REFLECTOR_MIPMAP_LEVEL, *PREFLECTOR_MIPMAP_LEVEL;

typedef const struct REFLECTOR_MIPMAP_LEVEL *PCREFLECTOR_MIPMAP_LEVEL;

struct _REFLECTOR_MIPMAP {
    _Field_size_(num_levels) PREFLECTOR_MIPMAP_LEVEL levels;
    size_t num_levels;
    TEXTURE_FILTERING_ALGORITHM texture_filtering;
    WRAP_MODE wrap_mode;
    float_t last_level_index_fp;
};

//
// Static Functions
//

static
inline
size_t
SizeTLog2(
    _In_ size_t value
    )
{
    assert(value != 0 && (value & (value - 1)) == 0);

    value >>= 1;

    size_t result = 0;
    while (value != 0)
    {
        value >>= 1;
        result += 1;
    }

    return result;
}

static
inline
float_t
FloatTLog2(
    _In_ float_t value
    )
{
    float_t inv_log2 = (float_t)1.442695040888963387004650940071;
    return log(value) * inv_log2;
}

_Ret_writes_maybenull_(width * height / 4)
static
PCOLOR3
DownsampleColors(
    _In_reads_(width * height) PCCOLOR3 texels,
    _In_ size_t width,
    _In_ size_t height,
    _In_ size_t *new_width,
    _In_ size_t *new_height
    )
{
    assert(texels != NULL);
    assert(width != 0 && (width & (width - 1)) == 0);
    assert(height != 0 && (height & (height - 1)) == 0);

    *new_width = width / 2;
    *new_height = height / 2;

    PCOLOR3 colors =
        (PCOLOR3)calloc(*new_width * *new_height, sizeof(COLOR3));

    if (colors == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < *new_height; i++)
    {
        for (size_t j = 0; j < *new_width; j++)
        {
            size_t source_row = i * 2;
            size_t source_column = j * 2;

            COLOR3 color = texels[source_row * width + source_column];

            color = ColorAdd(color,
                             texels[source_row * width + source_column + 1],
                             color.color_space);

            source_row += 1;

            color = ColorAdd(color,
                             texels[source_row * width + source_column],
                             color.color_space);

            color = ColorAdd(color,
                             texels[source_row * width + source_column + 1],
                             color.color_space);

            colors[i * *new_width + j] = ColorScale(color, (float_t)0.25);
        }
    }

    return colors;
}

static
PCREFLECTOR
ReflectorMipmapLookupTexel(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ size_t level,
    _In_ float_t s,
    _In_ float_t t
    )
{
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
            s = (float_t)1.0 + s;
        }

        if (t < (float_t)0.0)
        {
            t = (float_t)1.0 + t;
        }
    }
    else if (mipmap->wrap_mode == WRAP_MODE_CLAMP)
    {
        s = IMin(IMax((float_t)0.0, s), (float_t)1.0);
        t = IMin(IMax((float_t)0.0, t), (float_t)1.0);
    }
    else if (s < (float_t)0.0 || (float_t)1.0 < s ||
             t < (float_t)0.0 || (float_t)1.0 < t)
    {
        assert(mipmap->wrap_mode == WRAP_MODE_BLACK);
        return NULL;
    }

    size_t x = (size_t)floor(mipmap->levels[level].width_fp * s);

    if (x == mipmap->levels[level].width)
    {
        x -= 1;
    }

    size_t y = (size_t)floor(mipmap->levels[level].height_fp * t);

    if (y == mipmap->levels[level].height)
    {
        y -= 1;
    }

    return mipmap->levels[level].texels[y * mipmap->levels[level].width + x];
}

static
ISTATUS
ReflectorMipmapLookupWithTriangleFilter(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ size_t level,
    _In_ float_t s,
    _In_ float_t t,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    if (mipmap->num_levels <= level)
    {
        level = mipmap->num_levels - 1;
    }

    float_t scaled_s = s * mipmap->levels[level].width_fp;
    float_t scaled_t = t * mipmap->levels[level].height_fp;

    float_t scaled_s0 = floor(scaled_s - (float_t)0.5) + (float_t)0.5;
    float_t scaled_t0 = floor(scaled_t - (float_t)0.5) + (float_t)0.5;

    float_t s0 = scaled_s0 * mipmap->levels[level].texel_width;
    float_t t0 = scaled_t0 * mipmap->levels[level].texel_height;

    float_t ds = scaled_s - scaled_s0;
    float_t dt = scaled_t - scaled_t0;

    float_t one_minus_ds = (float_t)1.0 - ds;
    float_t one_minus_dt = (float_t)1.0 - dt;

    float_t s1 = s0 + mipmap->levels[level].texel_width;
    float_t t1 = t0 + mipmap->levels[level].texel_height;

    PCREFLECTOR texel = ReflectorMipmapLookupTexel(mipmap, level, s0, t0);

    PCREFLECTOR result;
    ISTATUS status =
        ReflectorCompositorAttenuateReflector(compositor,
                                              texel,
                                              one_minus_ds * one_minus_dt,
                                              &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    texel = ReflectorMipmapLookupTexel(mipmap, level, s0, t1);

    status =
        ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                   result,
                                                   texel,
                                                   one_minus_ds * dt,
                                                   &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    texel = ReflectorMipmapLookupTexel(mipmap, level, s1, t0);

    status =
        ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                   result,
                                                   texel,
                                                   ds * one_minus_dt,
                                                   &result);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    texel = ReflectorMipmapLookupTexel(mipmap, level, s1, t1);

    status =
        ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                   result,
                                                   texel,
                                                   ds * dt,
                                                   reflector);

    return status;
}

ISTATUS
ReflectorMipmapLookupTextureFilteringNone(
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
    *reflector = ReflectorMipmapLookupTexel(mipmap, 0, s, t);
    return ISTATUS_SUCCESS;
}

static
ISTATUS
ReflectorMipmapLookupTextureFilteringTrilinear(
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
    dsdx = fabs(dsdx);
    dsdy = fabs(dsdy);
    dtdx = fabs(dtdx);
    dtdy = fabs(dtdy);

    float_t max = IMax(dsdx, IMax(dsdy, IMax(dtdx, dtdy)));
    float_t level =
        mipmap->last_level_index_fp + FloatTLog2(IMax(max, (float_t)1e-8));

    if (level < (float_t)0.0)
    {
        ISTATUS status =
            ReflectorMipmapLookupWithTriangleFilter(mipmap,
                                                    0,
                                                    s,
                                                    t,
                                                    compositor,
                                                    reflector);

        return status;
    }

    if (level >= mipmap->last_level_index_fp)
    {
        ISTATUS status =
            ReflectorMipmapLookupWithTriangleFilter(mipmap,
                                                    mipmap->num_levels - 1,
                                                    s,
                                                    t,
                                                    compositor,
                                                    reflector);

        return status;
    }

#if FLT_EVAL_METHOD	== 0
    float delta, level0;
    delta = modff(level, &level0);
#elif FLT_EVAL_METHOD == 1
    double delta, level0;
    delta = modf(level, &level0);
#elif FLT_EVAL_METHOD == 2
    long double delta, level0;
    delta = modfl(level, &level0);
#endif

    PCREFLECTOR reflector0;
    ISTATUS status =
        ReflectorMipmapLookupWithTriangleFilter(mipmap,
                                                (size_t)level0,
                                                s,
                                                t,
                                                compositor,
                                                &reflector0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    PCREFLECTOR reflector1;
    status =
        ReflectorMipmapLookupWithTriangleFilter(mipmap,
                                                (size_t)level0 + 1,
                                                s,
                                                t,
                                                compositor,
                                                &reflector1);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorCompositorAttenuateReflector(compositor,
                                                   reflector0,
                                                   delta,
                                                   &reflector0);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    status = ReflectorCompositorAttenuatedAddReflectors(compositor,
                                                        reflector0,
                                                        reflector1,
                                                        (float_t)1.0 - delta,
                                                        reflector);

    return status;
}

static
bool
ReflectorMipmapAllocateInternal(
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PREFLECTOR_MIPMAP *mipmap
    )
{
    assert(width != 0);
    assert(height != 0);
    assert(mipmap != NULL);
    assert(texture_filtering == TEXTURE_FILTERING_ALGORITHM_NONE ||
           texture_filtering == TEXTURE_FILTERING_ALGORITHM_TRILINEAR);
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

    size_t width_log_2 = SizeTLog2(width);
    size_t height_log_2 = SizeTLog2(height);

    size_t num_levels = 1;
    if (width_log_2 < height_log_2)
    {
        num_levels += width_log_2;
    }
    else
    {
        num_levels += height_log_2;
    }

    PREFLECTOR_MIPMAP_LEVEL levels =
        (PREFLECTOR_MIPMAP_LEVEL)calloc(num_levels, sizeof(REFLECTOR_MIPMAP_LEVEL));

    if (levels == NULL)
    {
        free(result);
        return false;
    }

    result->levels = levels;
    result->num_levels = num_levels;
    result->texture_filtering = texture_filtering;
    result->wrap_mode = wrap_mode;
    result->last_level_index_fp = num_levels - 1;

    for (size_t i = 0; i < num_levels; i++)
    {
        PREFLECTOR *texels =
            (PREFLECTOR*)calloc(width * height, sizeof(PREFLECTOR));

        if (texels == NULL)
        {
            ReflectorMipmapFree(result);
            return false;
        }

        levels[i].texels = texels;
        levels[i].width = width;
        levels[i].height = height;
        levels[i].width_fp = (float_t)width;
        levels[i].height_fp = (float_t)height;
        levels[i].texel_width = (float_t)1.0 / (float_t)width;
        levels[i].texel_height = (float_t)1.0 / (float_t)height;

        width /= 2;
        height /= 2;
    }

    *mipmap = result;

    return true;
}

//
// Functions
//

ISTATUS
ReflectorMipmapAllocate(
    _In_reads_(height * width) const COLOR3 texels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ WRAP_MODE wrap_mode,
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _Out_ PREFLECTOR_MIPMAP *mipmap
    )
{
    if (texels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0 || (width & (width - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0 || (height & (height - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (texture_filtering != TEXTURE_FILTERING_ALGORITHM_NONE &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_TRILINEAR)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (color_extrapolator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    PREFLECTOR_MIPMAP result;
    bool success = ReflectorMipmapAllocateInternal(width,
                                                   height,
                                                   texture_filtering,
                                                   wrap_mode,
                                                   &result);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < width * height; i++)
    {
        if (!ColorValidate(texels[i]))
        {
            ReflectorMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        ISTATUS status = ColorExtrapolatorComputeReflector(color_extrapolator,
                                                           texels[i],
                                                           result->levels[0].texels + i);

        if (status != ISTATUS_SUCCESS)
        {
            ReflectorMipmapFree(result);
            return status;
        }
    }

    PCOLOR3 working = NULL;
    PCCOLOR3 working_const = texels;
    for (size_t i = 1; i < result->num_levels; i++)
    {
        PCOLOR3 new_working = DownsampleColors(working_const,
                                               result->levels[i - 1].width,
                                               result->levels[i - 1].height,
                                               &result->levels[i].width,
                                               &result->levels[i].height);

        free(working);

        if (new_working == NULL)
        {
            ReflectorMipmapFree(result);
            return ISTATUS_ALLOCATION_FAILED;
        }

        working_const = new_working;
        working = new_working;

        size_t num_samples = result->levels[i].height * result->levels[i].width;

        for (size_t j = 0; j < num_samples; j++)
        {
            ISTATUS status =
                ColorExtrapolatorComputeReflector(color_extrapolator,
                                                  working[j],
                                                  result->levels[i].texels + j);

            if (status != ISTATUS_SUCCESS)
            {
                free(working);
                ReflectorMipmapFree(result);
                return status;
            }
        }
    }

    free(working);

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

    if (!isfinite(dsdx))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(dsdy))
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (!isfinite(dtdx))
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (!isfinite(dtdy))
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

    if (mipmap->texture_filtering == TEXTURE_FILTERING_ALGORITHM_NONE)
    {
        ISTATUS status =
            ReflectorMipmapLookupTextureFilteringNone(mipmap,
                                                      s,
                                                      t,
                                                      dsdx,
                                                      dsdy,
                                                      dtdx,
                                                      dtdy,
                                                      compositor,
                                                      reflector);

        return status;
    }

    assert(mipmap->texture_filtering == TEXTURE_FILTERING_ALGORITHM_TRILINEAR);
    ISTATUS status =
        ReflectorMipmapLookupTextureFilteringTrilinear(mipmap,
                                                        s,
                                                        t,
                                                        dsdx,
                                                        dsdy,
                                                        dtdx,
                                                        dtdy,
                                                        compositor,
                                                        reflector);

    return status;
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

    for (size_t i = 0; i < mipmap->num_levels; i++)
    {
        for (size_t j = 0;
             j < mipmap->levels[i].height * mipmap->levels[i].width;
             j++)
        {
            ReflectorRelease(mipmap->levels[i].texels[j]);
        }

        free(mipmap->levels[i].texels);
    }

    free(mipmap->levels);
    free(mipmap);
}

//
// Types
//

typedef struct _FLOAT_MIPMAP_LEVEL {
    _Field_size_(width * height) float_t *texels;
    size_t width;
    size_t height;
    float_t width_fp;
    float_t height_fp;
    float_t texel_width;
    float_t texel_height;
} FLOAT_MIPMAP_LEVEL, *PFLOAT_MIPMAP_LEVEL;

typedef const struct FLOAT_MIPMAP_LEVEL *PCFLOAT_MIPMAP_LEVEL;

struct _FLOAT_MIPMAP {
    _Field_size_(num_levels) PFLOAT_MIPMAP_LEVEL levels;
    size_t num_levels;
    TEXTURE_FILTERING_ALGORITHM texture_filtering;
    WRAP_MODE wrap_mode;
    float_t last_level_index_fp;
};

//
// Static Functions
//

static
bool
FloatMipmapAllocate(
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_MIPMAP *mipmap
    )
{
    assert(width != 0);
    assert(height != 0);
    assert(mipmap != NULL);
    assert(texture_filtering == TEXTURE_FILTERING_ALGORITHM_NONE ||
           texture_filtering == TEXTURE_FILTERING_ALGORITHM_TRILINEAR);
    assert(wrap_mode == WRAP_MODE_REPEAT ||
           wrap_mode == WRAP_MODE_BLACK ||
           wrap_mode == WRAP_MODE_CLAMP);

    size_t num_pixels;
    bool success = CheckedMultiplySizeT(width, height, &num_pixels);

    if (!success)
    {
        return false;
    }

    PFLOAT_MIPMAP result =
        (PFLOAT_MIPMAP)malloc(sizeof(FLOAT_MIPMAP));

    if (result == NULL)
    {
        return false;
    }

    size_t width_log_2 = SizeTLog2(width);
    size_t height_log_2 = SizeTLog2(height);

    size_t num_levels = 1;
    if (width_log_2 < height_log_2)
    {
        num_levels += width_log_2;
    }
    else
    {
        num_levels += height_log_2;
    }

    PFLOAT_MIPMAP_LEVEL levels =
        (PFLOAT_MIPMAP_LEVEL)calloc(num_levels, sizeof(FLOAT_MIPMAP_LEVEL));

    if (levels == NULL)
    {
        free(result);
        return false;
    }

    result->levels = levels;
    result->num_levels = num_levels;
    result->texture_filtering = texture_filtering;
    result->wrap_mode = wrap_mode;
    result->last_level_index_fp = num_levels - 1;

    for (size_t i = 0; i < num_levels; i++)
    {
        float_t *texels =
            (float_t*)calloc(width * height, sizeof(float_t));

        if (texels == NULL)
        {
            FloatMipmapFree(result);
            return false;
        }

        levels[i].texels = texels;
        levels[i].width = width;
        levels[i].height = height;
        levels[i].width_fp = (float_t)width;
        levels[i].height_fp = (float_t)height;
        levels[i].texel_width = (float_t)1.0 / (float_t)width;
        levels[i].texel_height = (float_t)1.0 / (float_t)height;

        width >>= 1;
        height >>= 1;
    }

    *mipmap = result;

    return true;
}

_Ret_writes_maybenull_(width * height / 4)
static
float_t*
DownsampleFloats(
    _In_reads_(width * height) const float_t *texels,
    _In_ size_t width,
    _In_ size_t height,
    _Out_ size_t* new_height,
    _Out_ size_t* new_width
    )
{
    assert(texels != NULL);
    assert(width != 0 && (width & (width - 1)) == 0);
    assert(height != 0 && (height & (height - 1)) == 0);

    *new_width = width / 2;
    *new_height = height / 2;

    float_t *values = 
        (float_t*)calloc(*new_width * *new_height, sizeof(float_t));

    if (values == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < *new_height; i++)
    {
        for (size_t j = 0; j < *new_width; j++)
        {
            size_t source_row = i * 2;
            size_t source_column = j * 2;

            float_t value =
                texels[source_row * width + source_column] +
                texels[source_row * width + source_column + 1] +
                texels[source_row * (width + 1) + source_column] +
                texels[source_row * (width + 1) + source_column + 1];

            values[i * *new_width + j] = value * (float_t)0.25;
        }
    }

    return values;
}

static
float_t
FloatMipmapLookupTexel(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ size_t level,
    _In_ float_t s,
    _In_ float_t t
    )
{
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
            s = (float_t)1.0 + s;
        }

        if (t < (float_t)0.0)
        {
            t = (float_t)1.0 + t;
        }
    }
    else if (mipmap->wrap_mode == WRAP_MODE_CLAMP)
    {
        s = IMin(IMax((float_t)0.0, s), (float_t)1.0);
        t = IMin(IMax((float_t)0.0, t), (float_t)1.0);
    }
    else if (s < (float_t)0.0 || (float_t)1.0 < s ||
             t < (float_t)0.0 || (float_t)1.0 < t)
    {
        assert(mipmap->wrap_mode == WRAP_MODE_BLACK);
        return (float_t)0.0;
    }

    size_t x = (size_t)floor(mipmap->levels[level].width_fp * s);

    if (x == mipmap->levels[level].width)
    {
        x -= 1;
    }

    size_t y = (size_t)floor(mipmap->levels[level].height_fp * t);

    if (y == mipmap->levels[level].height)
    {
        y -= 1;
    }

    return mipmap->levels[level].texels[y * mipmap->levels[level].width + x];
}

static
float_t
FloatMipmapLookupWithTriangleFilter(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ size_t level,
    _In_ float_t s,
    _In_ float_t t
    )
{
    if (mipmap->num_levels <= level)
    {
        level = mipmap->num_levels - 1;
    }

    float_t scaled_s = s * mipmap->levels[level].width_fp;
    float_t scaled_t = t * mipmap->levels[level].height_fp;

    float_t scaled_s0 = floor(scaled_s - (float_t)0.5) + (float_t)0.5;
    float_t scaled_t0 = floor(scaled_t - (float_t)0.5) + (float_t)0.5;

    float_t s0 = scaled_s0 * mipmap->levels[level].texel_width;
    float_t t0 = scaled_t0 * mipmap->levels[level].texel_height;

    float_t ds = scaled_s - scaled_s0;
    float_t dt = scaled_t - scaled_t0;

    float_t one_minus_ds = (float_t)1.0 - ds;
    float_t one_minus_dt = (float_t)1.0 - dt;

    float_t s1 = s0 + mipmap->levels[level].texel_width;
    float_t t1 = t0 + mipmap->levels[level].texel_height;

    float_t result =
        (one_minus_ds * one_minus_dt * FloatMipmapLookupTexel(mipmap, level, s0, t0)) +
        (one_minus_ds * dt * FloatMipmapLookupTexel(mipmap, level, s0, t1)) +
        (ds * one_minus_dt * FloatMipmapLookupTexel(mipmap, level, s1, t0)) +
        (ds * dt * FloatMipmapLookupTexel(mipmap, level, s1, t1));

    return result;
}

static
void
FloatMipmapLookupTextureFilteringNone(
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
    *value = FloatMipmapLookupTexel(mipmap, 0, s, t);
}

static
void
FloatMipmapLookupTextureFilteringTrilinear(
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
    dsdx = fabs(dsdx);
    dsdy = fabs(dsdy);
    dtdx = fabs(dtdx);
    dtdy = fabs(dtdy);

    float_t max = IMax(dsdx, IMax(dsdy, IMax(dtdx, dtdy)));
    float_t level =
        mipmap->last_level_index_fp + FloatTLog2(IMax(max, (float_t)1e-8));

    if (level < (float_t)0.0)
    {
        *value = FloatMipmapLookupWithTriangleFilter(mipmap, 0, s, t);
    }
    else if (level >= mipmap->last_level_index_fp)
    {
        *value = FloatMipmapLookupWithTriangleFilter(mipmap,
                                                     mipmap->num_levels - 1,
                                                     s,
                                                     t);
    }
    else
    {
#if FLT_EVAL_METHOD	== 0
        float delta, level0;
        delta = modff(level, &level0);
#elif FLT_EVAL_METHOD == 1
        double delta, level0;
        delta = modf(level, &level0);
#elif FLT_EVAL_METHOD == 2
        long double delta, level0;
        delta = modfl(level, &level0);
#endif

        float_t value0 =
            FloatMipmapLookupWithTriangleFilter(mipmap,
                                                (size_t)level0,
                                                s,
                                                t);

        float_t value1 =
            FloatMipmapLookupWithTriangleFilter(mipmap,
                                                (size_t)level0 + 1,
                                                s,
                                                t);

        *value = delta * value0 + ((float_t)1.0 - delta) * value1;
    }
}

//
// Functions
//

ISTATUS
FloatMipmapAllocateFromFloats(
    _In_reads_(height * width) const float_t texels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_MIPMAP *mipmap
    )
{
    if (texels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0 || (width & (width - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0 || (height & (height - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (texture_filtering != TEXTURE_FILTERING_ALGORITHM_NONE &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_TRILINEAR)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    PFLOAT_MIPMAP result;
    bool success = FloatMipmapAllocate(width,
                                       height,
                                       texture_filtering,
                                       wrap_mode,
                                       &result);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < width * height; i++)
    {
        if (!isfinite(texels[i]) || texels[i] < (float_t)0.0)
        {
            FloatMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        result->levels[0].texels[i] = texels[i];
    }

    float_t *working = NULL;
    const float_t *working_const = texels;
    for (size_t i = 1; i < result->num_levels; i++)
    {
        float_t *new_working = DownsampleFloats(working_const, 
                                                result->levels[i - 1].width,
                                                result->levels[i - 1].height,
                                                &result->levels[i].width,
                                                &result->levels[i].height);

        free(working);

        if (new_working == NULL)
        {
            FloatMipmapFree(result);
            return ISTATUS_ALLOCATION_FAILED;
        }

        working_const = new_working;
        working = new_working;

        for (size_t j = 0;
             j < result->levels[i].height * result->levels[i].width;
             j++)
        {
            result->levels[i].texels[j] = working[j];
        }
    }

    free(working);

    *mipmap = result;

    return ISTATUS_SUCCESS;
}

ISTATUS
FloatMipmapAllocateFromLuma(
    _In_reads_(height * width) const COLOR3 texels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_MIPMAP *mipmap
    )
{
    if (texels == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (width == 0 || (width & (width - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (height == 0 || (height & (height - 1)) != 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (texture_filtering != TEXTURE_FILTERING_ALGORITHM_NONE &&
        texture_filtering != TEXTURE_FILTERING_ALGORITHM_TRILINEAR)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (wrap_mode != WRAP_MODE_REPEAT &&
        wrap_mode != WRAP_MODE_BLACK &&
        wrap_mode != WRAP_MODE_CLAMP)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (mipmap == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    PFLOAT_MIPMAP result;
    bool success = FloatMipmapAllocate(width,
                                       height,
                                       texture_filtering,
                                       wrap_mode,
                                       &result);

    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    for (size_t i = 0; i < width * height; i++)
    {
        if (!ColorValidate(texels[i]))
        {
            FloatMipmapFree(result);
            return ISTATUS_INVALID_ARGUMENT_00;
        }

        COLOR3 color = ColorConvert(texels[i], COLOR_SPACE_XYZ);
        result->levels[0].texels[i] = color.values[1];
    }

    float_t *working = NULL;
    const float_t *working_const = result->levels[0].texels;
    for (size_t i = 1; i < result->num_levels; i++)
    {
        float_t *new_working = DownsampleFloats(working_const, 
                                                result->levels[i - 1].width,
                                                result->levels[i - 1].height,
                                                &result->levels[i].width,
                                                &result->levels[i].height);

        free(working);

        if (new_working == NULL)
        {
            FloatMipmapFree(result);
            return ISTATUS_ALLOCATION_FAILED;
        }

        working_const = new_working;
        working = new_working;

        for (size_t j = 0;
             j < result->levels[i].height * result->levels[i].width;
             j++)
        {
            result->levels[i].texels[j] = working[j];
        }
    }

    free(working);

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

    if (!isfinite(dsdx))
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (!isfinite(dsdy))
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (!isfinite(dtdx))
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (!isfinite(dtdy))
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (value == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (mipmap->texture_filtering == TEXTURE_FILTERING_ALGORITHM_NONE)
    {
        FloatMipmapLookupTextureFilteringNone(mipmap,
                                              s,
                                              t,
                                              dsdx,
                                              dsdy,
                                              dtdx,
                                              dtdy,
                                              value);
    }
    else
    {
        assert(mipmap->texture_filtering == TEXTURE_FILTERING_ALGORITHM_TRILINEAR);
        FloatMipmapLookupTextureFilteringTrilinear(mipmap,
                                                   s,
                                                   t,
                                                   dsdx,
                                                   dsdy,
                                                   dtdx,
                                                   dtdy,
                                                   value);
    }

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

    for (size_t i = 0; i < mipmap->num_levels; i++)
    {
        free(mipmap->levels[i].texels);
    }

    free(mipmap->levels);
    free(mipmap);
}