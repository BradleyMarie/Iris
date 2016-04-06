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

#endif // _IRIS_PHYSX_PLUS_PLUS_HEADER_