/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_constantnormal.h

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

typedef
VOID
ConstantNormalComputeNormal(
    _In_ PCVOID Context, 
    _In_ PCPOINT3 HitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCONSTANT_NORMAL ConstantNormal;

    ASSERT(Context != NULL);
    ASSERT(HitPoint != NULL);
    ASSERT(SurfaceNormal != NULL);

    ConstantNormal = (PCONSTANT_NORMAL) Context;

    *SurfaceNormal = ConstantNormal->Normal;
}

//
// Static variables
//

STATIC NORMAL_VTABLE ConstantNormalWorldHeader = {
    ConstantNormalComputeNormal,
    free,
    FALSE,
    FALSE
};

STATIC NORMAL_VTABLE ConstantNormalModelHeader = {
    ConstantNormalComputeNormal,
    free,
    TRUE,
    FALSE
};

STATIC NORMAL_VTABLE ConstantNormalPremultipliedWorldHeader = {
    ConstantNormalComputeNormal,
    free,
    FALSE,
    TRUE
};

STATIC NORMAL_VTABLE ConstantNormalPremultipliedModelHeader = {
    ConstantNormalComputeNormal,
    free,
    TRUE,
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
    _In_ UINT8 NormalType
    )
{
    PCONSTANT_NORMAL ConstantNormal;
    BOOL Prenormalized;

    ASSERT(Normal != NULL);

    ValidType = VerifyNormalType(NormalType);

    if (ValidType == FALSE)
    {
        return NULL;
    }

    ConstantNormal = (PCONSTANT_NORMAL) malloc(sizeof(CONSTANT_NORMAL))

    if (ConstantNormal == NULL)
    {
        return NULL;
    }

    switch (NormalType)
    {
        case IRIS_TOOLKIT_NORMAL_WORLD:
            ConstantNormal->NormalHeader.NormalVTable = ConstantNormalWorldHeader;
            break;
        case IRIS_TOOLKIT_NORMAL_MODEL:
            ConstantNormal->NormalHeader.NormalVTable = ConstantNormalModelHeader;
            break;
        case IRIS_TOOLKIT_NORMAL_WORLD_PRENORMALIZED:
            ConstantNormal->NormalHeader.NormalVTable = ConstantNormalPrenormalizedWorldHeader;
            break;
        case IRIS_TOOLKIT_NORMAL_MODEL_PRENORMALIZED:
            ConstantNormal->NormalHeader.NormalVTable = ConstantNormalPrenormalizedModelHeader;
            break;
        default:
            ASSERT(FALSE);
            break;
    }

    ConstantNormal->Normal = *Normal;

    return (PNORMAL) ConstantNormal;
}