/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisp.h

Abstract:

    This module includes all of the Iris internal headers.

--*/

#ifndef _IRIS_INTERNAL_HEADER_
#define _IRIS_INTERNAL_HEADER_

#ifndef _IRIS_EXPORT_RAY_ROUTINES_
#define _DISABLE_IRIS_RAY_EXPORTS_
#endif

#ifndef _IRIS_EXPORT_VECTOR_ROUTINES_
#define _DISABLE_IRIS_VECTOR_EXPORTS_
#endif

#ifndef _IRIS_EXPORT_POINT_ROUTINES_
#define _DISABLE_IRIS_POINT_EXPORTS_
#endif

#ifndef _IRIS_EXPORT_SHAPE_REFERENCE_ROUTINES_
#define _DISABLE_IRIS_SHAPE_REFERENCE_EXPORTS_
#endif

#include <iris.h>
#include <iriscommon_malloc.h>
#include <iriscommon_pointerlist.h>
#include <iriscommon_constpointerlist.h>
#include <iriscommon_dynamicallocator.h>
#include <iriscommon_staticallocator.h>

#include "iris_math.h"
#include "iris_matrixreference.h"
#include "iris_matrix.h"
#include "iris_vector.h"
#include "iris_point.h"
#include "iris_ray.h"
#include "iris_hitallocator.h"
#include "iris_shapereference.h"
#include "iris_shape.h"
#include "iris_sharedhitdataallocator.h"
#include "iris_scene.h"

#endif // _IRIS_INTERNAL_HEADER_