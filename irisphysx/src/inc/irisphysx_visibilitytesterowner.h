/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_visibilitytesterowner.h

Abstract:

    This file contains the internal definitions for the 
    SPECTRUM_VISIBILITY_TESTER_OWNER type.

--*/

#ifndef _SPECTRUM_VISIBILITY_TESTER_OWNER_IRIS_PHYSX_INTERNAL_
#define _SPECTRUM_VISIBILITY_TESTER_OWNER_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

struct _SPECTRUM_VISIBILITY_TESTER_OWNER {
    SPECTRUM_VISIBILITY_TESTER VisibilityTester;  
};

typedef struct _SPECTRUM_VISIBILITY_TESTER_OWNER SPECTRUM_VISIBILITY_TESTER_OWNER, *PSPECTRUM_VISIBILITY_TESTER_OWNER;
typedef CONST SPECTRUM_VISIBILITY_TESTER_OWNER *PCSPECTRUM_VISIBILITY_TESTER_OWNER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
SpectrumVisibilityTesterOwnerInitialize(
    _Out_ PSPECTRUM_VISIBILITY_TESTER_OWNER VisibilityTesterOwner
    )
{
    ISTATUS Status;

    ASSERT(VisibilityTesterOwner != NULL);
    
    Status = SpectrumVisibilityTesterInitialize(&VisibilityTesterOwner->VisibilityTester);
                                        
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
SpectrumVisibilityTesterOwnerSetSceneAndEpsilon(
    _Inout_ PSPECTRUM_VISIBILITY_TESTER_OWNER VisibilityTesterOwner,
    _In_ PSCENE Scene,
    _In_ FLOAT Epsilon
    )
{
    ASSERT(VisibilityTesterOwner != NULL);
    ASSERT(Scene != NULL);
    ASSERT(IsFiniteFloat(Epsilon) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(Epsilon) != FALSE);
    
    SpectrumVisibilityTesterSetSceneAndEpsilon(&VisibilityTesterOwner->VisibilityTester,
                                               Scene,
                                               Epsilon);
                                       
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
SpectrumVisibilityTesterOwnerGetVisibilityTester(
    _In_ PSPECTRUM_VISIBILITY_TESTER_OWNER VisibilityTesterOwner,
    _In_ PCSCENE Scene,
    _In_ FLOAT Epsilon,
    _Out_ PSPECTRUM_VISIBILITY_TESTER *VisibilityTester
    )
{
    ASSERT(VisibilityTesterOwner != NULL);
    ASSERT(Scene != NULL);
    ASSERT(IsFiniteFloat(Epsilon) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(Epsilon) != FALSE);
    ASSERT(VisibilityTester != NULL);

    SpectrumVisibilityTesterSetSceneAndEpsilon(&VisibilityTesterOwner->VisibilityTester,
                                               Scene,
                                               Epsilon);

    *VisibilityTester = &VisibilityTesterOwner->VisibilityTester;

    return ISTATUS_SUCCESS;
}

SFORCEINLINE
VOID
SpectrumVisibilityTesterOwnerDestroy(
    _In_ _Post_invalid_ PSPECTRUM_VISIBILITY_TESTER_OWNER Tester
    )
{
    ASSERT(Tester != NULL);
    
    SpectrumVisibilityTesterDestroy(&Tester->VisibilityTester);
}

#endif // _SPECTRUM_VISIBILITY_TESTER_OWNER_IRIS_PHYSX_INTERNAL_