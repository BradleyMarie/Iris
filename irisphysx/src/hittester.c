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
PhysxHitTesterTestGeometry(
    _Inout_ PPHYSX_HIT_TESTER HitTester,
    _In_ PCPHYSX_GEOMETRY Geometry
    )
{
    ISTATUS Status;
    
    if (HitTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    Status = HitTesterTestGeometry(HitTester->Tester,
                                   PhysxGeometryTestRayAdapter,
                                   Geometry);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxHitTesterTestGeometryWithTransform(
    _Inout_ PPHYSX_HIT_TESTER HitTester,
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_opt_ PCMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    )
{
    ISTATUS Status;
    
    if (HitTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    Status = HitTesterTestGeometryWithTransform(HitTester->Tester,
                                                PhysxGeometryTestRayAdapter,
                                                Geometry,
                                                ModelToWorld,
                                                Premultiplied);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxHitTesterTestPremultipliedGeometryWithTransform(
    _Inout_ PPHYSX_HIT_TESTER HitTester,
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_opt_ PCMATRIX ModelToWorld
    )
{
    ISTATUS Status;
    
    if (HitTester == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (Geometry == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    Status = HitTesterTestPremultipliedGeometryWithTransform(HitTester->Tester,
                                                             PhysxGeometryTestRayAdapter,
                                                             Geometry,
                                                             ModelToWorld);

    return Status;
}
