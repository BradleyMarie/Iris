/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_hittester.h

Abstract:

    This file contains the declarations for the 
    HIT_TESTER type.

--*/

#ifndef _HIT_TESTER_IRIS_
#define _HIT_TESTER_IRIS_

#include <iris.h>

//
// Types
//

typedef struct _HIT_TESTER HIT_TESTER, *PHIT_TESTER;
typedef CONST HIT_TESTER *PCHIT_TESTER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
HitTesterTestShape(
    _Inout_ PHIT_TESTER HitTester,
    _In_ PCSHAPE Shape
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
HitTesterTestShapeWithTransform(
    _Inout_ PHIT_TESTER HitTester,
    _In_ PCSHAPE Shape,
    _In_opt_ PCMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
HitTesterTestPremultipliedShapeWithTransform(
    _Inout_ PHIT_TESTER HitTester,
    _In_ PCSHAPE Shape,
    _In_opt_ PCMATRIX ModelToWorld
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
HitTesterGetRay(
    _In_ PHIT_TESTER HitTester,
    _Out_ PRAY Ray
    );

#endif // _HIT_TESTER_IRIS_