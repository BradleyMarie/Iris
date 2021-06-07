/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    fresnel.h

Abstract:

    Defines an abstract fresnel function.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_BSDFS_FRESNELS_FRESNEL_
#define _IRIS_PHYSX_TOOLKIT_BSDFS_FRESNELS_FRESNEL_

#include "iris_physx/iris_physx.h"

//
// Types
//

struct _FRESNEL;

typedef
ISTATUS
(*PFRESNEL_COMPUTE_ROUTINE)(
    _In_ const struct _FRESNEL *function,
    _In_ float_t cosine_theta_incoming,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    );

typedef struct _FRESNEL_VTABLE {
    PFRESNEL_COMPUTE_ROUTINE compute_routine;
} FRESNEL_VTABLE, *PFRESNEL_VTABLE;

typedef const FRESNEL_VTABLE *PCFRESNEL_VTABLE;

typedef struct _FRESNEL {
    PCFRESNEL_VTABLE vtable;
    float_t eta_i;
    float_t eta_t;
} FRESNEL, *PFRESNEL;

typedef const FRESNEL *PCFRESNEL;

//
// Functions
//

static
inline
ISTATUS
FresnelCompute(
    _In_ PCFRESNEL function,
    _In_ float_t cosine_theta_incoming,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    )
{
    assert((float_t)-1.0 <= cosine_theta_incoming);
    assert(cosine_theta_incoming <= (float_t)1.0);
    ISTATUS status = function->vtable->compute_routine(function,
                                                       cosine_theta_incoming,
                                                       compositor,
                                                       reflector);
    return status;
}

#endif // _IRIS_PHYSX_TOOLKIT_BSDFS_FRESNELS_FRESNEL_