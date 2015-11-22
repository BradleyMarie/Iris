/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_visibilitytester.h

Abstract:

    This file contains the internal definitions for the 
    SPECTRUM_VISIBILITY_TESTER type.

--*/

#ifndef _SPECTRUM_VISIBILITY_TESTER_IRIS_PHYSX_INTERNAL_
#define _SPECTRUM_VISIBILITY_TESTER_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _SPECTRUM_VISIBILITY_TESTER {
    PRAYTRACER_OWNER RayTracerOwner;
    FLOAT Epsilon;
    PCSCENE Scene;
};

//
// VisibilityTester Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
SpectrumVisibilityTesterInitialize(
    _Out_ PSPECTRUM_VISIBILITY_TESTER Tester
    )
{
    PRAYTRACER_OWNER RayTracerOwner;
    VECTOR3 TemporaryDirection;
    POINT3 TemporaryOrigin;
    RAY TemporaryRay;
    ISTATUS Status;

    ASSERT(Tester != NULL);

    TemporaryDirection = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
    TemporaryOrigin = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    TemporaryRay = RayCreate(TemporaryOrigin, TemporaryDirection);

    Status = RayTracerOwnerAllocate(TemporaryRay, &RayTracerOwner);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Tester->Scene = NULL;
    Tester->RayTracerOwner = RayTracerOwner;
    Tester->Epsilon = (FLOAT) 0.0;

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
SpectrumVisibilityTesterSetSceneAndEpsilon(
    _Inout_ PSPECTRUM_VISIBILITY_TESTER VisibilityTester,
    _In_ PCSCENE Scene,
    _In_ FLOAT Epsilon
    )
{
    ASSERT(VisibilityTester != NULL);
    ASSERT(Scene != NULL);
    ASSERT(IsNormalFloat(Epsilon) != FALSE);
    ASSERT(IsFiniteFloat(Epsilon) != FALSE);
    ASSERT(Epsilon < (FLOAT) 0.0);
    
    VisibilityTester->Scene = Scene;
    VisibilityTester->Epsilon = Epsilon;
}

SFORCEINLINE
VOID
SpectrumVisibilityTesterDestroy(
    _In_opt_ _Post_invalid_ PSPECTRUM_VISIBILITY_TESTER Tester
    )
{
    ASSERT(Tester != NULL);

    RayTracerOwnerFree(Tester->RayTracerOwner);
}

#endif // _SPECTRUM_VISIBILITY_TESTER_IRIS_PHYSX_INTERNAL_