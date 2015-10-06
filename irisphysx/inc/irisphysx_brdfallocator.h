/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_brdfalloocator.h

Abstract:

    This file contains the definitions for the BRDF_ALLOCATOR type.

--*/

#ifndef _BRDF_ALLOCATOR_IRIS_PHYSX_
#define _BRDF_ALLOCATOR_IRIS_PHYSX_

#include <irisphysx.h>

//
// Types
//

typedef struct _BRDF_ALLOCATOR BRDF_ALLOCATOR, *PBRDF_ALLOCATOR;
typedef CONST BRDF_ALLOCATOR *PCBRDF_ALLOCATOR;

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISPHYSXAPI
PBRDF_ALLOCATOR
BrdfAllocatorCreate(
    VOID
    );

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISPHYSXAPI
ISTATUS
BrdfAllocatorAllocate(
    _Inout_ PBRDF_ALLOCATOR BrdfAllocator,
    _In_ PBRDF_SAMPLE SampleRoutine,
    _In_ PBRDF_COMPUTE_REFLECTANCE ComputeReflectanceRoutine,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment,
    _Out_ PCBRDF *Brdf
    );

IRISPHYSXAPI
VOID
BrdfAllocatorFreeAll(
    _Inout_opt_ PBRDF_ALLOCATOR BrdfAllocator
    );

IRISPHYSXAPI
VOID
BrdfAllocatorDestroy(
    _Pre_maybenull_ _Post_invalid_ PBRDF_ALLOCATOR BrdfAllocator
    );

#endif // _BRDF_ALLOCATOR_IRIS_PHYSX_