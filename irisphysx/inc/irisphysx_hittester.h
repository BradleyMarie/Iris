/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_hittester.h

Abstract:

    This module implements the PHYSX_HIT_TESTER public functions.

--*/

#ifndef _IRIS_PHYSX_HIT_TESTER_
#define _IRIS_PHYSX_HIT_TESTER_

#include <iris.h>

//
// Types
//

typedef struct _PHYSX_HIT_TESTER PHYSX_HIT_TESTER, *PPHYSX_HIT_TESTER;
typedef CONST PHYSX_HIT_TESTER *PCPHYSX_HIT_TESTER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxHitTesterTestGeometry(
    _Inout_ PPHYSX_HIT_TESTER HitTester,
    _In_ PCPHYSX_GEOMETRY Geometry
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxHitTesterTestGeometryWithTransform(
    _Inout_ PPHYSX_HIT_TESTER HitTester,
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_opt_ PCMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
PhysxHitTesterTestPremultipliedGeometryWithTransform(
    _Inout_ PPHYSX_HIT_TESTER HitTester,
    _In_ PCPHYSX_GEOMETRY Geometry,
    _In_opt_ PCMATRIX ModelToWorld
    );

#endif // _IRIS_PHYSX_HIT_TESTER_
