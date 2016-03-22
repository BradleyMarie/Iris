/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_phong.h

Abstract:

    This file contains the definitions for the PHONG shading types.

--*/

#ifndef _PHONG_SHADER_IRIS_TOOLKIT_
#define _PHONG_SHADER_IRIS_TOOLKIT_

#include <iristoolkit.h>

//
// Types
//

typedef
ISTATUS
(*PPHONG_LIGHT_SHADE_ROUTINE)(
    _In_ PCVOID Context,
    _In_ COLOR3 Ambient,
    _In_ COLOR3 Diffuse,
    _In_ COLOR3 Specular,
    _In_ FLOAT Shininess,
    _In_ POINT3 WorldHitPoint,
    _In_ VECTOR3 WorldViewer,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 LightColor
    );

typedef struct _PHONG_LIGHT_VTABLE {
    PPHONG_LIGHT_SHADE_ROUTINE ShadeRoutine;
    PFREE_ROUTINE FreeRoutine;
} PHONG_LIGHT_VTABLE, *PPHONG_LIGHT_VTABLE;

typedef CONST PHONG_LIGHT_VTABLE *PCLIGHT_SHADER_VTABLE;

typedef struct _PHONG_LIGHT {
    PCLIGHT_SHADER_VTABLE PhongLightVTable;
} PHONG_LIGHT, *PPHONG_LIGHT;

typedef CONST PHONG_LIGHT *PCPHONG_LIGHT;

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PDIRECT_SHADER
PhongDirectShaderAllocate(
    _In_reads_(NumberOfLights) PCPHONG_LIGHT CONST *Lights,
    _In_ SIZE_T NumberOfLights,
    _In_ PLIGHT_SELECTION_ROUTINE LightSelectionRoutine,
    _In_ COLOR3 Ambient,
    _In_ COLOR3 Diffuse,
    _In_ COLOR3 Specular,
    _In_ FLOAT Shininess
    );

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PPHONG_LIGHT
DirectionalPhongLightAllocate(
    _In_ VECTOR3 WorldDirectionToLight,
    _In_ COLOR3 Ambient,
    _In_ COLOR3 Diffuse,
    _In_ COLOR3 Specular,
    _In_ BOOL CastsShadows
    );

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PPHONG_LIGHT
PointPhongLightAllocate(
    _In_ POINT3 WorldLocation,
    _In_ COLOR3 Ambient,
    _In_ COLOR3 Diffuse,
    _In_ COLOR3 Specular,
    _In_ BOOL CastsShadows
    );

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PPHONG_LIGHT
AttenuatedPointPhongLightAllocate(
    _In_ POINT3 WorldLocation,
    _In_ COLOR3 Ambient,
    _In_ COLOR3 Diffuse,
    _In_ COLOR3 Specular,
    _In_ BOOL CastsShadows,
    _In_ FLOAT ConstantAttenuation,
    _In_ FLOAT LinearAttenuation,
    _In_ FLOAT QuadraticAttenuation
    );

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PPHONG_LIGHT
PointPhongSpotLightAllocate(
    _In_ POINT3 WorldLocation,
    _In_ COLOR3 Ambient,
    _In_ COLOR3 Diffuse,
    _In_ COLOR3 Specular,
    _In_ BOOL CastsShadows,
    _In_ VECTOR3 SpotLightDirection,
    _In_ FLOAT SpotLightExponent,
    _In_ FLOAT SpotLightCutoff
    );

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PPHONG_LIGHT
AttenuatedPhongSpotLightAllocate(
    _In_ POINT3 WorldLocation,
    _In_ COLOR3 Ambient,
    _In_ COLOR3 Diffuse,
    _In_ COLOR3 Specular,
    _In_ BOOL CastsShadows,
    _In_ FLOAT ConstantAttenuation,
    _In_ FLOAT LinearAttenuation,
    _In_ FLOAT QuadraticAttenuation,
    _In_ VECTOR3 SpotLightDirection,
    _In_ FLOAT SpotLightExponent,
    _In_ FLOAT SpotLightCutoff
    );

//
// Functions
//

SFORCEINLINE
VOID
PhongLightFree(
    _In_opt_ _Post_invalid_ PPHONG_LIGHT PhongLight
    )
{
    if (PhongLight == NULL)
    {
        return;
    }

    PhongLight->PhongLightVTable->FreeRoutine(PhongLight);
}

#endif // _PHONG_SHADER_IRIS_TOOLKIT_