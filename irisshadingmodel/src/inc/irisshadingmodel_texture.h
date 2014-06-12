/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisshadingmodel_texture.h

Abstract:

    This file contains the internal definitions for the TEXTURE type.

--*/

#ifndef _TEXTURE_IRIS_SHADING_MODEL_INTERNAL_
#define _TEXTURE_IRIS_SHADING_MODEL_INTERNAL_

#include <irisshadingmodelp.h>

//
// Types
//

struct _TEXTURE_SHADER {
    PSHADE_RAY_ROUTINE ShadeRayRoutine;
    PCVOID Context;
    PRAYSHADER NextRayShader;
    UINT8 CurrentDepth;
    FLOAT Distance;
    PCVECTOR3 WorldViewer;
    PCPOINT3 WorldHit;
    PCVECTOR3 ModelViewer;
    PCPOINT3 ModelHit;
    PCMATRIX ModelToWorld;
    PCVOID AdditionalData;
    PSURFACE_NORMAL SurfaceNormal;
    PCOLOR4 Color;
};

//
// Functions
//

SFORCEINLINE
VOID
TextureShaderInitialize(
    _Out_ PTEXTURE_SHADER TextureShader,
    _In_ PSHADE_RAY_ROUTINE ShadeRayRoutine,
    _In_opt_ PCVOID Context,
    _Inout_opt_ PRAYSHADER NextRayShader,
    _In_ UINT8 CurrentDepth,
    _In_ FLOAT Distance,
    _In_ PCVECTOR3 WorldViewer,
    _In_ PCPOINT3 WorldHit,
    _In_ PCVECTOR3 ModelViewer,
    _In_ PCPOINT3 ModelHit,
    _In_opt_ PCMATRIX ModelToWorld,
    _In_ PCVOID AdditionalData,
    _In_ PSURFACE_NORMAL SurfaceNormal,
    _In_ PCOLOR4 Color
    )
{
    ASSERT(TextureShader != NULL);
    ASSERT(Context != NULL);
    ASSERT(IsNormalFloat(Distance));
    ASSERT(IsFiniteFloat(Distance));
    ASSERT(Distance > (FLOAT) 0.0);
    ASSERT(WorldViewer != NULL);
    ASSERT(WorldHit != NULL);
    ASSERT(ModelViewer != NULL);
    ASSERT(ModelHit != NULL);
    ASSERT(Color != NULL);

    TextureShader->ShadeRayRoutine = ShadeRayRoutine;
    TextureShader->Context = Context;
    TextureShader->NextRayShader = NextRayShader;
    TextureShader->CurrentDepth = CurrentDepth;
    TextureShader->Distance = Distance;
    TextureShader->WorldViewer = WorldViewer;
    TextureShader->WorldHit = WorldHit;
    TextureShader->ModelViewer = ModelViewer;
    TextureShader->ModelHit = ModelHit;
    TextureShader->ModelToWorld = ModelToWorld;
    TextureShader->AdditionalData = AdditionalData;
    TextureShader->SurfaceNormal = SurfaceNormal;
    TextureShader->Color = Color;
}

#endif // _TEXTURE_IRIS_SHADING_MODEL_INTERNAL_