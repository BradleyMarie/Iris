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

typedef
_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS 
(*PHIT_TESTER_TEST_GEOMETRY_ROUTINE)(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PHIT_ALLOCATOR HitAllocator,
    _Outptr_result_maybenull_ PHIT_LIST *HitList
    );

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
HitTesterTestGeometry(
    _Inout_ PHIT_TESTER HitTester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID Data
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
HitTesterTestGeometryWithTransform(
    _Inout_ PHIT_TESTER HitTester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID Data,
    _In_opt_ PCMATRIX ModelToWorld,
    _In_ BOOL Premultiplied
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISAPI
ISTATUS
HitTesterTestPremultipliedGeometryWithTransform(
    _Inout_ PHIT_TESTER HitTester,
    _In_ PHIT_TESTER_TEST_GEOMETRY_ROUTINE TestGeometryRoutine,
    _In_opt_ PCVOID Data,
    _In_opt_ PCMATRIX ModelToWorld
    );

#endif // _HIT_TESTER_IRIS_