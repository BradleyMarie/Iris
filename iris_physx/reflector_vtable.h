/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    reflector_vtable.h

Abstract:

    The vtable for a reflector.

--*/

#ifndef _IRIS_PHYSX_REFLECTOR_VTABLE_
#define _IRIS_PHYSX_REFLECTOR_VTABLE_

#if __cplusplus 
#include <math.h>
#else
#include <tgmath.h>
#endif // __cplusplus

#include "common/free_routine.h"
#include "common/sal.h"
#include "common/status.h"

//
// Types
//

typedef
ISTATUS
(*PREFLECTOR_REFLECT_ROUTINE)(
    _In_ const void *context,
    _In_ float_t wavelength,
    _In_ float_t incoming_intensity,
    _Out_ float_t *outgoing_intensity
    );

typedef
ISTATUS
(*PREFLECTOR_GET_ALBEDO_ROUTINE)(
    _In_ const void *context,
    _Out_ float_t *albedo
    );

typedef struct _REFLECTOR_VTABLE {
    PREFLECTOR_REFLECT_ROUTINE reflect_routine;
    PREFLECTOR_GET_ALBEDO_ROUTINE get_albedo_routine;
    PFREE_ROUTINE free_routine;
} REFLECTOR_VTABLE, *PREFLECTOR_VTABLE;

typedef const REFLECTOR_VTABLE *PCREFLECTOR_VTABLE;

#endif // _IRIS_PHYSX_REFLECTOR_VTABLE_