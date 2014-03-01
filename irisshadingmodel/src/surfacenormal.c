/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    surfacenormal.c

Abstract:

    This file contains the function definitions for the SURFACE_NORMAL type.

--*/

#include <irisshadingmodelp.h>

VOID
SuraceNormalGetWorldNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 WorldNormal
    )
{
    ASSERT(SurfaceNormal != NULL);
    ASSERT(WorldNormal != NULL);

    if (SurfaceNormal->WorldNormalValid != FALSE)
    {
        *WorldNormal = SurfaceNormal->WorldNormal;
        return;
    }

    if (SurfaceNormal->ModelNormalValid != FALSE)
    {
        VectorMatrixTransposedMultiply(SurfaceNormal->WorldToModel,
                                       &SurfaceNormal->ModelNormal,
                                       WorldNormal);
    }
    else
    {
        NormalComputeNormal(SurfaceNormal->Normal,
                            SurfaceNormal->WorldHit,
                            SurfaceNormal->ModelHit,
                            SurfaceNormal->AdditionalData,
                            WorldNormal);

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
}

VOID
SuraceNormalGetModelNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 ModelNormal
    )
{
    ASSERT(SurfaceNormal != NULL);
    ASSERT(ModelNormal != NULL);

    if (SurfaceNormal->ModelNormalValid != FALSE)
    {
        *ModelNormal = SurfaceNormal->ModelNormal;
        return;
    }

    if (SurfaceNormal->WorldNormalValid != FALSE)
    {
        VectorMatrixTransposedMultiply(SurfaceNormal->WorldToModel->Inverse,
                                       &SurfaceNormal->WorldNormal,
                                       ModelNormal);
    }
    else
    {
        NormalComputeNormal(SurfaceNormal->Normal,
                            SurfaceNormal->WorldHit,
                            SurfaceNormal->ModelHit,
                            SurfaceNormal->AdditionalData,
                            ModelNormal);

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
}

VOID
SuraceNormalGetNormalizedWorldNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 NormalizedWorldNormal
    )
{
    ASSERT(SurfaceNormal != NULL);
    ASSERT(NormalizedWorldNormal != NULL);

    if (SurfaceNormal->NormalizedWorldNormalValid != FALSE)
    {
        *NormalizedWorldNormal = SurfaceNormal->NormalizedWorldNormal;
        return;
    }

    SuraceNormalGetWorldNormal(SurfaceNormal, NormalizedWorldNormal);

    if (SurfaceNormal->PrenormalizedTerm == PRENORMALIZED_NONE)
    {
        VectorNormalize(NormalizedWorldNormal, NormalizedWorldNormal);
    }

    SurfaceNormal->NormalizedWorldNormal = *NormalizedWorldNormal;
    SurfaceNormal->NormalizedWorldNormalValid = TRUE;
}

VOID
SuraceNormalGetNormalizedModelNormal(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Out_ PVECTOR3 NormalizedModelNormal
    )
{
    ASSERT(SurfaceNormal != NULL);
    ASSERT(NormalizedModelNormal != NULL);

    if (SurfaceNormal->NormalizedModelNormalValid != FALSE)
    {
        *NormalizedModelNormal = SurfaceNormal->NormalizedModelNormal;
        return;
    }

    SuraceNormalGetModelNormal(SurfaceNormal, NormalizedModelNormal);

    if (SurfaceNormal->PrenormalizedTerm == PRENORMALIZED_NONE)
    {
        VectorNormalize(NormalizedModelNormal, NormalizedModelNormal);
    }

    SurfaceNormal->NormalizedModelNormal = *NormalizedModelNormal;
    SurfaceNormal->NormalizedModelNormalValid = TRUE;
}