/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    surfacenormal.c

Abstract:

    This file contains the function definitions for the SURFACE_NORMAL type.

--*/

#include <irisrgbp.h>

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SurfaceNormalGetWorldNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 WorldNormal
    )
{
    ISTATUS Status;

    if (SurfaceNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (WorldNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (SurfaceNormal->WorldNormalValid != FALSE)
    {
        *WorldNormal = SurfaceNormal->WorldNormal;
        return ISTATUS_SUCCESS;
    }

    Status = SurfaceNormalGetModelNormal(SurfaceNormal, WorldNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (SurfaceNormal->ModelToWorld != NULL)
    {
        *WorldNormal = VectorMatrixInverseTransposedMultiply(SurfaceNormal->ModelToWorld,
                                                             *WorldNormal);
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
SurfaceNormalGetModelNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 ModelNormal
    )
{
    ISTATUS Status;

    if (SurfaceNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (ModelNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

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
SurfaceNormalGetNormalizedWorldNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 NormalizedWorldNormal
    )
{
    ISTATUS Status;

    if (SurfaceNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (NormalizedWorldNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (SurfaceNormal->NormalizedWorldNormalValid != FALSE)
    {
        *NormalizedWorldNormal = SurfaceNormal->NormalizedWorldNormal;
        return ISTATUS_SUCCESS;
    }

    Status = SurfaceNormalGetWorldNormal(SurfaceNormal, NormalizedWorldNormal);

    if (Status != ISTATUS_SUCCESS ||
        SurfaceNormal->NormalizedWorldNormalValid != FALSE)
    {
        return Status;
    }

    *NormalizedWorldNormal = VectorNormalize(*NormalizedWorldNormal, NULL);

    SurfaceNormal->NormalizedWorldNormal = *NormalizedWorldNormal;
    SurfaceNormal->NormalizedWorldNormalValid = TRUE;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SurfaceNormalGetNormalizedModelNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 NormalizedModelNormal
    )
{
    ISTATUS Status;

    if (SurfaceNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (NormalizedModelNormal == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (SurfaceNormal->NormalizedModelNormalValid != FALSE)
    {
        *NormalizedModelNormal = SurfaceNormal->NormalizedModelNormal;
        return ISTATUS_SUCCESS;
    }

    Status = SurfaceNormalGetModelNormal(SurfaceNormal, NormalizedModelNormal);

    if (Status != ISTATUS_SUCCESS ||
        SurfaceNormal->NormalizedModelNormalValid != FALSE)
    {
        return Status;
    }

    *NormalizedModelNormal = VectorNormalize(*NormalizedModelNormal, NULL);

    SurfaceNormal->NormalizedModelNormal = *NormalizedModelNormal;
    SurfaceNormal->NormalizedModelNormalValid = TRUE;

    return ISTATUS_SUCCESS;
}