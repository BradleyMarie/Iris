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
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PINDIRECT_SHADING_ROUTINE)(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _In_ PCVECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_opt_ PRAYSHADER RayTracer,
    _Out_ PCOLOR3 Indirect
    );

typedef struct _INDIRECT_SHADER_VTABLE {
    PINDIRECT_SHADING_ROUTINE IndirectRoutine;
    PFREE_ROUTINE FreeRoutine;
} INDIRECT_SHADER_VTABLE, *PINDIRECT_SHADER_VTABLE;

typedef CONST INDIRECT_SHADER_VTABLE *PCINDIRECT_SHADER_VTABLE;

struct _INDIRECT_SHADER {
    PCINDIRECT_SHADER_VTABLE IndirectShaderVTable;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
IndirectShaderShade(
    _In_ PCINDIRECT_SHADER IndirectShader,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _In_ PCVECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_opt_ PRAYSHADER RayTracer,
    _Out_ PCOLOR3 Indirect
    )
{
    ISTATUS Status;

    ASSERT(IndirectShader != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(Indirect != NULL);

    Status = IndirectShader->IndirectShaderVTable->IndirectRoutine(IndirectShader,
                                                                   WorldHitPoint,
                                                                   ModelHitPoint,
                                                                   WorldViewer,
                                                                   ModelViewer,
                                                                   AdditionalData,
                                                                   SurfaceNormal,
                                                                   Rng,
                                                                   VisibilityTester,
                                                                   RayTracer,
                                                                   Indirect);

    return Status;
}

SFORCEINLINE
VOID
IndirectShaderFree(
    _In_opt_ _Post_invalid_ PINDIRECT_SHADER IndirectShader
    )
{
    if (IndirectShader == NULL)
    {
        return;
    }

    IndirectShader->IndirectShaderVTable->FreeRoutine(IndirectShader);
}

#endif // _INDIRECT_SHADER_IRIS_SHADING_MODEL_