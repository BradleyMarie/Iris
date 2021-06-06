/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    fresnel.h

Abstract:

    Initializes a dielectric fresnel function.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_BSDFS_FRESNELS_DIELECTRIC_
#define _IRIS_PHYSX_TOOLKIT_BSDFS_FRESNELS_DIELECTRIC_

#include "iris_physx_toolkit/bsdfs/fresnels/fresnel.h"

//
// Functions
//

ISTATUS
DielectricFresnelCreate(
    _In_ float_t eta_i,
    _In_ float_t eta_t,
    _Out_ PFRESNEL function
    );

#endif // _IRIS_PHYSX_TOOLKIT_BSDFS_FRESNELS_DIELECTRIC_