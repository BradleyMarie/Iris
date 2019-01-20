/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    light.h

Abstract:

    The interface for an object representing a light.

--*/

#ifndef _IRIS_LIGHT_
#define _IRIS_LIGHT_

#include "iris_physx/light_vtable.h"

//
// Types
//

typedef struct _LIGHT LIGHT, *PLIGHT;
typedef const LIGHT *PCLIGHT;

//
// Functions
//

ISTATUS
LightAllocate(
    _In_ PCLIGHT_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PLIGHT *light
    );

ISTATUS
LightSample(
    _In_ PCLIGHT light,
    _In_ POINT3 hit_point,
    _In_ VECTOR3 surface_normal,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PRANDOM rng,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ PVECTOR3 to_light,
    _Out_ float_t *pdf
    );

ISTATUS
LightComputeEmissive(
    _In_ PCLIGHT light,
    _In_ RAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    );

ISTATUS
LightComputeEmissiveWithPdf(
    _In_ PCLIGHT light,
    _In_ RAY to_light,
    _Inout_ PVISIBILITY_TESTER visibility_tester,
    _Inout_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum,
    _Out_ float_t *pdf
    );

void
LightRetain(
    _In_opt_ PLIGHT light
    );

void
LightRelease(
    _In_opt_ _Post_invalid_ PLIGHT light
    );

#endif // _IRIS_LIGHT_