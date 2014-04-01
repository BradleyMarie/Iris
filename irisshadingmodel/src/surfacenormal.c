/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    surfacenormal.c

Abstract:

    This file contains the function definitions for the SURFACE_NORMAL type.

--*/

#include <irisshadingmodelp.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SuraceNormalGetWorldNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 WorldNormal
    )
{
    ISTATUS Status;

    ASSERT(SurfaceNormal != NULL);
    ASSERT(WorldNormal != NULL);

    if (SurfaceNormal->WorldNormalValid != FALSE)
    {
        *WorldNormal = SurfaceNormal->WorldNormal;
        return ISTATUS_SUCCESS;
    }

    Status = SuraceNormalGetModelNormal(SurfaceNormal, WorldNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (SurfaceNormal->WorldToModel != NULL)
    {
        VectorMatrixTransposedMultiply(SurfaceNormal->WorldToModel,
                                       WorldNormal,
                                       WorldNormal);
    }
    else if (SurfaceNormal->Prenormalized != FALSE)
    {
        SurfaceNormal->NormalizedWorldNormal = *WorldNormal;
        SurfaceNormal->NormalizedWorldNormalValid = TRUE;
    }

    SurfaceNormal->WorldNormal = *WorldNormal;
    SurfaceNormal->WorldNormalValid = TRUE;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SuraceNormalGetModelNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 ModelNormal
    )
{
    ISTATUS Status;

    ASSERT(SurfaceNormal != NULL);
    ASSERT(ModelNormal != NULL);

    if (SurfaceNormal->ModelNormalValid != FALSE)
    {
        *ModelNormal = SurfaceNormal->ModelNormal;
        return ISTATUS_SUCCESS;
    }

    Status = NormalComputeNormal(SurfaceNormal->Normal,
                                 SurfaceNormal->ModelHit,
                                 SurfaceNormal->AdditionalData,
                                 ModelNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    SurfaceNormal->ModelNormal = *ModelNormal;
    SurfaceNormal->ModelNormalValid = TRUE;

    if (SurfaceNormal->Prenormalized != FALSE)
    {
        SurfaceNormal->NormalizedModelNormal = *ModelNormal;
        SurfaceNormal->NormalizedModelNormalValid = TRUE;
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SuraceNormalGetNormalizedWorldNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 NormalizedWorldNormal
    )
{
    ISTATUS Status;

    ASSERT(SurfaceNormal != NULL);
    ASSERT(NormalizedWorldNormal != NULL);

    if (SurfaceNormal->NormalizedWorldNormalValid != FALSE)
    {
        *NormalizedWorldNormal = SurfaceNormal->NormalizedWorldNormal;
        return ISTATUS_SUCCESS;
    }

    Status = SuraceNormalGetWorldNormal(SurfaceNormal, NormalizedWorldNormal);

    if (Status != ISTATUS_SUCCESS ||
        SurfaceNormal->NormalizedWorldNormalValid != FALSE)
    {
        return Status;
    }

    VectorNormalize(NormalizedWorldNormal, NormalizedWorldNormal);

    SurfaceNormal->NormalizedWorldNormal = *NormalizedWorldNormal;
    SurfaceNormal->NormalizedWorldNormalValid = TRUE;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SuraceNormalGetNormalizedModelNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 NormalizedModelNormal
    )
{
    ISTATUS Status;

    ASSERT(SurfaceNormal != NULL);
    ASSERT(NormalizedModelNormal != NULL);

    if (SurfaceNormal->NormalizedModelNormalValid != FALSE)
    {
        *NormalizedModelNormal = SurfaceNormal->NormalizedModelNormal;
        return ISTATUS_SUCCESS;
    }

    Status = SuraceNormalGetModelNormal(SurfaceNormal, NormalizedModelNormal);

    if (Status != ISTATUS_SUCCESS ||
        SurfaceNormal->NormalizedModelNormalValid != FALSE)
    {
        return Status;
    }

    VectorNormalize(NormalizedModelNormal, NormalizedModelNormal);

    SurfaceNormal->NormalizedModelNormal = *NormalizedModelNormal;
    SurfaceNormal->NormalizedModelNormalValid = TRUE;

    return ISTATUS_SUCCESS;
}