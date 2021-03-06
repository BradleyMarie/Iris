/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    reflector.h

Abstract:

    Interface sampling from a reflector.

--*/

#ifndef _IRIS_PHYSX_REFLECTOR_
#define _IRIS_PHYSX_REFLECTOR_

#include <stddef.h>

#include "iris_physx/reflector_vtable.h"

//
// Types
//

typedef struct _REFLECTOR REFLECTOR, *PREFLECTOR;
typedef const REFLECTOR *PCREFLECTOR;

//
// Data
//

extern const REFLECTOR *const iris_physx_perfect_reflector;

//
// Functions
//

ISTATUS
ReflectorAllocate(
    _In_ PCREFLECTOR_VTABLE vtable,
    _In_reads_bytes_opt_(data_size) const void *data,
    _In_ size_t data_size,
    _In_ size_t data_alignment,
    _Out_ PREFLECTOR *reflector
    );

ISTATUS
ReflectorReflect(
    _In_opt_ PCREFLECTOR reflector,
    _In_ float_t wavelength,
    _Out_ float_t *reflectance
    );

ISTATUS
ReflectorGetAlbedo(
    _In_opt_ PCREFLECTOR reflector,
    _Out_ float_t *albedo
    );

void
ReflectorRetain(
    _In_opt_ PREFLECTOR reflector
    );

void
ReflectorRelease(
    _In_opt_ _Post_invalid_ PREFLECTOR reflector
    );

#endif // _IRIS_PHYSX_REFLECTOR_