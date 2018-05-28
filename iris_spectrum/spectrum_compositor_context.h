/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    spectrum_compositor_context.h

Abstract:

    A spectrum compositor context mananges the lifetime of a spectrum compositor
    in order to allow memory allocated by the compositor to be reused across 
    each lifetime of the compositor.

    Once the compositor context goes out of scope, any memory allocated used
    by the compositor will be freed.

--*/

#ifndef _IRIS_SPECTRUM_SPECTRUM_COMPOSITOR_CONTEXT_
#define _IRIS_SPECTRUM_SPECTRUM_COMPOSITOR_CONTEXT_

#include "iris_spectrum/spectrum_compositor.h"

//
// Types
//

typedef struct _SPECTRUM_COMPOSITOR_CONTEXT SPECTRUM_COMPOSITOR_CONTEXT;
typedef SPECTRUM_COMPOSITOR_CONTEXT *PSPECTRUM_COMPOSITOR_CONTEXT;
typedef const SPECTRUM_COMPOSITOR_CONTEXT *PCSPECTRUM_COMPOSITOR_CONTEXT;

typedef
ISTATUS
(*PSPECTRUM_COMPOSITOR_CONTEXT_LIFETIME_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PSPECTRUM_COMPOSITOR_CONTEXT compositor_context
    );

typedef
ISTATUS
(*PSPECTRUM_COMPOSITOR_LIFETIME_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PSPECTRUM_COMPOSITOR compositor
    );

//
// Functions
//

//IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorContextCreate(
    _In_ PSPECTRUM_COMPOSITOR_CONTEXT_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context
    );


//IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorCreate(
    _Inout_ PSPECTRUM_COMPOSITOR_CONTEXT compositor_context,
    _In_ PSPECTRUM_COMPOSITOR_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context
    );
    
#endif // _IRIS_SPECTRUM_SPECTRUM_COMPOSITOR_CONTEXT_