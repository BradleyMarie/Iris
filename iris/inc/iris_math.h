/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_math.h

Abstract:

    This module implements the Iris math routines.

--*/

#include <iris.h>
#include <math.h>

#ifndef _MATH_IRIS_
#define _MATH_IRIS_

#define PI_FLOAT (FLOAT)3.14159265358979323846264338327950288419716939
#define INV_PI_FLOAT (FLOAT)0.3183098861837906715377675267450287240689

#define SqrtFloat(number) sqrtf(number)
#define MaxFloat(number) maxf(number)
#define MinFloat(number) minf(number)
#define FmaFloat(m0, m1, a0) fmaf(m0, m1, a0)
#define AbsFloat(number) fabsf(number)
#define CosFloat(number) cosf(number)
#define SinFloat(number) sinf(number)
#define IsNaNFloat(number) isnan(number)
#define IsInfFloat(number) isinf(number)
#define IsNormalFloat(number) isnormal(number)
#define IsFiniteFloat(number) isfinite(number)

#endif // _MATH_IRIS_