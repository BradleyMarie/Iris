/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysx_light.h

Abstract:

    This file contains the definitions for the PBR_LIGHT type.

--*/

#ifndef _PBR_LIGHT_IRIS_PHYSX_INTERNAL_
#define _PBR_LIGHT_IRIS_PHYSX_INTERNAL_

#include <irisphysxp.h>

//
// Prototypes
//

VOID
PBRLightFree(
    _In_ _Post_invalid_ PPBR_LIGHT PBRLight
    );

#endif // _PBR_LIGHT_IRIS_PHYSX_INTERNAL_