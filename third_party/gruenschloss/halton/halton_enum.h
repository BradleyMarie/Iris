// Copyright (c) 2012 Leonhard Gruenschloss (leonhard@gruenschloss.org)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef HALTON_ENUM_H
#define HALTON_ENUM_H

#include <assert.h>

// Determine the index of the i-th sample falling into a pixel, based on the
// elementary interval property of the Halton sequence.
// This is an implementation of the two-dimensional case of the more general
// construction in L. Gruenschloss, M. Raab, and A. Keller: "Enumerating Quasi-Monte
// Carlo Point Sequences in Elementary Intervals".
// This assumes that identity digit permutations are used for the first two components,
// i.e. basis 2 and 3.

typedef struct Halton_enum_ {
    unsigned m_p2; // Smallest integer with 2^m_p2 >= width.
    unsigned m_p3; // Smallest integer with 3^m_p3 >= height.
    unsigned m_x; // 3^m_p3 * ((2^m_p2)^(-1) mod 3^m_p3).
    unsigned m_y; // 2^m_p2 * ((3^m_p3)^(-1) mod 2^m_p2).
    float m_scale_x; // 2^m_p2.
    float m_scale_y; // 3^m_p3.
    unsigned m_increment; // Product of prime powers, i.e. m_res2 * m_res3.
} Halton_enum;

// Initialize the enumeration for the given resolution.
static inline Halton_enum halton_enum(unsigned width, unsigned height);

// Return how many samples per pixel can be queried before sample index overflow occurs.
static inline unsigned get_max_samples_per_pixel(const Halton_enum* object) { return ~0u / object->m_increment; }

// Return the index of the i-th sample falling into the given pixel (x, y) within the
// previously given resolution bounds. i must be smaller than the value returned by
// get_max_samples_per_pixel.
static inline unsigned get_index(const Halton_enum* object, unsigned i, unsigned x, unsigned y);

// Scale the x-component of a sample in [0,1) to [0,width).
static inline float scale_x(const Halton_enum* object, float x);

// Scale the y-component of a sample in [0,1) to [0,height).
static inline float scale_y(const Halton_enum* object, float y);

static inline void extended_euclid(int a, int b, int* first, int* second);
static inline unsigned halton2_inverse(unsigned i, unsigned digits);
static inline unsigned halton3_inverse(unsigned i, unsigned digits);

static inline Halton_enum halton_enum(const unsigned width, const unsigned height)
{
    assert(width && height);

    Halton_enum result;

    result.m_p2 = 0;
    unsigned w = 1;
    while (w < width) // Find 2^m_p2 >= width.
    {
        ++result.m_p2;
        w *= 2;
    }
    result.m_scale_x = (float)w;

    result.m_p3 = 0;
    unsigned h = 1;
    while (h < height) // Find 3^m_p3 >= height.
    {
        ++result.m_p3;
        h *= 3;
    }
    result.m_scale_y = (float)h;

    result.m_increment = w * h; // There's exactly one sample per pixel.

    // Determine the multiplicative inverses.
    int first, second;
    extended_euclid((int)h, (int)w, &first, &second);
    const unsigned inv2 = (first < 0) ? (first + w) : (first % w);
    const unsigned inv3 = (second < 0) ? (second + h) : (second % h);
    assert((!inv2 && w == 1) || (inv2 > 0 && (h * inv2) % w == 1));
    assert((!inv3 && h == 1) || (inv3 > 0 && (w * inv3) % h == 1));
    result.m_x = h * inv2;
    result.m_y = w * inv3;

    return result;
}

static inline unsigned get_index(const Halton_enum* object, const unsigned i, const unsigned x, const unsigned y)
{
    // Promote to 64 bits to avoid overflow.
    const unsigned long long hx = halton2_inverse(x, object->m_p2);
    const unsigned long long hy = halton3_inverse(y, object->m_p3);
    // Apply Chinese remainder theorem.
    const unsigned offset = (unsigned)((hx * object->m_x + hy * object->m_y) % object->m_increment);
    return offset + i * object->m_increment;
}

static inline float scale_x(const Halton_enum* object, const float x)
{
    return x * object->m_scale_x;
}

static inline float scale_y(const Halton_enum* object, const float y)
{
    return y * object->m_scale_y;
}

static inline void extended_euclid(const int a, const int b, int* first, int* second)
{
    if (!b)
    {
        *first = 1u;
        *second = 0u;
        return;
    }
    const int q = a / b;
    const int r = a % b;
    int st_first, st_second;
    extended_euclid(b, r, &st_first, &st_second);
    *first = st_second;
    *second = st_first - q * st_second;
}

static inline unsigned halton2_inverse(unsigned index, const unsigned digits)
{
    index = (index << 16) | (index >> 16);
    index = ((index & 0x00ff00ff) << 8) | ((index & 0xff00ff00) >> 8);
    index = ((index & 0x0f0f0f0f) << 4) | ((index & 0xf0f0f0f0) >> 4);
    index = ((index & 0x33333333) << 2) | ((index & 0xcccccccc) >> 2);
    index = ((index & 0x55555555) << 1) | ((index & 0xaaaaaaaa) >> 1);
    return index >> (32 - digits);
}

static inline unsigned halton3_inverse(unsigned index, const unsigned digits)
{
    unsigned result = 0;
    for (unsigned d = 0; d < digits; ++d)
    {
        result = result * 3 + index % 3;
        index /= 3;
    }
    return result;
}

#endif // HALTON_ENUM_H

