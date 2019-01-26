/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    sample_direct_lighting.h

Abstract:

    Samples the brdf and light to compute an estimate the direct lighting.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_SAMPLE_DIRECT_LIGHTING_
#define _IRIS_PHYSX_TOOLKIT_SAMPLE_DIRECT_LIGHTING_

#include "iris_physx/iris_physx.h"

//
// Functions
//

ISTATUS
SampleDirectLighting(
    _In_ PCLIGHT light,
    _In_ PCBRDF brdf,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 to_hit_point,
    _In_ VECTOR3 surface_normal,
    _In_ VECTOR3 shading_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR spectrum_compositor,
    _Inout_ PREFLECTOR_COMPOSITOR reflector_compositor,
    _Out_ PCSPECTRUM *spectrum
    );

#endif // _IRIS_PHYSX_TOOLKIT_SAMPLE_DIRECT_LIGHTING_