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
// Include IrisToolkit
//

extern "C" {
#include <iristoolkit.h>
}

//
// Include C++ standard library headers
//

#include <stdexcept>
#include <utility>
#include <memory>
#include <atomic>
#include <new>

//
// Include Iris++ Headers
//

#include "irisplusplus_api.h"
#include "irisplusplus_matrix.h"
#include "irisplusplus_vector.h"
#include "irisplusplus_point.h"
#include "irisplusplus_ray.h"
#include "irisplusplus_irispointer.h"
#include "irisplusplus_shapehitallocator.h"
#include "irisplusplus_normal.h"
#include "irisplusplus_texture.h"
#include "irisplusplus_shape.h"
#include "irisplusplus_translucentshader.h"
#include "irisplusplus_emissiveshader.h"
#include "irisplusplus_rng.h"
#include "irisplusplus_visibilitytester.h"

#endif // _IRIS_PLUS_PLUS_HEADER_