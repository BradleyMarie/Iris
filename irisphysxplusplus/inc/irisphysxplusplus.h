/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus.h

Abstract:

    This module includes all of the IrisPhysX++ public headers.

--*/

#ifndef _IRIS_PHYSX_PLUS_PLUS_HEADER_
#define _IRIS_PHYSX_PLUS_PLUS_HEADER_

//
// Include Iris
//

extern "C" {
#include <irisphysx.h>
}

//
// Include C++ standard library headers
//

#ifdef _MSC_VER
#include "optional.hpp"
#else
#include <optional>
#endif

#include <type_traits>
#include <functional>
#include <memory>
#include <tuple>

//
// Include Iris++, IrisAdvanced++, and IrisSpectrum++
//

#include <irisplusplus.h>
#include <irisadvancedplusplus.h>
#include <irisspectrumplusplus.h>

//
// Include IrisPhysx++ Headers
//

#include "irisphysxplusplus_api.h"
#include "irisphysxplusplus_brdf.h"
#include "irisphysxplusplus_brdfreference.h"
#include "irisphysxplusplus_brdfallocator.h"
#include "irisphysxplusplus_material.h"
#include "irisphysxplusplus_materialreference.h"
#include "irisphysxplusplus_hitallocator.h"
#include "irisphysxplusplus_visibilitytester.h"
#include "irisphysxplusplus_light.h"
#include "irisphysxplusplus_lightreference.h"
#include "irisphysxplusplus_geometry.h"
#include "irisphysxplusplus_geometryreference.h"
#include "irisphysxplusplus_hittester.h"
#include "irisphysxplusplus_raytracer.h"
#include "irisphysxplusplus_integrator.h"

#endif // _IRIS_PHYSX_PLUS_PLUS_HEADER_