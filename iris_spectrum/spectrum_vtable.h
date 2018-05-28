/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    spectrum_type_vtable.h

Abstract:

    The vtable for a spectrum.

--*/

#ifndef _IRIS_SPECTRUM_SPECTRUM_VTABLE_
#define _IRIS_SPECTRUM_SPECTRUM_VTABLE_

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
(*PSPECTRUM_SAMPLE_ROUTINE)(
    _In_ const void *context,
    _In_ float_t wavelength,
    _Out_ float_t *intensity
    );

typedef struct _SPECTRUM_VTABLE {
    PSPECTRUM_SAMPLE_ROUTINE sample_routine;
    PFREE_ROUTINE free_routine;
} SPECTRUM_VTABLE, *PSPECTRUM_VTABLE;

typedef const SPECTRUM_VTABLE *PCSPECTRUM_VTABLE;

#endif // _IRIS_SPECTRUM_SPECTRUM_VTABLE_