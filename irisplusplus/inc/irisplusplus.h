/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisplusplus.h

Abstract:

    This module includes all of the Iris++ public headers.

--*/

#ifndef _IRIS_PLUS_PLUS_HEADER_
#define _IRIS_PLUS_PLUS_HEADER_

//
// Include Iris
//

extern "C" {
#include <iris.h>
}

//
// Include C++ standard library headers
//

#include <type_traits>
#include <functional>
#include <memory>
#include <tuple>

//
// Include Iris++ Headers
//

#include "irisplusplus_api.h"
#include "irisplusplus_utility.h"
#include "irisplusplus_matrix.h"
#include "irisplusplus_matrixreference.h"
#include "irisplusplus_vector.h"
#include "irisplusplus_point.h"
#include "irisplusplus_ray.h"
#include "irisplusplus_hitallocator.h"
#include "irisplusplus_hittester.h"
#include "irisplusplus_raytracer.h"

#endif // _IRIS_PLUS_PLUS_HEADER_