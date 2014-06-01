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

#include <utility>

//
// Include Iris++ Headers
//

namespace Iris {
#include "irisplusplus_vector.h"
}

#endif // _IRIS_PLUS_PLUS_HEADER_