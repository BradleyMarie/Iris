/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_normal.h

Abstract:

    This file contains the internal for the NORMAL type.

--*/

#ifndef _NORMAL_IRIS_SHADING_MODEL_INTERNAL_
#define _NORMAL_IRIS_SHADING_MODEL_INTERNAL_

#include <irisshadingmodelp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
NormalComputeNormal(
    _In_ PCNORMAL Normal, 
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    ISTATUS Status;

    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Normal != NULL);
    
    if (Normal->NormalVTable->IsModelNormal != FALSE)
    {
        Status = Normal->NormalVTable->ComputeNormalRoutine(Normal,
                                                            ModelHitPoint,
                                                            AdditionalData,
                                                            SurfaceNormal);
    }
    else
    {
        Status = Normal->NormalVTable->ComputeNormalRoutine(Normal,
                                                            WorldHitPoint,
                                                            AdditionalData,
                                                            SurfaceNormal);
    }

    return Status;
}

#endif // _NORMAL_IRIS_SHADING_MODEL_INTERNAL_