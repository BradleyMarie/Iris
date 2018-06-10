/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    reflector_allocator_context.h

Abstract:

    A reflector allocator context mananges the lifetime of a reflector allocator
    in order to allow memory allocated by the allocator to be reused across 
    each lifetime of the allocator.

    Once the allocator context goes out of scope, any memory allocated used
    by the allocator will be freed.

--*/

#ifndef _IRIS_SPECTRUM_REFLECTOR_ALLOCATOR_CONTEXT_
#define _IRIS_SPECTRUM_REFLECTOR_ALLOCATOR_CONTEXT_

#include "iris_spectrum/reflector_allocator.h"

//
// Types
//

typedef struct _REFLECTOR_ALLOCATOR_CONTEXT REFLECTOR_ALLOCATOR_CONTEXT;
typedef REFLECTOR_ALLOCATOR_CONTEXT *PREFLECTOR_ALLOCATOR_CONTEXT;
typedef const REFLECTOR_ALLOCATOR_CONTEXT *PCREFLECTOR_ALLOCATOR_CONTEXT;

typedef
ISTATUS
(*PREFLECTOR_ALLOCATOR_CONTEXT_LIFETIME_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PREFLECTOR_ALLOCATOR_CONTEXT allocator_context
    );

typedef
ISTATUS
(*PREFLECTOR_ALLOCATOR_LIFETIME_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PREFLECTOR_ALLOCATOR allocator
    );

//
// Functions
//

//IRISSPECTRUMAPI
ISTATUS
ReflectorAllocatorContextCreate(
    _In_ PREFLECTOR_ALLOCATOR_CONTEXT_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context
    );

//IRISSPECTRUMAPI
ISTATUS
ReflectorAllocatorCreate(
    _Inout_ PREFLECTOR_ALLOCATOR_CONTEXT compositor_context,
    _In_ PREFLECTOR_ALLOCATOR_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context
    );
    
#endif // _IRIS_SPECTRUM_REFLECTOR_ALLOCATOR_CONTEXT_