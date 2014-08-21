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
#include "irisplusplus_shape.h"

#endif // _IRIS_PLUS_PLUS_HEADER_