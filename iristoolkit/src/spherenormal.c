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
    _In_ POINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCSPHERE_NORMAL SphereNormal;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);

    SphereNormal = (PCSPHERE_NORMAL) Context;

    *SurfaceNormal = PointSubtract(ModelHitPoint, SphereNormal->Center);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SphereNormalComputeBackNormal(
    _In_ PCVOID Context, 
    _In_ POINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCSPHERE_NORMAL SphereNormal;

    ASSERT(Context != NULL);
    ASSERT(SurfaceNormal != NULL);

    SphereNormal = (PCSPHERE_NORMAL) Context;

    *SurfaceNormal = PointSubtract(SphereNormal->Center, ModelHitPoint);

    return ISTATUS_SUCCESS;
}

//
// Static variables
//

CONST STATIC NORMAL_VTABLE SphereNormalFrontHeader = {
    SphereNormalComputeFrontNormal,
    NULL,
    FALSE
};

CONST STATIC NORMAL_VTABLE SphereNormalBackHeader = {
    SphereNormalComputeBackNormal,
    NULL,
    FALSE
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
PNORMAL
SphereNormalAllocate(
    _In_ POINT3 Center,
    _In_ BOOL Front
    )
{
    PCNORMAL_VTABLE SphereNormalVTable;
    SPHERE_NORMAL SphereNormal;
    PNORMAL Normal;
    BOOL Valid;

    Valid = PointValidate(Center);

    if (Valid == FALSE)
    {
        return NULL;
    }
    
    if (Front != FALSE)
    {
        SphereNormalVTable = &SphereNormalFrontHeader;
    }
    else
    {
        SphereNormalVTable = &SphereNormalBackHeader;
    }

    SphereNormal.Center = Center;

    Normal = NormalAllocate(SphereNormalVTable,
                            &SphereNormal,
                            sizeof(SPHERE_NORMAL),
                            sizeof(PVOID));

    return Normal;
}