/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_visibilitytester.h

Abstract:

    This file contains the definitions for the VISIBILITY_TESTER type.

--*/

#ifndef _VISIBILITY_TESTER_IRIS_SHADING_MODEL_
#define _VISIBILITY_TESTER_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

typedef struct _VISIBILITY_TESTER VISIBILITY_TESTER, *PVISIBILITY_TESTER;
typedef CONST VISIBILITY_TESTER *PCVISIBILITY_TESTER;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISSHADINGMODELAPI
PVISIBILITY_TESTER
VisibilityTesterAllocate(
    _In_ PCSCENE Scene,
    _In_ FLOAT Epsilon
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
VisibilityTesterTestVisibility(
    _In_ PVISIBILITY_TESTER Tester,
    _In_ PCRAY WorldRay,
    _In_ FLOAT DistanceToObject,
    _Out_ PBOOL Visible
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSHADINGMODELAPI
ISTATUS
VisibilityTesterTestVisibilityAnyDistance(
    _In_ PVISIBILITY_TESTER Tester,
    _In_ PCRAY WorldRay,
    _Out_ PBOOL Visible
    );

IRISSHADINGMODELAPI
VOID
VisibilityTesterFree(
    _In_opt_ _Post_invalid_ PVISIBILITY_TESTER Tester
    );

#endif // _RANDOM_IRIS_SHADING_MODEL_