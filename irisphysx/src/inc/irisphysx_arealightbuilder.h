/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_arealightbuilder.h

Abstract:

    This file contains the exports from the area light builder file.

--*/

#ifndef _PHYSX_AREA_LIGHT_IRIS_PHYSX_INTERNAL_
#define _PHYSX_AREA_LIGHT_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Types
//

typedef struct _PHYSX_AREA_LIGHT_REFERENCE_COUNT PHYSX_AREA_LIGHT_REFERENCE_COUNT, *PPHYSX_AREA_LIGHT_REFERENCE_COUNT;

//
// Functions
//

VOID
AreaLightReferenceCountRetain(
    _In_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount
    );

VOID
AreaLightReferenceCountRelease(
    _In_ _Post_invalid_ PPHYSX_AREA_LIGHT_REFERENCE_COUNT ReferenceCount
    );

#endif // _PHYSX_AREA_LIGHT_IRIS_PHYSX_INTERNAL_