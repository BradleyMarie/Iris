/*++

Copyright (c) 2020 Brad Weinberger

Module Name:

    png_mipmap.h

Abstract:

    Creates a mipmap from a PNG file.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_PNG_MIPMAP_
#define _IRIS_PHYSX_TOOLKIT_PNG_MIPMAP_

#include "iris_physx_toolkit/mipmap.h"

#if __cplusplus 
extern "C" {
#endif // __cplusplus

//
// Functions
//

ISTATUS
PngReflectorMipmapAllocate(
    _In_z_ const char* filename,
    _In_ WRAP_MODE wrap_mode,
    _Inout_ PCOLOR_EXTRAPOLATOR color_extrapolator,
    _Out_ PREFLECTOR_MIPMAP *mipmap
    );

ISTATUS
PngFloatMipmapAllocate(
    _In_z_ const char* filename,
    _In_ WRAP_MODE wrap_mode,
    _Out_ PFLOAT_MIPMAP *mipmap
    );

#if __cplusplus 
}
#endif // __cplusplus

#endif // _IRIS_PHYSX_TOOLKIT_PNG_MIPMAP_