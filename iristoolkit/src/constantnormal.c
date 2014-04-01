/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    constantnormal.c

Abstract:

    This file contains the definitions for the CONSTANT_NORMAL type.

--*/

#include <iristoolkitp.h>

//
// Types
//

typedef struct _CONSTANT_NORMAL {
    NORMAL NormalHeader;
    VECTOR3 Normal;
} CONSTANT_NORMAL, *PCONSTANT_NORMAL;

typedef CONST CONSTANT_NORMAL *PCCONSTANT_NORMAL;

//
// Static functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
ConstantNormalComputeNormal(
    _In_ PCVOID Context, 
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCONSTANT_NORMAL ConstantNormal;

    ASSERT(Context != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(SurfaceNormal != NULL);

    ConstantNormal = (PCONSTANT_NORMAL) Context;

    *SurfaceNormal = ConstantNormal->Normal;

    return ISTATUS_SUCCESS;
}

//
// Static variables
//

CONST STATIC NORMAL_VTABLE ConstantNormalHeader = {
    ConstantNormalComputeNormal,
    free,
    FALSE
};

CONST STATIC NORMAL_VTABLE ConstantNormalPrenormalizedHeader = {
    ConstantNormalComputeNormal,
    free,
    TRUE
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
PNORMAL
ConstantNormalAllocate(
    _In_ PCVECTOR3 Normal,
    _In_ BOOL Normalize
    )
{
    PCONSTANT_NORMAL ConstantNormal;

    if (Normal == FALSE)
    {
        return NULL;
    }

    ConstantNormal = (PCONSTANT_NORMAL) malloc(sizeof(CONSTANT_NORMAL));

    if (ConstantNormal == NULL)
    {
        return NULL;
    }

    if (Normalize != FALSE)
    {
        ConstantNormal->NormalHeader.NormalVTable = &ConstantNormalPrenormalizedHeader;
        VectorNormalize(Normal, &ConstantNormal->Normal);
    }
    else
    {
        ConstantNormal->NormalHeader.NormalVTable = &ConstantNormalHeader;
        ConstantNormal->Normal = *Normal;
    }

    return (PNORMAL) ConstantNormal;
}