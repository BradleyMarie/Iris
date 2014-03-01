/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_surfacenormal.h

Abstract:

    This file contains the internal definitions for the SURFACE_NORMAL type.

--*/

#ifndef _SURFACE_NORMAL_IRIS_SHADING_MODEL_INTERNAL_
#define _SURFACE_NORMAL_IRIS_SHADING_MODEL_INTERNAL_

#include <irisshadingmodel.h>

//
// Constants
//

#define PRENORMALIZED_MODEL 0
#define PRENORMALIZED_WORLD 1
#define PRENORMALIZED_NONE  2

//
// Types
//

struct _SURFACE_NORMAL {
    BOOL NormalizedWorldNormalValid;
    BOOL NormalizedModelNormalValid;
    BOOL WorldNormalValid;
    BOOL ModelNormalValid;
    VECTOR3 NormalizedWorldNormal;
    VECTOR3 NormalizedModelNormal;
    VECTOR3 WorldNormal;
    VECTOR3 ModelNormal;
    PCNORMAL Normal;
    PCMATRIX WorldToModel;
    PCPOINT3 WorldHit;
    PCPOINT3 ModelHit;
    PCVOID AdditionalData;
    UINT8 PrenormalizedTerm;
    BOOL IsModelNormal;
};

//
// Functions
//

SFORCEINLINE
VOID
SurfaceNormalInitialize(
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _In_ PCNORMAL Normal,
    _In_ PCPOINT3 WorldHit,
    _In_ PCPOINT3 ModelHit,
    _In_ PCMATRIX ModelToWorld,
    _In_opt_ PCVOID AdditionalData
    )
{
    PCNORMAL_VTABLE NormalVTable;

    ASSERT(SurfaceNormal != NULL);
    ASSERT(Normal != NULL);
    ASSERT(WorldHit != NULL);
    ASSERT(ModelHit != NULL);
    ASSERT(ModelToWorld != NULL);

    SurfaceNormal->Normal = Normal;
    SurfaceNormal->WorldHit = WorldHit;
    SurfaceNormal->ModelHit = ModelHit;
    SurfaceNormal->WorldToModel = ModelToWorld->Inverse;
    SurfaceNormal->AdditionalData = AdditionalData;

    NormalVTable = Normal->NormalVTable;

    if (NormalVTable->Prenormalized != FALSE)
    {
        if (NormalVTable->IsModelNormal != FALSE)
        {
            SurfaceNormal->PrenormalizedTerm = PRENORMALIZED_MODEL;
            SurfaceNormal->IsModelNormal = TRUE;
        }
        else
        {
            SurfaceNormal->PrenormalizedTerm = PRENORMALIZED_WORLD;
            SurfaceNormal->IsModelNormal = FALSE;
        }
    }
    else
    {
        SurfaceNormal->PrenormalizedTerm = PRENORMALIZED_NONE;
        SurfaceNormal->IsModelNormal = NormalVTable->IsModelNormal;
    }

    SurfaceNormal->WorldNormalValid = FALSE;
    SurfaceNormal->ModelNormalValid = FALSE;
    SurfaceNormal->NormalizedModelNormalValid = FALSE;
    SurfaceNormal->NormalizedWorldNormalValid = FALSE;
}

#endif // _SURFACE_NORMAL_IRIS_SHADING_MODEL_INTERNAL_