/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameraplusplus.h

Abstract:

    This module includes all of the IrisCamera++ public headers.

--*/

#ifndef _IRIS_CAMERA_PLUS_PLUS_HEADER_
#define _IRIS_CAMERA_PLUS_PLUS_HEADER_

//
// Include IrisCamera
//

extern "C" {
#include <iriscamera.h>
}

//
// Include C++ standard library headers
//

#include <stdexcept>
#include <memory>
#include <string>

//
// Include IrisAdvanced++ Headers
//

#include <irisadvancedplusplus.h>

//
// Include IrisCamera++ Headers
//

#include "iriscameraplusplus_api.h"
#include "iriscameraplusplus_framebuffer.h"
#include "iriscameraplusplus_randomgenerator.h"
#include "iriscameraplusplus_camera.h"
#include "iriscameraplusplus_pixelsampler.h"
#include "iriscameraplusplus_sampletracer.h"
#include "iriscameraplusplus_sampletracergenerator.h"
#include "iriscameraplusplus_render.h"

#endif // _IRIS_CAMERA_PLUS_PLUS_HEADER_
