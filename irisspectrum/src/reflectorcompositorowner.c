/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    reflectorcompositorowner.c

Abstract:

    This file contains the function definitions 
    for the REFLECTOR_COMPOSITOR_OWNER type.

--*/

#include <irisspectrump.h>

//
// Types
//

struct _REFLECTOR_COMPOSITOR_OWNER {
    REFLECTOR_COMPOSITOR Compositor;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ReflectorCompositorOwnerAllocate(
    _Out_ PREFLECTOR_COMPOSITOR_OWNER *Result
    )
{
    PREFLECTOR_COMPOSITOR_OWNER CompositorOwner;
    ISTATUS Status;

    if (Result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    CompositorOwner = (PREFLECTOR_COMPOSITOR_OWNER) malloc(sizeof(REFLECTOR_COMPOSITOR_OWNER));

    if (CompositorOwner == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = ReflectorCompositorInitialize(&CompositorOwner->Compositor);

    if (Status != ISTATUS_SUCCESS)
    {
        free(CompositorOwner);
        return Status;
    }

    return ISTATUS_SUCCESS;
}

_Ret_
PREFLECTOR_COMPOSITOR
ReflectorCompositorOwnerGetCompositor(
    _In_ PREFLECTOR_COMPOSITOR_OWNER CompositorOwner
    )
{
    if (CompositorOwner == NULL)
    {
        return NULL;
    }

    return &CompositorOwner->Compositor;
}

VOID
ReflectorCompositorOwnerClear(
    _Inout_ PREFLECTOR_COMPOSITOR_OWNER CompositorOwner
    )
{
    if (CompositorOwner == NULL)
    {
        return;
    }

    ReflectorCompositorClear(&CompositorOwner->Compositor);
}

VOID
ReflectorCompositorOwnerFree(
    _In_opt_ _Post_invalid_ PREFLECTOR_COMPOSITOR_OWNER CompositorOwner
    )
{
    if (CompositorOwner == NULL)
    {
        return;
    }

    ReflectorCompositorDestroy(&CompositorOwner->Compositor);
}