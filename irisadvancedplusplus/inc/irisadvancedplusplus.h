/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisplusplus_advanced.h

Abstract:

    This module includes all of the IrisAdvanced++ public headers.

--*/

#ifndef _IRIS_ADVANCED_PLUS_PLUS_HEADER_
#define _IRIS_ADVANCED_PLUS_PLUS_HEADER_

//
// Include Iris Advanced
//

extern "C" {
#include <irisadvanced.h>
}

//
// Include C++ standard library headers
//

#include <stdexcept>
#include <memory>

//
// Include Iris++ Headers
//

#include <irisplusplus.h>

//
// Include IrisAdvanced++ Headers
//

#include "irisadvancedplusplus_api.h"
#include "irisadvancedplusplus_color3.h"
#include "irisadvancedplusplus_random.h"
#include "irisadvancedplusplus_randomallocator.h"
#include "irisadvancedplusplus_randomgenerator.h"
#include "irisadvancedplusplus_randombase.h"

#endif // _IRIS_ADVANCED_PLUS_PLUS_HEADER_
