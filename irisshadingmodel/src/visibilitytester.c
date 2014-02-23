/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    visibilitytester.c

Abstract:

    This file contains the function definitions for the VISIBILITY_TESTER type.

--*/

#include <irisshadingmodelp.h>

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

IRISSHADINGMODELAPI
VOID
VisibilityTesterFree(
    _Pre_maybenull_ _Post_invalid_ PVISIBILITY_TESTER Tester
    );