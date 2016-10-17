/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    hittester.c

Abstract:

    This file contains the definitions for the PBR_HIT_TESTER type.

--*/

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRHitTesterTestGeometry(
    _Inout_ PPBR_HIT_TESTER PBRHitTester,
    _In_ PCPHYSX_GEOMETRY Geometry
    )
{
    ISTATUS Status;
    
    if (PBRHitTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    Status = HitTesterTestGeometry(PBRHitTester->Tester,
                                   PhysxGeometryTestRayAdapter,
                                   Geometry);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRHitTesterTestGeometryWithTransform(
    _Inout_ PPBR_HIT_TESTER PBRHitTester,
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_opt_ PCMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    ISTATUS Status;
    
    if (PBRHitTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    Status = HitTesterTestGeometryWithTransform(PBRHitTester->Tester,
                                                PhysxGeometryTestRayAdapter,
                                                Geometry,
                                                ModelToWorld,
                                                Premultiplied);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PBRHitTesterTestPremultipliedGeometryWithTransform(
    _Inout_ PPBR_HIT_TESTER PBRHitTester,
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_opt_ PCMATRIX ModelToWorld
    )
{
    ISTATUS Status;
    
    if (PBRHitTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    Status = HitTesterTestPremultipliedGeometryWithTransform(PBRHitTester->Tester,
                                                             PhysxGeometryTestRayAdapter,
                                                             Geometry,
                                                             ModelToWorld);

    return Status;
}
