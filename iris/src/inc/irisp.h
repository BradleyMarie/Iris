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

#include <iris.h>

#include "iris_types.h"
#include "iris_matrix.h"
#include "iris_vector.h"
#include "iris_point.h"
#include "iris_ray.h"
#include "iris_pointerlist.h"
#include "iris_constpointerlist.h"
#include "iris_dynamicallocator.h"
#include "iris_staticallocator.h"
#include "iris_geometryhit.h"
#include "iris_shapehitallocator.h"
#include "iris_sharedgeometryhitallocator.h"
#include "iris_geometryhitallocator.h"
#include "iris_geometry.h"
#include "iris_raytracer.h"

#endif // _IRIS_INTERNAL_HEADER_