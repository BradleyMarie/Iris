/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

   brdf_allocator_context.c

Abstract:

    A BRDF allocator context mananges the lifetime of a BRDF allocator in order
    to allow memory allocated by the allocator to be reused across each lifetime
    of the allocator.

--*/

#include "iris_physx/brdf_allocator_context.h"
#include "iris_physx/brdf_allocator_internal.h"

//
// Types
//

struct _BRDF_ALLOCATOR_CONTEXT {
    BRDF_ALLOCATOR allocator;
};

//
// Functions
//

ISTATUS
BrdfAllocatorContextCreate(
    _In_ PBRDF_ALLOCATOR_CONTEXT_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context
    )
{
    if (callback == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    BRDF_ALLOCATOR_CONTEXT context;
    BrdfAllocatorInitialize(&context.allocator);

    ISTATUS status = callback(callback_context, &context);
    BrdfAllocatorDestroy(&context.allocator);
    
    return status;
}

ISTATUS
BrdfAllocatorCreate(
    _Inout_ PBRDF_ALLOCATOR_CONTEXT allocator_context,
    _In_ PBRDF_ALLOCATOR_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context
    )
{
    if (allocator_context == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (callback == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    ISTATUS status = callback(callback_context, &allocator_context->allocator);
    BrdfAllocatorClear(&allocator_context->allocator);

    return status;
}