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
    NULL,
    FALSE
};

CONST STATIC NORMAL_VTABLE ConstantNormalPrenormalizedHeader = {
    ConstantNormalComputeNormal,
    NULL,
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
    PCNORMAL_VTABLE ConstantNormalVTable;
    CONSTANT_NORMAL ConstantNormal;
    PNORMAL SurfaceNormal;
    BOOL Valid;

    if (Normal == FALSE)
    {
        return NULL;
    }

    Valid = VectorValidate(*Normal);

    if (Valid == FALSE)
    {
        return NULL;
    }

    if (Normalize != FALSE)
    {
        ConstantNormalVTable = &ConstantNormalPrenormalizedHeader;
        ConstantNormal.Normal = VectorNormalize(*Normal, NULL);
    }
    else
    {
        ConstantNormalVTable = &ConstantNormalHeader;
        ConstantNormal.Normal = *Normal;
    }

    SurfaceNormal = NormalAllocate(ConstantNormalVTable,
                                   &ConstantNormal,
                                   sizeof(CONSTANT_NORMAL),
                                   sizeof(PVOID));

    return SurfaceNormal;
}