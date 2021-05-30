/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    perlin_textures.c

Abstract:

    Reimplementation of PBRT textures generated using Perlin noise.

--*/

#include <stdalign.h>
#include <stdlib.h>

#include "iris_physx_toolkit/perlin_textures.h"

//
// Perlin Noise Data
//

const static uint8_t noise_permutations[512] = {
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140,
    36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120,
    234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71,
    134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133,
    230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161,
    1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130,
    116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250,
    124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227,
    47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44,
    154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98,
    108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34,
    242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14,
    239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121,
    50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243,
    141, 128, 195, 78, 66, 215, 61, 156, 180, 151, 160, 137, 91, 90, 15, 131,
    13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37,
    240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252,
    219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125,
    136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158,
    231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245,
    40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187,
    208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198,
    173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126,
    255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223,
    183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167,
    43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185,
    112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179,
    162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199,
    106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236,
    205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156,
    180
};

//
// Perlin Noise Routines
//

static
inline
float_t
Lerp(
    _In_ float_t t,
    _In_ float_t v0,
    _In_ float_t v1
    )
{
    return v0 + t * (v1 - v0);
}

static
inline
float_t
SmoothStep(
    _In_ float_t minimum,
    _In_ float_t maximum,
    _In_ float_t value
    )
{
    float_t v = (value - minimum) / (maximum - minimum);

    v = IMax(v, (float_t)0.0);
    v = IMin(v, (float_t)1.0);

    return v * v * ((float_t)-2.0 * v + (float_t)3.0);
}

static
inline
float_t
Grad(
    _In_ uint8_t x,
    _In_ uint8_t y,
    _In_ uint8_t z,
    _In_ float_t dx,
    _In_ float_t dy,
    _In_ float_t dz
    )
{
    uint8_t h = noise_permutations[x];
    h = noise_permutations[h + y];
    h = noise_permutations[h + z];
    h &= 0x0F;

    float_t u = h < 8 || h == 12 || h == 13 ? dx : dy;
    float_t v = h < 4 || h == 12 || h == 13 ? dy : dz;
    return ((h & 0x01) ? -u : u) + ((h & 0x02) ? -v : v);
}

static
inline
float_t
NoiseWeight(
    _In_ float_t t
    )
{
    float_t t3 = t * t * t;
    float_t t4 = t3 * t;
    return (float_t)6.0 * t4 * t - (float_t)15.0 * t4 + (float_t)10 * t3;
}

static
inline
float_t
Noise(
    _In_ POINT3 point
    )
{
    float_t floor_x = floor(point.x);
    float_t floor_y = floor(point.y);
    float_t floor_z = floor(point.z);

    float_t dx = point.x - floor_x;
    float_t dy = point.y - floor_y;
    float_t dz = point.z - floor_z;

    uint8_t ix = (uint8_t)(int8_t)(int)floor_x;
    uint8_t iy = (uint8_t)(int8_t)(int)floor_y;
    uint8_t iz = (uint8_t)(int8_t)(int)floor_z;

    float_t w000 = Grad(ix, iy, iz, dx, dy, dz);
    float_t w100 = Grad(ix + 1, iy, iz, dx - 1, dy, dz);
    float_t w010 = Grad(ix, iy + 1, iz, dx, dy - 1, dz);
    float_t w110 = Grad(ix + 1, iy + 1, iz, dx - 1, dy - 1, dz);
    float_t w001 = Grad(ix, iy, iz + 1, dx, dy, dz - 1);
    float_t w101 = Grad(ix + 1, iy, iz + 1, dx - 1, dy, dz - 1);
    float_t w011 = Grad(ix, iy + 1, iz + 1, dx, dy - 1, dz - 1);
    float_t w111 = Grad(ix + 1, iy + 1, iz + 1, dx - 1, dy - 1, dz - 1);

    float_t wx = NoiseWeight(dx);
    float_t wy = NoiseWeight(dy);
    float_t wz = NoiseWeight(dz);

    float_t x00 = Lerp(wx, w000, w100);
    float_t x10 = Lerp(wx, w010, w110);
    float_t x01 = Lerp(wx, w001, w101);
    float_t x11 = Lerp(wx, w011, w111);
    float_t y0 = Lerp(wy, x00, x10);
    float_t y1 = Lerp(wy, x01, x11);
    float_t result = Lerp(wz, y0, y1);

    return result;
}

