/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    mipmap.h

Abstract:

    Creates a mipmap.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_MIPMAP_
#define _IRIS_PHYSX_TOOLKIT_MIPMAP_

#include "iris_advanced_toolkit/color_io.h"
#include "iris_physx_toolkit/color_extrapolator.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Enums
//

typedef enum _TEXTURE_FILTERING_ALGORITHM {
    TEXTURE_FILTERING_ALGORITHM_NONE = 0,
    TEXTURE_FILTERING_ALGORITHM_TRILINEAR = 1,
    TEXTURE_FILTERING_ALGORITHM_EWA = 2
} TEXTURE_FILTERING_ALGORITHM;

typedef enum _WRAP_MODE {
    WRAP_MODE_REPEAT = 0,
    WRAP_MODE_BLACK = 1,
    WRAP_MODE_CLAMP = 2
} WRAP_MODE;

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
    _In_reads_(height * width) const COLOR3 texels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ float_t max_anisotropy,
    _In_ WRAP_MODE wrap_mode,
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _Out_ PSPECTRUM_MIPMAP *mipmap
    );

ISTATUS
SpectrumMipmapLookup(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    );

ISTATUS
SpectrumMipmapFilteredLookup(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _In_ PSPECTRUM_COMPOSITOR compositor,
    _Out_ PCSPECTRUM *spectrum
    );

ISTATUS
SpectrumMipmapGetDimensions(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _Out_ size_t* levels,
    _Out_ size_t* width,
    _Out_ size_t* height
    );

ISTATUS
SpectrumMipmapTexelLookup(
    _In_ PCSPECTRUM_MIPMAP mipmap,
    _In_ size_t levels,
    _In_ size_t width,
    _In_ size_t height,
    _Out_ PCSPECTRUM* spectrum
    );

void
SpectrumMipmapFree(
    _In_opt_ _Post_invalid_ PSPECTRUM_MIPMAP mipmap
    );

//
// Types
//

typedef struct _REFLECTOR_MIPMAP REFLECTOR_MIPMAP, *PREFLECTOR_MIPMAP;
typedef const REFLECTOR_MIPMAP *PCREFLECTOR_MIPMAP;

//
// Functions
//

ISTATUS
ReflectorMipmapAllocate(
    _In_reads_(height * width) const COLOR3 texels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ float_t max_anisotropy,
    _In_ WRAP_MODE wrap_mode,
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _Out_ PREFLECTOR_MIPMAP *mipmap
    );

ISTATUS
ReflectorMipmapLookup(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    );

ISTATUS
ReflectorMipmapFilteredLookup(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _In_ PREFLECTOR_COMPOSITOR compositor,
    _Out_ PCREFLECTOR *reflector
    );

ISTATUS
ReflectorMipmapGetDimensions(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _Out_ size_t* levels,
    _Out_ size_t* width,
    _Out_ size_t* height
    );

ISTATUS
ReflectorMipmapTexelLookup(
    _In_ PCREFLECTOR_MIPMAP mipmap,
    _In_ size_t levels,
    _In_ size_t width,
    _In_ size_t height,
    _Out_ PCREFLECTOR* reflector
    );

void
ReflectorMipmapFree(
    _In_opt_ _Post_invalid_ PREFLECTOR_MIPMAP mipmap
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
FloatMipmapAllocateFromFloats(
    _In_reads_(height * width) const float_t texels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ float_t max_anisotropy,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_MIPMAP *mipmap
    );

ISTATUS
FloatMipmapAllocateFromLuma(
    _In_reads_(height * width) const COLOR3 texels[],
    _In_ size_t width,
    _In_ size_t height,
    _In_ TEXTURE_FILTERING_ALGORITHM texture_filtering,
    _In_ float_t max_anisotropy,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_MIPMAP *mipmap
    );

ISTATUS
FloatMipmapLookup(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _Out_ float_t *value
    );

ISTATUS
FloatMipmapFilteredLookup(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ float_t s,
    _In_ float_t t,
    _In_ float_t dsdx,
    _In_ float_t dsdy,
    _In_ float_t dtdx,
    _In_ float_t dtdy,
    _Out_ float_t *value
    );

ISTATUS
FloatMipmapGetDimensions(
    _In_ PCFLOAT_MIPMAP mipmap,
    _Out_ size_t* levels,
    _Out_ size_t* width,
    _Out_ size_t* height
    );

ISTATUS
FloatMipmapTexelLookup(
    _In_ PCFLOAT_MIPMAP mipmap,
    _In_ size_t levels,
    _In_ size_t width,
    _In_ size_t height,
    _Out_ float_t* value
    );

void
FloatMipmapFree(
    _In_opt_ _Post_invalid_ PFLOAT_MIPMAP mipmap
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_REFLECTOR_MIPMAP_