/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    spherenormal.c

Abstract:

    This file contains the definitions for the SPHERE_NORMAL type.

--*/

#include <iristoolkitp.h>

//
// Types
//

typedef struct _SPHERE_NORMAL {
    NORMAL NormalHeader;
    POINT3 Center;
} SPHERE_NORMAL, *PSPHERE_NORMAL;

typedef CONST SPHERE_NORMAL *PCSPHERE_NORMAL;

//
// Static functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SphereNormalComputeFrontNormal(
    _In_ PCVOID Context, 
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCSPHERE_NORMAL SphereNormal;

    ASSERT(Context != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(SurfaceNormal != NULL);

    SphereNormal = (PCSPHERE_NORMAL) Context;

    *SurfaceNormal = PointSubtract(*ModelHitPoint, SphereNormal->Center);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SphereNormalComputeBackNormal(
    _In_ PCVOID Context, 
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCSPHERE_NORMAL SphereNormal;

    ASSERT(Context != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(SurfaceNormal != NULL);

    SphereNormal = (PCSPHERE_NORMAL) Context;

    *SurfaceNormal = PointSubtract(SphereNormal->Center, *ModelHitPoint);

    return ISTATUS_SUCCESS;
}

//
// Static variables
//

CONST STATIC NORMAL_VTABLE SphereNormalFrontHeader = {
    SphereNormalComputeFrontNormal,
    free,
    FALSE
};

CONST STATIC NORMAL_VTABLE SphereNormalBackHeader = {
    SphereNormalComputeBackNormal,
    free,
    FALSE
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
PNORMAL
SphereNormalAllocate(
    _In_ PCPOINT3 Center,
    _In_ BOOL Front
    )
{
    PSPHERE_NORMAL SphereNormal;

    if (Center == NULL)
    {
        return NULL;
    }

    SphereNormal = (PSPHERE_NORMAL) malloc(sizeof(SPHERE_NORMAL));

    if (SphereNormal == NULL)
    {
        return NULL;
    }

    if (Front != FALSE)
    {
        SphereNormal->NormalHeader.NormalVTable = &SphereNormalFrontHeader;
    }
    else
    {
        SphereNormal->NormalHeader.NormalVTable = &SphereNormalBackHeader;
    }

    SphereNormal->Center = *Center;

    return (PNORMAL) SphereNormal;
}