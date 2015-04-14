/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisrgb_direct.h

Abstract:

    This file contains the definitions for the DIRECT_MATERIAL type.

--*/

#ifndef _DIRECT_SHADER_IRIS_RGB_
#define _DIRECT_SHADER_IRIS_RGB_

#include <irisrgb.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PDIRECT_SHADING_ROUTINE)(
    _In_ PCVOID Context,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ VECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 Direct
    );

typedef struct _DIRECT_SHADER_VTABLE {
    PDIRECT_SHADING_ROUTINE DirectRoutine;
    PFREE_ROUTINE FreeRoutine;
} DIRECT_SHADER_VTABLE, *PDIRECT_SHADER_VTABLE;

typedef CONST DIRECT_SHADER_VTABLE *PCDIRECT_SHADER_VTABLE;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISRGBAPI
PDIRECT_SHADER
DirectShaderAllocate(
    _In_ PCDIRECT_SHADER_VTABLE DirectShaderVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISRGBAPI
ISTATUS
DirectShaderShade(
    _In_ PCDIRECT_SHADER DirectShader,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ VECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 Direct
    );

IRISRGBAPI
VOID
DirectShaderReference(
    _In_opt_ PDIRECT_SHADER DirectShader
    );

IRISRGBAPI
VOID
DirectShaderDereference(
    _In_opt_ _Post_invalid_ PDIRECT_SHADER DirectShader
    );

#endif // _DIRECT_SHADER_IRIS_RGB_