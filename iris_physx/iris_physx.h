/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    iris_physx.h

Abstract:

    The public header file for Iris Physx.

--*/

#ifndef _IRIS_PHYSX_IRIS_PHYSX_
#define _IRIS_PHYSX_IRIS_PHYSX_

#include "iris_advanced/iris_advanced.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

#include "iris_physx/area_light.h"
#include "iris_physx/bsdf.h"
#include "iris_physx/bsdf_allocator.h"
#include "iris_physx/color_integrator.h"
#include "iris_physx/emissive_material.h"
#include "iris_physx/hit_allocator.h"
#include "iris_physx/hit_tester.h"
#include "iris_physx/integrator.h"
#include "iris_physx/light.h"
#include "iris_physx/light_sample_collector.h"
#include "iris_physx/light_sample_list.h"
#include "iris_physx/light_sampler.h"
#include "iris_physx/material.h"
#include "iris_physx/ray_tracer.h"
#include "iris_physx/reflector.h"
#include "iris_physx/reflector_compositor.h"
#include "iris_physx/scene.h"
#include "iris_physx/shape.h"
#include "iris_physx/spectrum.h"
#include "iris_physx/spectrum_compositor.h"
#include "iris_physx/visibility_tester.h"

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_IRIS_PHYSX_