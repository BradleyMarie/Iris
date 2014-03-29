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

    if (SurfaceNormal->ModelNormalValid != FALSE)
    {
        VectorMatrixTransposedMultiply(SurfaceNormal->WorldToModel,
                                       &SurfaceNormal->ModelNormal,
                                       WorldNormal);
    }
    else
    {
        Status = NormalComputeNormal(SurfaceNormal->Normal,
                                     SurfaceNormal->WorldHit,
                                     SurfaceNormal->ModelHit,
                                     SurfaceNormal->AdditionalData,
                                     WorldNormal);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        if (SurfaceNormal->IsModelNormal != FALSE)
        {
            SurfaceNormal->ModelNormal = *WorldNormal;
            SurfaceNormal->ModelNormalValid = TRUE;

            VectorMatrixTransposedMultiply(SurfaceNormal->WorldToModel,
                                           WorldNormal,
                                           WorldNormal);
        }
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

    if (SurfaceNormal->WorldNormalValid != FALSE)
    {
        VectorMatrixTransposedMultiply(SurfaceNormal->WorldToModel->Inverse,
                                       &SurfaceNormal->WorldNormal,
                                       ModelNormal);
    }
    else
    {
        Status = NormalComputeNormal(SurfaceNormal->Normal,
                                     SurfaceNormal->WorldHit,
                                     SurfaceNormal->ModelHit,
                                     SurfaceNormal->AdditionalData,
                                     ModelNormal);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        if (SurfaceNormal->IsModelNormal == FALSE)
        {
            SurfaceNormal->WorldNormal = *ModelNormal;
            SurfaceNormal->WorldNormalValid = TRUE;

            VectorMatrixTransposedMultiply(SurfaceNormal->WorldToModel->Inverse,
                                           ModelNormal,
                                           ModelNormal);
        }
    }

    SurfaceNormal->ModelNormal = *ModelNormal;
    SurfaceNormal->ModelNormalValid = TRUE;

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

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (SurfaceNormal->PrenormalizedTerm == PRENORMALIZED_NONE)
    {
        VectorNormalize(NormalizedWorldNormal, NormalizedWorldNormal);
    }

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

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (SurfaceNormal->PrenormalizedTerm == PRENORMALIZED_NONE)
    {
        VectorNormalize(NormalizedModelNormal, NormalizedModelNormal);
    }

    SurfaceNormal->NormalizedModelNormal = *NormalizedModelNormal;
    SurfaceNormal->NormalizedModelNormalValid = TRUE;

    return ISTATUS_SUCCESS;
}