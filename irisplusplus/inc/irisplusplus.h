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

#include <functional>
#include <stdexcept>
#include <utility>
#include <memory>
#include <atomic>
#include <tuple>
#include <new>

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
#include "irisplusplus_irispointer.h"
#include "irisplusplus_shapereference.h"
#include "irisplusplus_shape.h"
#include "irisplusplus_shapebase.h"
#include "irisplusplus_raytracerreference.h"
#include "irisplusplus_scene.h"
#include "irisplusplus_raytracer.h"

#endif // _IRIS_PLUS_PLUS_HEADER_