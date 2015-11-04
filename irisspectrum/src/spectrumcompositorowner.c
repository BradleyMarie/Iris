/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    spectrumcompositorowner.c

Abstract:

    This file contains the function definitions 
    for the SPECTRUM_COMPOSITOR_OWNER type.

--*/

#include <irisspectrump.h>

//
// Types
//

struct _SPECTRUM_COMPOSITOR_OWNER {
    SPECTRUM_COMPOSITOR Compositor;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumCompositorOwnerAllocate(
    _Out_ PSPECTRUM_COMPOSITOR_OWNER *Result
    )
{
    PSPECTRUM_COMPOSITOR_OWNER CompositorOwner;
    ISTATUS Status;

    if (Result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    CompositorOwner = (PSPECTRUM_COMPOSITOR_OWNER) malloc(sizeof(SPECTRUM_COMPOSITOR_OWNER));

    if (CompositorOwner == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = SpectrumCompositorInitialize(&CompositorOwner->Compositor);

    if (Status != ISTATUS_SUCCESS)
    {
        free(CompositorOwner);
        return Status;
    }

    return ISTATUS_SUCCESS;
}

_Ret_
PSPECTRUM_COMPOSITOR
SpectrumCompositorOwnerGetCompositor(
    _In_ PSPECTRUM_COMPOSITOR_OWNER CompositorOwner
    )
{
    if (CompositorOwner == NULL)
    {
        return NULL;
    }

    return &CompositorOwner->Compositor;
}

VOID
SpectrumCompositorOwnerClear(
    _Inout_ PSPECTRUM_COMPOSITOR_OWNER CompositorOwner
    )
{
    if (CompositorOwner == NULL)
    {
        return;
    }

    SpectrumCompositorClear(&CompositorOwner->Compositor);
}

VOID
SpectrumCompositorOwnerFree(
    _In_opt_ _Post_invalid_ PSPECTRUM_COMPOSITOR_OWNER CompositorOwner
    )
{
    if (CompositorOwner == NULL)
    {
        return;
    }

    SpectrumCompositorDestroy(&CompositorOwner->Compositor);
}