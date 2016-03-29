/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_hittester.h

Abstract:

    This module implements the PBR_HIT_TESTER public functions.

--*/

#ifndef _IRIS_PHYSX_HIT_TESTER_
#define _IRIS_PHYSX_HIT_TESTER_

#include <iris.h>

//
// Types
//

typedef struct _PBR_HIT_TESTER PBR_HIT_TESTER, *PPBR_HIT_TESTER;
typedef CONST PBR_HIT_TESTER *PCPBR_HIT_TESTER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRHitTesterTestGeometry(
    _Inout_ PPBR_HIT_TESTER PBRHitTester,
    _In_ PCPBR_GEOMETRY PBRGeometry
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRHitTesterTestGeometryWithTransform(
    _Inout_ PPBR_HIT_TESTER PBRHitTester,
    _In_ PCPBR_GEOMETRY PBRGeometry,
    _In_opt_ PCMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PBRHitTesterTestPremultipliedGeometryWithTransform(
    _Inout_ PPBR_HIT_TESTER PBRHitTester,
    _In_ PCPBR_GEOMETRY PBRGeometry,
    _In_opt_ PCMATRIX ModelToWorld
    );

#endif // _IRIS_PHYSX_HIT_TESTER_