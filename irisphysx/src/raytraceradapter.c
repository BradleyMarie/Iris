/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    raytraceradapter.c

Abstract:

    This file contains the definitions for the RayTracer adapter functions.

--*/

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxHitTesterAdapter(
    _In_opt_ PCVOID Context, 
    _Inout_ PHIT_TESTER HitTester,
    _In_ RAY Ray
    )
{
    PCPHYSX_HIT_TESTER_ADAPTER_CONTEXT HitTesterAdapterContext;
    PHYSX_HIT_TESTER PhysxHitTester;
    ISTATUS Status;
    
    PhysxHitTesterInitialize(&PhysxHitTester,
                             HitTester);
    
    HitTesterAdapterContext = (PCPHYSX_HIT_TESTER_ADAPTER_CONTEXT) Context;
    
    Status = HitTesterAdapterContext->TestGeometryRoutine(HitTesterAdapterContext->TestGeometryRoutineContext,
                                                          &PhysxHitTester,
                                                          Ray);

    return Status;
}
