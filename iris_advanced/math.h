/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    math.h

Abstract:

    Set of  mathematical values and functions commonly needed in rendering
    computations.

--*/

#ifndef _IRIS_ADVANCED_MATH_
#define _IRIS_ADVANCED_MATH_

#include "iris/iris.h"

//
// Data
//

extern const float_t iris_inv_two_pi;
extern const float_t iris_inv_pi;
extern const float_t iris_pi;
extern const float_t iris_two_pi;

//
// Functions
//

static
inline
void
SinCos(
    _In_ float_t theta,
    _Out_ float_t *s,
    _Out_ float_t *c
    )
{
    assert(s != NULL);
    assert(c != NULL);

#if FLT_EVAL_METHOD	== 0
    double expanded_theta = (double)theta;
#elif FLT_EVAL_METHOD == 1
    long double expanded_theta = (long double)theta;
#elif FLT_EVAL_METHOD == 2
    long double expanded_theta = (long double)theta;
#endif

    *s = sin(expanded_theta);
    *c = cos(expanded_theta);
}

#endif // _IRIS_ADVANCED_MATH_