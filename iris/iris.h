/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    iris.h

Abstract:

    The public header file for Iris.

--*/

#ifndef _IRIS_IRIS_
#define _IRIS_IRIS_

#if __cplusplus 
extern "C" {
#endif // __cplusplus

#include "iris/hit.h"
#include "iris/hit_allocator.h"
#include "iris/hit_context.h"
#include "iris/hit_tester.h"
#include "iris/matrix.h"
#include "iris/multiply.h"
#include "iris/point.h"
#include "iris/ray.h"
#include "iris/ray_tracer.h"
#include "iris/vector.h"

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_IRIS_