static
inline
float_t
FractionalBrownianNoise(
    _In_ POINT3 point,
    _In_ VECTOR3 dpoint_dx,
    _In_ VECTOR3 dpoint_dy,
    _In_ float_t omega,
    _In_ uint32_t max_octaves
    )
{
    float_t dpoint_dx_length_squared = VectorDotProduct(dpoint_dx, dpoint_dx);
    float_t dpoint_dy_length_squared = VectorDotProduct(dpoint_dy, dpoint_dy);
    float_t length_squared = IMax(dpoint_dx_length_squared,
                                  dpoint_dy_length_squared);

    float_t n = (float_t)-1.0 - (float_t)0.5 * log2(length_squared);
    n = IMax(n, (float_t)0.0);
    n = IMin(n, (float_t)max_octaves);
    float_t n_floor = floor(n);
    uint32_t n_floor_int = (uint32_t)n_floor;

    float_t sum = (float_t)0.0;
    float_t lambda = (float_t)1.0;
    float_t o = (float_t)1.0;

    for (uint32_t i = 0; i < n_floor_int; i++)
    {
        POINT3 p = point;
        p.x *= lambda;
        p.y *= lambda;
        p.z *= lambda;

        sum += o * Noise(p);
        lambda *= (float_t)1.99;
        o *= omega;
    }

    POINT3 p = point;
    p.x *= lambda;
    p.y *= lambda;
    p.z *= lambda;

    float_t n_partial = n - n_floor;
    sum += o * SmoothStep((float_t)0.3, (float_t)0.7, n_partial) * Noise(p);
    return sum;
}

//
// Windy Float Texture Type
//

typedef struct _FLOAT_WINDY_TEXTURE {
    PMATRIX texture_to_world;
} FLOAT_WINDY_TEXTURE, *PFLOAT_WINDY_TEXTURE;

typedef const FLOAT_WINDY_TEXTURE *PCFLOAT_WINDY_TEXTURE;

//
// Windy Float Texture Static Functions
//

static
ISTATUS
WindyFloatSample(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Out_ float_t *value
    )
{
    PFLOAT_WINDY_TEXTURE texture = (PFLOAT_WINDY_TEXTURE)context;

    POINT3 p = PointMatrixInverseMultiply(texture->texture_to_world,
                                          intersection->world_hit_point);

    VECTOR3 dp_dx = VectorMatrixMultiply(texture->texture_to_world,
                                         intersection->world_dp_dx);

    VECTOR3 dp_dy = VectorMatrixMultiply(texture->texture_to_world,
                                         intersection->world_dp_dy);

    float_t wave_height = FractionalBrownianNoise(p,
                                                  dp_dx,
                                                  dp_dy,
                                                  (float_t)0.5,
                                                  6);

    p.x *= (float_t)0.1;
    p.y *= (float_t)0.1;
    p.z *= (float_t)0.1;
    dp_dx = VectorScale(dp_dx, (float_t)0.1);
    dp_dy = VectorScale(dp_dy, (float_t)0.1);

    float_t wind_strength = FractionalBrownianNoise(p,
                                                    dp_dx,
                                                    dp_dy,
                                                    (float_t)0.5,
                                                    3);

    *value = fabs(wind_strength) * wave_height;

    return ISTATUS_SUCCESS;
}

static
void
WindyFloatFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PFLOAT_WINDY_TEXTURE texture = (PFLOAT_WINDY_TEXTURE)context;
    MatrixRelease(texture->texture_to_world);
}

//
// Windy Float Texture Static Variables
//

static const FLOAT_TEXTURE_VTABLE float_windy_texture_vtable = {
    WindyFloatSample,
    WindyFloatFree
};

//
// Windy Reflector Texture Type
//

