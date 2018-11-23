/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    tone_mapper_vtable.h

Abstract:

    The vtable for a tone mapper.

--*/

#ifndef _IRIS_PHYSX_TONE_MAPPER_VTABLE_
#define _IRIS_PHYSX_TONE_MAPPER_VTABLE_

#include "iris_advanced/iris_advanced.h"
#include "iris_physx/spectrum.h"

//
// Types
//

typedef
ISTATUS
(*PTONE_MAPPER_ADD_SAMPLE_ROUTINE)(
    _In_ void *context,
    _In_opt_ PCSPECTRUM spectrum
    );

typedef
ISTATUS
(*PTONE_MAPPER_COMPUTE_TONE_ROUTINE)(
    _In_ const void *context,
    _Out_ PCOLOR3 color
    );

typedef
ISTATUS
(*PTONE_MAPPER_CLEAR_ROUTINE)(
    _Inout_ void *context
    );

typedef struct _TONE_MAPPER_VTABLE {
    PTONE_MAPPER_ADD_SAMPLE_ROUTINE add_sample_routine;
    PTONE_MAPPER_COMPUTE_TONE_ROUTINE compute_tone_routine;
    PTONE_MAPPER_CLEAR_ROUTINE clear_routine;
    PFREE_ROUTINE free_routine;
} TONE_MAPPER_VTABLE, *PTONE_MAPPER_VTABLE;

typedef const TONE_MAPPER_VTABLE *PCTONE_MAPPER_VTABLE;

#endif // _IRIS_PHYSX_TONE_MAPPER_VTABLE_