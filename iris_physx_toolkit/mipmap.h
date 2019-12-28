/*++

Copyright (c) 2019 Brad Weinberger

Module Name:

    mipmap.h

Abstract:

    Creates a mipmap.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_MIPMAP_
#define _IRIS_PHYSX_TOOLKIT_MIPMAP_

#include "iris_physx_toolkit/rgb_interpolator.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Types
//

typedef struct _MIPMAP MIPMAP, *PMIPMAP;
typedef const MIPMAP *PCMIPMAP;

//
// Functions
//

ISTATUS
MipmapAllocate(
    _In_reads_(height * width) float_t textels[][3],
    _In_ size_t width,
    _In_ size_t height,
    _In_ PCRGB_INTERPOLATOR rgb_interpolator,
    _Out_ PMIPMAP *mipmap
    );

ISTATUS
MipmapLookup(
    _In_ PCMIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    );

void
MipmapFree(
    _In_opt_ _Post_invalid_ PMIPMAP mipmap
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_MIPMAP_