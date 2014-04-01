/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_normal.h

Abstract:

    This file contains the definitions for the NORMAL type.

--*/

#ifndef _NORMAL_IRIS_SHADING_MODEL_
#define _NORMAL_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PNORMAL_COMPUTE_NORMAL_ROUTINE)(
    _In_ PCVOID Context, 
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    );

typedef struct _NORMAL_VTABLE {
    PNORMAL_COMPUTE_NORMAL_ROUTINE ComputeNormalRoutine;
    PFREE_ROUTINE FreeRoutine;
    BOOL Prenormalized;
} NORMAL_VTABLE, *PNORMAL_VTABLE;

typedef CONST NORMAL_VTABLE *PCNORMAL_VTABLE;

typedef struct _NORMAL {
    PCNORMAL_VTABLE NormalVTable;
} NORMAL, *PNORMAL;

typedef CONST NORMAL *PCNORMAL;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
NormalComputeNormal(
    _In_ PCNORMAL Normal, 
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    ISTATUS Status;

    ASSERT(ModelHitPoint != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Normal != NULL);
    
    Status = Normal->NormalVTable->ComputeNormalRoutine(Normal,
                                                        ModelHitPoint,
                                                        AdditionalData,
                                                        SurfaceNormal);

    return Status;
}

SFORCEINLINE
VOID
NormalFree(
    _Pre_maybenull_ _Post_invalid_ PNORMAL Normal
    )
{
    if (Normal == NULL)
    {
        return;
    }

    Normal->NormalVTable->FreeRoutine(Normal);
}

#endif // _NORMAL_IRIS_SHADING_MODEL_