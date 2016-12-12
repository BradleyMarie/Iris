/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxtoolkitplusplus.h

Abstract:

    This module includes all of the IrisPhysXToolkit++ public headers.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_PLUS_PLUS_HEADER_
#define _IRIS_PHYSX_TOOLKIT_PLUS_PLUS_HEADER_

//
// Include Iris
//

extern "C" {
#include <irisphysxtoolkit.h>
}

//
// Include C++ standard library headers
//

#include <functional>
#include <stdexcept>
#include <vector>
#include <memory>
#include <tuple>

//
// Include Iris++, IrisAdvanced++, IrisCamera++,
// IrisSpectrum++, IrisPhysx++, IrisAdvancedToolkit++
//

#include <irisplusplus.h>
#include <irisadvancedplusplus.h>
#include <iriscameraplusplus.h>
#include <irisadvancedtoolkitplusplus.h>
#include <irisspectrumplusplus.h>
#include <irisphysxplusplus.h>

//
// Include IrisPhysx++ Headers
//

#include "irisphysxtoolkitplusplus_api.h"
#include "irisphysxtoolkitplusplus_typedefs.h"
#include "irisphysxtoolkitplusplus_pinhole.h"
#include "irisphysxtoolkitplusplus_sphere.h"
#include "irisphysxtoolkitplusplus_triangle.h"
#include "irisphysxtoolkitplusplus_infiniteplane.h"
#include "irisphysxtoolkitplusplus_constantarealight.h"
#include "irisphysxtoolkitplusplus_lambertianmaterial.h"

#endif // _IRIS_PHYSX_TOOLKIT_PLUS_PLUS_HEADER_