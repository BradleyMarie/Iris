/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_indirect.h

Abstract:

    This file contains the definitions for the INDIRECT_MATERIAL type.

--*/

#ifndef _INDIRECT_SHADER_IRIS_SHADING_MODEL_
#define _INDIRECT_SHADER_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef
ISTATUS
(*PINDIRECT_SHADING_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_ PRAYTRACER RayTracer,
    _Out_ PCOLOR3 Direct
    );

typedef struct _INDIRECT_SHADER_VTABLE {
    PINDIRECT_SHADING_ROUTINE IndirectRoutine;
    PFREE_ROUTINE FreeRoutine;
} INDIRECT_SHADER_VTABLE, *PINDIRECT_SHADER_VTABLE;

typedef CONST INDIRECT_SHADER_VTABLE *PCINDIRECT_SHADER_VTABLE;

typedef struct _INDIRECT_SHADER {
    PCINDIRECT_SHADER_VTABLE IndirectShaderVTable;
} INDIRECT_SHADER, *PINDIRECT_SHADER;

typedef CONST INDIRECT_SHADER *PCINDIRECT_SHADER;

//
// Functions
//

SFORCEINLINE
VOID
IndirectShaderFree(
    _Pre_maybenull_ _Post_invalid_ PINDIRECT_SHADER IndirectShader
    )
{
    if (IndirectShader == NULL)
    {
        return;
    }

    IndirectShader->IndirectShaderVTable->FreeRoutine(IndirectShader);
}

#endif // _INDIRECT_SHADER_IRIS_SHADING_MODEL_