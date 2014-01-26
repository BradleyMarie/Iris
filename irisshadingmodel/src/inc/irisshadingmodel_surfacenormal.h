/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_surfacenormal.h

Abstract:

    This file contains the definitions for the SURFACE_NORMAL type.

--*/

#ifndef _SURFACE_NORMAL_IRIS_SHADING_MODEL_
#define _SURFACE_NORMAL_IRIS_SHADING_MODEL_

#include <irisshadingmodel.h>

//
// Types
//

struct _SURFACE_NORMAL {
    BOOL NormalizedWorldNormalValid;
    BOOL NormalizedModelNormalValid;
    BOOL WorldNormalValid;
    BOOL ModelNormalValid;
    VECTOR3 NormalizedWorldNormal;
    VECTOR3 NormalizedModelNormal;
    VECTOR3 WorldNormal;
    VECTOR3 ModelNormal;
    PCMATRIX WorldToModel;
    PCPOINT3 WorldHit;
    PCPOINT3 ModelHit;
    UINT8 PrenormalizedTerm;
};

#endif // _SURFACE_NORMAL_IRIS_SHADING_MODEL_