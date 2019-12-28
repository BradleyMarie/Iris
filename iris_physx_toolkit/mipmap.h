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

typedef struct _SPECTRUM_MIPMAP SPECTRUM_MIPMAP, *PSPECTRUM_MIPMAP;
typedef const SPECTRUM_MIPMAP *PCSPECTRUM_MIPMAP;

//
// Functions
//

ISTATUS
SpectrumMipmapAllocate(
    _In_reads_(height * width) float_t textels[][3],
    _In_ size_t width,
    _In_ size_t height,
    _In_ PCRGB_INTERPOLATOR rgb_interpolator,
    _Inout_opt_ PCOLOR_INTEGRATOR color_integrator,
    _Out_ PSPECTRUM_MIPMAP *mipmap
    );

ISTATUS
SpectrumMipmapLookup(
    _In_ PCSPECTRUM_MIPMAP mipmap,
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
SpectrumMipmapFree(
    _In_opt_ _Post_invalid_ PSPECTRUM_MIPMAP mipmap
    );

//
// Types
//

typedef struct _FLOAT_MIPMAP FLOAT_MIPMAP, *PFLOAT_MIPMAP;
typedef const FLOAT_MIPMAP *PCFLOAT_MIPMAP;

//
// Functions
//

ISTATUS
FloatMipmapAllocate(
    _In_reads_(height * width) float_t textels[],
    _In_ size_t width,
    _In_ size_t height,
    _Out_ PFLOAT_MIPMAP *mipmap
    );

ISTATUS
FloatMipmapLookup(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _Out_ float_t *value
    );

void
FloatMipmapFree(
    _In_opt_ _Post_invalid_ PFLOAT_MIPMAP mipmap
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_SPECTRUM_MIPMAP_