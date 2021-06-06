/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    trowbridge_reitz.h

Abstract:

    Initializes a Trowbridge-Reitz microfacet distribution.

--*/

#ifndef _IRIS_PHYSX_TOOLKIT_BSDFS_MICROFACET_DISTRIBUTIONS_TROWBRIDGE_REITZ_
#define _IRIS_PHYSX_TOOLKIT_BSDFS_MICROFACET_DISTRIBUTIONS_TROWBRIDGE_REITZ_

#include "iris_physx_toolkit/bsdfs/microfacet_distributions/microfacet_distribution.h"

//
// Functions
//

ISTATUS
TrowbridgeReitzCreate(
    _In_ float_t alpha_x,
    _In_ float_t alpha_y,
    _Out_ PMICROFACET_DISTRIBUTION distribution
    );

static
inline
float_t
TrowbridgeReitzRoughnessToAlpha(
    _In_ float_t roughness
    )
{
    roughness = IMax(roughness, (float_t)0.001);

    float_t x = log(roughness);
    float_t x0 = (float_t)1.62142;
    float_t x1 = (float_t)0.819955 * x;
    float_t x2 = (float_t)0.1734 * x * x;
    float_t x3 = (float_t)0.0171201 * x * x * x;
    float_t x4 = (float_t)0.000640711 * x * x * x * x;

    return x0 + x1 + x2 + x3 + x4;
}

#endif // _IRIS_PHYSX_TOOLKIT_BSDFS_MICROFACET_DISTRIBUTIONS_TROWBRIDGE_REITZ_