/*++

Copyright (c) 2018 Brad Weinberger

Module Name:

    brdf_allocator_context.h

Abstract:

    A BRDF allocator context mananges the lifetime of a BRDF allocator in order
    to allow memory allocated by the allocator to be reused across each lifetime
    of the allocator.

    Once the allocator context goes out of scope, any memory allocated used
    by the allocator will be freed.

--*/

#ifndef _IRIS_PHYSX_BRDF_ALLOCATOR_CONTEXT_
#define _IRIS_PHYSX_BRDF_ALLOCATOR_CONTEXT_

#include "iris_physx/brdf_allocator.h"

//
// Types
//

typedef struct _BRDF_ALLOCATOR_CONTEXT BRDF_ALLOCATOR_CONTEXT;
typedef BRDF_ALLOCATOR_CONTEXT *PBRDF_ALLOCATOR_CONTEXT;
typedef const BRDF_ALLOCATOR_CONTEXT *PCBRDF_ALLOCATOR_CONTEXT;

typedef
ISTATUS
(*PBRDF_ALLOCATOR_CONTEXT_LIFETIME_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PBRDF_ALLOCATOR_CONTEXT allocator_context
    );

typedef
ISTATUS
(*PBRDF_ALLOCATOR_LIFETIME_ROUTINE)(
    _Inout_opt_ void *context,
    _In_ PBRDF_ALLOCATOR allocator
    );

//
// Functions
//

ISTATUS
BrdfAllocatorContextCreate(
    _In_ PBRDF_ALLOCATOR_CONTEXT_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context
    );

ISTATUS
BrdfAllocatorCreate(
    _Inout_ PBRDF_ALLOCATOR_CONTEXT compositor_context,
    _In_ PBRDF_ALLOCATOR_LIFETIME_ROUTINE callback,
    _Inout_opt_ void *callback_context
    );
    
#endif // _IRIS_PHYSX_BRDF_ALLOCATOR_CONTEXT_