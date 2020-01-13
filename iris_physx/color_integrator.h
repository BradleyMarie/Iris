/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    color_integrator.h

Abstract:

    Computes the xyz color of a spectrum or a reflector.

--*/

#ifndef _IRIS_PHYSX_COLOR_INTEGRATOR_
#define _IRIS_PHYSX_COLOR_INTEGRATOR_

#include "iris_physx/color_integrator_vtable.h"

//
// Types
//

typedef struct _COLOR_INTEGRATOR COLOR_INTEGRATOR, *PCOLOR_INTEGRATOR;
typedef const COLOR_INTEGRATOR *PCCOLOR_INTEGRATOR;

//
// Functions
//

ISTATUS
ColorIntegratorAllocate(
    _In_ PCCOLOR_INTEGRATOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PCOLOR_INTEGRATOR *color_integrator
    );

void
ColorIntegratorRetain(
    _In_opt_ PCOLOR_INTEGRATOR color_integrator
    );

void
ColorIntegratorRelease(
    _In_opt_ _Post_invalid_ PCOLOR_INTEGRATOR color_integrator
    );

#endif // _IRIS_PHYSX_COLOR_INTEGRATOR_