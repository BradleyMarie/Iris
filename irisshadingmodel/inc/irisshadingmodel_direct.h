/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_direct.h

Abstract:

    This file contains the definitions for the DIRECT_MATERIAL type.

--*/

#ifndef _DIRECT_SHADER_IRIS_SHADING_MODEL_
#define _DIRECT_SHADER_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef 
ISTATUS
(*PDIRECT_SHADING_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
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

typedef struct _DIRECT_SHADER {
    PCDIRECT_SHADER_VTABLE DirectShaderVTable;
} DIRECT_SHADER, *PDIRECT_SHADER;

typedef CONST DIRECT_SHADER *PCDIRECT_SHADER;

//
// Functions
//

SFORCEINLINE
VOID
DirectShaderFree(
    _Pre_maybenull_ _Post_invalid_ PDIRECT_SHADER DirectShader
    )
{
    if (DirectShader == NULL)
    {
        return;
    }

    DirectShader->DirectShaderVTable->FreeRoutine(DirectShader);
}

#endif // _DIRECT_SHADER_IRIS_SHADING_MODEL_