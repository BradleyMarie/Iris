/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_shader.h

Abstract:

    This file contains the definitions for the SHADER type.

--*/

#ifndef _SHADER_IRIS_SHADING_MODEL_
#define _SHADER_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef
VOID
(*PTRANSLUCENT_SHADING_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PFLOAT Alpha
    );

typedef
VOID
(*PEMISSIVE_SHADING_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PCOLOR3 Emissive
    );

typedef struct _SHADER {
    PEMISSIVE_SHADING_ROUTINE EmissiveRoutine;
    PTRANSLUCENT_SHADING_ROUTINE TranslucentRoutine;
} SHADER, *PSHADER;

typedef CONST SHADER *PCSHADER;

#endif // _SHADER_IRIS_SHADING_MODEL_