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

#ifndef SOBOL_SINGLE_H
#define SOBOL_SINGLE_H

#include <assert.h>

#define SOBOL_SINGLE_DEFAULT_SCRAMBLE 0U

#define SOBOL_SINGLE_NUM_DIMENSIONS 1024
#define SOBOL_SINGLE_SIZE 52
#define SOBOL_SINGLE_MATRIX_SIZE (SOBOL_SINGLE_NUM_DIMENSIONS * SOBOL_SINGLE_SIZE)

const extern unsigned sobol_single_matrices[SOBOL_SINGLE_MATRIX_SIZE];

// Compute one component of the Sobol'-sequence, where the component
// corresponds to the dimension parameter, and the index specifies
// the point inside the sequence. The scramble parameter can be used
// to permute elementary intervals, and might be chosen randomly to
// generate a randomized QMC sequence.
static inline float sobol_single_sample(
    unsigned long long index,
    const unsigned dimension,
    const unsigned scramble)
{
    assert(dimension < SOBOL_SINGLE_NUM_DIMENSIONS);

    unsigned result = scramble;
    for (unsigned i = dimension * SOBOL_SINGLE_SIZE; index; index >>= 1, ++i)
    {
        if (index & 1)
            result ^= sobol_single_matrices[i];
    }

    return result * (1.f / (1ULL << 32));
}

#endif