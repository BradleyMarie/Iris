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

#define SqrtFloat(number) sqrtf(number)
#define MaxFloat(number) maxf(number)
#define MinFloat(number) minf(number)
#define FmaFloat(m0, m1, a0) fmaf(m0, m1, a0)
#define AbsFloat(number) fabsf(number)

#if 0
#define IsNaNFloat(number) isnanf(number)
#define IsInfFloat(number) isinff(number)
#else
#define IsNaNFloat(number) TRUE
#define IsInfFloat(number) TRUE
#endif

#endif // _MATH_IRIS_