/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    reflector_allocator_context.c

Abstract:

    A reflector allocator context mananges the lifetime of a reflector allocator
    in order to allow memory allocated by the allocator to be reused across 
    each lifetime of the allocator.

--*/

#include "iris_spectrum/reflector_allocator_context.h"
#include "iris_spectrum/reflector_allocator_internal.h"

//
// Types
//

struct _REFLECTOR_ALLOCATOR_CONTEXT {
    REFLECTOR_ALLOCATOR allocator;
};

//
// Functions
//

ISTATUS
ReflectorAllocatorContextCreate(
    _In_ PREFLECTOR_ALLOCATOR_CONTEXT_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context
    )
{
    if (callback == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    REFLECTOR_ALLOCATOR_CONTEXT context;
    ReflectorAllocatorInitialize(&context.allocator);

    ISTATUS status = callback(callback_context, &context);
    ReflectorAllocatorDestroy(&context.allocator);
    
    return status;
}

ISTATUS
ReflectorAllocatorCreate(
    _Inout_ PREFLECTOR_ALLOCATOR_CONTEXT allocator_context,
    _In_ PREFLECTOR_ALLOCATOR_LIFETIME_ROUTINE callback,
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
    ReflectorAllocatorClear(&allocator_context->allocator);

    return status;
}