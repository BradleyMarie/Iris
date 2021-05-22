/*++

Copyright (c) 2021 Brad Weinberger

Module Name:

    bsdf_sample_type.h

Abstract:

    An enum representing the type of path sampled from a BSDF.

--*/

#ifndef _IRIS_PHYSX_BSDF_SAMPLE_TYPE_
#define _IRIS_PHYSX_BSDF_SAMPLE_TYPE_

#include <stdbool.h>

//
// Types
//

typedef enum _BSDF_SAMPLE_TYPE {
    BSDF_SAMPLE_TYPE_REFLECTION_DIFFUSE_ONLY        = 0x00,
    BSDF_SAMPLE_TYPE_REFLECTION_CONTIANS_SPECULAR   = 0x02,
    BSDF_SAMPLE_TYPE_TRANSMISSION_DIFFUSE_ONLY      = 0x01,
    BSDF_SAMPLE_TYPE_TRANSMISSION_CONTAINS_SPECULAR = 0x03,
} BSDF_SAMPLE_TYPE, *PBSDF_SAMPLE_TYPE;

typedef const BSDF_SAMPLE_TYPE *PCBSDF_SAMPLE_TYPE;

//
// Functions
//

static
inline
bool
BsdfSampleIsTransmission(
    _In_ BSDF_SAMPLE_TYPE type
    )
{
    return type & 0x01;
}

static
inline
bool
BsdfSampleContainsSpecular(
    _In_ BSDF_SAMPLE_TYPE type
    )
{
    return type & 0x02;
}

#endif // _IRIS_PHYSX_BSDF_SAMPLE_TYPE_
