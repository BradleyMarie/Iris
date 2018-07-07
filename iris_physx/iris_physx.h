/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    iris_physx.h

Abstract:

    The public header file for Iris Physx.

--*/

#ifndef _IRIS_PHYSX_IRIS_PHYSX_
#define _IRIS_PHYSX_IRIS_PHYSX_

#include "iris_advanced/iris_advanced.h"
#include "iris_spectrum/iris_spectrum.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

#include "iris_physx/brdf.h"
#include "iris_physx/brdf_allocator.h"
#include "iris_physx/brdf_allocator_context.h"
#include "iris_physx/hit_allocator.h"
#include "iris_physx/hit_tester.h"
#include "iris_physx/material.h"
#include "iris_physx/shape.h"

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_IRIS_PHYSX_