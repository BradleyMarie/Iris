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
PBRHitTesterAdapter(
    _In_opt_ PCVOID Context, 
    _Inout_ PHIT_TESTER HitTester,
    _In_ RAY Ray
    )
{
    PCPBR_HIT_TESTER_ADAPTER_CONTEXT PBRHitTesterAdapterContext;
    PBR_HIT_TESTER PBRHitTester;
    ISTATUS Status;
    
    PBRHitTesterInitialize(&PBRHitTester,
                           HitTester);
    
    PBRHitTesterAdapterContext = (PCPBR_HIT_TESTER_ADAPTER_CONTEXT) Context;
    
    Status = PBRHitTesterAdapterContext->TestGeometryRoutine(PBRHitTesterAdapterContext->TestGeometryRoutineContext,
                                                             &PBRHitTester,
                                                             Ray);

    return Status;
}