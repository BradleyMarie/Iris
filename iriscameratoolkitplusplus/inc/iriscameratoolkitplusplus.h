/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameratoolkitplusplus.h

Abstract:

    This module includes all of the IrisCameraToolkit++ public headers.

--*/

#ifndef _IRIS_CAMERA_TOOLKIT_PLUS_PLUS_HEADER_
#define _IRIS_CAMERA_TOOLKIT_PLUS_PLUS_HEADER_

//
// Include IrisCameraToolkit
//

extern "C" {
#include <iriscameratoolkit.h>
}

//
// Include IrisCamera++ Headers
//

#include <iriscameraplusplus.h>

//
// Include IrisCameraToolkit++ Headers
//

#include "iriscameratoolkitplusplus_mwcgenerator.h"
#include "iriscameratoolkitplusplus_pinhole.h"
#include "iriscameratoolkitplusplus_gridpixelsampler.h"

#endif // _IRIS_CAMERA_TOOLKIT_PLUS_PLUS_HEADER_
