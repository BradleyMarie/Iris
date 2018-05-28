/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    spectrum_compositor_context.c

Abstract:

    A spectrum compositor context mananges the lifetime of a spectrum compositor
    in order to allow memory allocated by the compositor to be reused across 
    each lifetime of the compositor.

--*/

#include "iris_spectrum/spectrum_compositor_context.h"
#include "iris_spectrum/spectrum_compositor_internal.h"

//
// Types
//

struct _SPECTRUM_COMPOSITOR_CONTEXT {
    SPECTRUM_COMPOSITOR compositor;
};

//
// Functions
//

ISTATUS
SpectrumCompositorContextCreate(
    _In_ PSPECTRUM_COMPOSITOR_CONTEXT_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context
    )
{
    if (callback == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    SPECTRUM_COMPOSITOR_CONTEXT context;
    bool success = SpectrumCompositorInitialize(&context.compositor);
    if (!success)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    ISTATUS status = callback(callback_context, &context);
    SpectrumCompositorDestroy(&context.compositor);
    return status;
}

ISTATUS
SpectrumCompositorCreate(
    _Inout_ PSPECTRUM_COMPOSITOR_CONTEXT compositor_context,
    _In_ PSPECTRUM_COMPOSITOR_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context
    )
{
    if (compositor_context == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (callback == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    ISTATUS status = callback(callback_context,
                              &compositor_context->compositor);
    SpectrumCompositorClear(&compositor_context->compositor);
    return status;
}