typedef struct _REFLECTOR_WINDY_TEXTURE {
    PMATRIX texture_to_world;
    PREFLECTOR reflector;
} REFLECTOR_WINDY_TEXTURE, *PREFLECTOR_WINDY_TEXTURE;

typedef const REFLECTOR_WINDY_TEXTURE *PCREFLECTOR_WINDY_TEXTURE;

//
// Windy Reflector Texture Static Functions
//

static
ISTATUS
WindyReflectorSample(
    _In_ const void *context,
    _In_ PCINTERSECTION intersection,
    _In_ const void *additional_data,
    _In_ const void *texture_coordinates,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCREFLECTOR *value
    )
{
    PREFLECTOR_WINDY_TEXTURE texture = (PREFLECTOR_WINDY_TEXTURE)context;

    POINT3 p = PointMatrixInverseMultiply(texture->texture_to_world,
                                          intersection->world_hit_point);
    p.x *= (float_t)0.1;
    p.y *= (float_t)0.1;
    p.z *= (float_t)0.1;

    VECTOR3 dp_dx = VectorMatrixTransposedMultiply(texture->texture_to_world,
                                                   intersection->model_dp_dx);
    dp_dx = VectorScale(dp_dx, (float_t)0.1);

    VECTOR3 dp_dy = VectorMatrixTransposedMultiply(texture->texture_to_world,
                                                   intersection->model_dp_dy);
    dp_dy = VectorScale(dp_dy, (float_t)0.1);

    float_t wind_strength = FractionalBrownianNoise(p,
                                                    dp_dx,
                                                    dp_dy,
                                                    (float_t)0.5,
                                                    3);

    float_t wave_height = FractionalBrownianNoise(p,
                                                  dp_dx,
                                                  dp_dy,
                                                  (float_t)0.5,
                                                  6);

    float_t modulation = fabs(wind_strength) * wave_height;
    ISTATUS status = ReflectorCompositorAttenuateReflector(reflector_compositor,
                                                           texture->reflector,
                                                           modulation,
                                                           value);

    return status;
}

static
void
WindyReflectorFree(
    _In_opt_ _Post_invalid_ void *context
    )
{
    PREFLECTOR_WINDY_TEXTURE texture = (PREFLECTOR_WINDY_TEXTURE)context;
    MatrixRelease(texture->texture_to_world);
    ReflectorRelease(texture->reflector);
}

//
// Windy Reflector Texture Static Variables
//

static const REFLECTOR_TEXTURE_VTABLE reflector_windy_texture_vtable = {
    WindyReflectorSample,
    WindyReflectorFree
};

//
// Functions
//

ISTATUS
WindyFloatTextureAllocate(
    _In_opt_ PMATRIX texture_to_world,
    _Out_ PFLOAT_TEXTURE *texture
    )
{
    if (texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    FLOAT_WINDY_TEXTURE windy_texture;
    windy_texture.texture_to_world = texture_to_world;

    ISTATUS status = FloatTextureAllocate(&float_windy_texture_vtable,
                                          &windy_texture,
                                          sizeof(FLOAT_WINDY_TEXTURE),
                                          alignof(FLOAT_WINDY_TEXTURE),
                                          texture);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    MatrixRetain(texture_to_world);

    return status;
}

ISTATUS
WindyReflectorTextureAllocate(
    _In_opt_ PMATRIX texture_to_world,
    _In_opt_ PREFLECTOR reflector,
    _Out_ PREFLECTOR_TEXTURE *texture
    )
{
    if (texture == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    REFLECTOR_WINDY_TEXTURE windy_texture;
    windy_texture.texture_to_world = texture_to_world;
    windy_texture.reflector = reflector;

    ISTATUS status = ReflectorTextureAllocate(&reflector_windy_texture_vtable,
                                              &windy_texture,
                                              sizeof(REFLECTOR_WINDY_TEXTURE),
                                              alignof(REFLECTOR_WINDY_TEXTURE),
                                              texture);

    if (status != ISTATUS_SUCCESS)
    {
        return status;
    }

    MatrixRetain(texture_to_world);
    ReflectorRetain(reflector);

    return status;
}