/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    environmental_light.h

Abstract:

    An environmental light represents the non-geometric light contributing to a
    scene.

--*/

#ifndef _IRIS_PHYSX_ENVIRONMENTAL_LIGHT_
#define _IRIS_PHYSX_ENVIRONMENTAL_LIGHT_

#include "iris_physx/environmental_light_vtable.h"
#include "iris_physx/light.h"

//
// Types
//

typedef struct _ENVIRONMENTAL_LIGHT ENVIRONMENTAL_LIGHT, *PENVIRONMENTAL_LIGHT;
typedef const ENVIRONMENTAL_LIGHT *PCENVIRONMENTAL_LIGHT;

//
// Functions
//

ISTATUS
EnvironmentalLightAllocate(
    _In_ PCENVIRONMENTAL_LIGHT_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PENVIRONMENTAL_LIGHT *environmental_light,
    _Out_ PLIGHT *light
    );

ISTATUS
EnvironmentalLightSample(
    _In_ PCENVIRONMENTAL_LIGHT light,
    _In_ VECTOR3 surface_normal,
    _Inout_ PRANDOM rng,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ PVECTOR3 to_light,
    _Out_ float_t *pdf
    );

ISTATUS
EnvironmentalLightComputeEmissive(
    _In_ PCENVIRONMENTAL_LIGHT light,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    );

ISTATUS
EnvironmentalLightComputeEmissiveWithPdf(
    _In_ PCENVIRONMENTAL_LIGHT light,
    _In_ VECTOR3 to_light,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    );

void
EnvironmentalLightRetain(
    _In_opt_ PENVIRONMENTAL_LIGHT environmental_light
    );

void
EnvironmentalLightRelease(
    _In_opt_ _Post_invalid_ PENVIRONMENTAL_LIGHT environmental_light
    );

#endif // _IRIS_PHYSX_ENVIRONMENTAL_LIGHT_
