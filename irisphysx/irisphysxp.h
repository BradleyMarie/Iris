/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisphysxp.h

Abstract:

    This module includes all of the Iris PhysX internal headers.

--*/

#ifndef _IRIS_PHYSX_INTERNAL_HEADER_
#define _IRIS_PHYSX_INTERNAL_HEADER_

#include <irisphysx.h>
#include <iriscommon_staticallocator.h>
#include <iriscommon_dynamicallocator.h>
#include <iriscommon_pointerlist.h>
#include <iriscommon_visibility.h>
#include <iriscommon_uint32toindexmap.h>

#include "irisphysx_types.h"
#include "irisphysx_brdf.h"
#include "irisphysx_brdfallocator.h"
#include "irisphysx_hitallocator.h"
#include "irisphysx_arealightbuilder.h"
#include "irisphysx_light.h"
#include "irisphysx_arealight.h"
#include "irisphysx_geometry.h"
#include "irisphysx_hittester.h"
#include "irisphysx_raytraceradapter.h"
#include "irisphysx_visibilitytester.h"
#include "irisphysx_sharedcontext.h"
#include "irisphysx_raytracer.h"

#endif // _IRIS_PHYSX_INTERNAL_HEADER_