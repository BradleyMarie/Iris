/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisrgb_translucent.h

Abstract:

    This file contains the definitions for the TRANSLUCENT_SHADER type.

--*/

#ifndef _TRANSLUCENT_SHADER_IRIS_RGB_
#define _TRANSLUCENT_SHADER_IRIS_RGB_

#include <irisrgb.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PTRANSLUCENT_SHADING_ROUTINE)(
    _In_ PCVOID Context,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PFLOAT Alpha
    );

typedef struct _TRANSLUCENT_SHADER_VTABLE {
    PTRANSLUCENT_SHADING_ROUTINE TranslucentRoutine;
    PFREE_ROUTINE FreeRoutine;
} TRANSLUCENT_SHADER_VTABLE, *PTRANSLUCENT_SHADER_VTABLE;

typedef CONST TRANSLUCENT_SHADER_VTABLE *PCTRANSLUCENT_SHADER_VTABLE;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISRGBAPI
PTRANSLUCENT_SHADER
TranslucentShaderAllocate(
    _In_ PCTRANSLUCENT_SHADER_VTABLE TranslucentShaderVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISRGBAPI
ISTATUS
TranslucentShaderShade(
    _In_ PCTRANSLUCENT_SHADER TranslucentShader,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PFLOAT Alpha
    );

IRISRGBAPI
VOID
TranslucentShaderReference(
    _In_opt_ PTRANSLUCENT_SHADER TranslucentShader
    );

IRISRGBAPI
VOID
TranslucentShaderDereference(
    _In_opt_ _Post_invalid_ PTRANSLUCENT_SHADER TranslucentShader
    );

#endif // _TRANSLUCENT_SHADER_IRIS_RGB_