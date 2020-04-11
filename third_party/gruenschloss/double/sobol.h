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

#ifndef SOBOL_DOUBLE_H
#define SOBOL_DOUBLE_H

#include <assert.h>

#define SOBOL_DOUBLE_DEFAULT_SCRAMBLE 0ULL

#define SOBOL_DOUBLE_NUM_DIMENSIONS 1024
#define SOBOL_DOUBLE_SIZE 52
#define SOBOL_DOUBLE_MATRIX_SIZE (SOBOL_DOUBLE_NUM_DIMENSIONS * SOBOL_DOUBLE_SIZE)

const extern unsigned long long sobol_double_matrices[SOBOL_DOUBLE_MATRIX_SIZE];

// Compute one component of the Sobol'-sequence, where the component
// corresponds to the dimension parameter, and the index specifies
// the point inside the sequence. The scramble parameter can be used
// to permute elementary intervals, and might be chosen randomly to
// generate a randomized QMC sequence. Only the Matrices::size least
// significant bits of the scramble value are used.
static inline double sobol_double_sample(
    unsigned long long index,
    const unsigned dimension,
    const unsigned long long scramble)
{
    assert(dimension < SOBOL_DOUBLE_NUM_DIMENSIONS);

    unsigned long long result = scramble & ~-(1ULL << SOBOL_DOUBLE_SIZE);
    for (unsigned i = dimension * SOBOL_DOUBLE_SIZE; index; index >>= 1, ++i)
    {
        if (index & 1)
            result ^= sobol_double_matrices[i];
    }

    return result * (1.0 / (1ULL << SOBOL_DOUBLE_SIZE));
}

#endif