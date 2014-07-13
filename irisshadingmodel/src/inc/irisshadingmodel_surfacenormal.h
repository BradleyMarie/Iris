/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_surfacenormal.h

Abstract:

    This file contains the internal definitions for the SURFACE_NORMAL type.

--*/

#ifndef _SURFACE_NORMAL_IRIS_SHADING_MODEL_INTERNAL_
#define _SURFACE_NORMAL_IRIS_SHADING_MODEL_INTERNAL_

#include <irisshadingmodelp.h>

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
    PCMATRIX ModelToWorld;
    POINT3 ModelHit;
    PCVOID AdditionalData;
    BOOL Prenormalized;
};

//
// Functions
//

SFORCEINLINE
VOID
SurfaceNormalInitialize(
    _Out_ PSURFACE_NORMAL SurfaceNormal,
    _In_ PCNORMAL Normal,
    _In_ POINT3 ModelHit,
    _In_opt_ PCMATRIX ModelToWorld,
    _In_opt_ PCVOID AdditionalData
    )
{
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Normal != NULL);

    SurfaceNormal->Normal = Normal;
    SurfaceNormal->ModelHit = ModelHit;
    SurfaceNormal->AdditionalData = AdditionalData;
    SurfaceNormal->WorldNormalValid = FALSE;
    SurfaceNormal->ModelNormalValid = FALSE;
    SurfaceNormal->NormalizedModelNormalValid = FALSE;
    SurfaceNormal->NormalizedWorldNormalValid = FALSE;
    SurfaceNormal->Prenormalized = Normal->NormalVTable->Prenormalized;

    if (ModelToWorld != NULL)
    {
        SurfaceNormal->ModelToWorld = ModelToWorld;
    }
    else
    {
        SurfaceNormal->ModelToWorld = NULL;
    }
}

#endif // _SURFACE_NORMAL_IRIS_SHADING_MODEL_INTERNAL_