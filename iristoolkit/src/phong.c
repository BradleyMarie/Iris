/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    phong.c

Abstract:

    This file contains the function definitions for the PHONG types.

--*/

#include <iristoolkitp.h>

//
// Types
//

typedef struct _PHONG_SHADER {
    DIRECT_SHADER DirectShaderHeader;
    PLIGHT_SELECTION_ROUTINE LightSelectionRoutine;
    _Field_size_(NumberOfLights) PCPHONG_LIGHT CONST *Lights;
    SIZE_T NumberOfLights;
    COLOR3 Ambient;
    COLOR3 Diffuse;
    COLOR3 Specular;
    FLOAT Shininess;
} PHONG_SHADER, *PPHONG_SHADER;

typedef CONST PHONG_SHADER *PCPHONG_SHADER;

typedef struct _DIRECTIONAL_PHONG_LIGHT {
    PHONG_LIGHT PhongLightHeader;
    VECTOR3 WorldDirectionToLight;
    COLOR3 Ambient;
    COLOR3 Diffuse;
    COLOR3 Specular;
    BOOL CastsShadows;
} DIRECTIONAL_PHONG_LIGHT, *PDIRECTIONAL_PHONG_LIGHT;

typedef CONST DIRECTIONAL_PHONG_LIGHT *PCDIRECTIONAL_PHONG_LIGHT;

typedef struct _POINT_PHONG_LIGHT {
    PHONG_LIGHT PhongLightHeader;
    POINT3 WorldLocation;
    COLOR3 Ambient;
    COLOR3 Diffuse;
    COLOR3 Specular;
    BOOL CastsShadows;
} POINT_PHONG_LIGHT, *PPOINT_PHONG_LIGHT;

typedef CONST POINT_PHONG_LIGHT *PCPOINT_PHONG_LIGHT;

typedef struct _ATTENUATED_POINT_PHONG_LIGHT {
    PHONG_LIGHT PhongLightHeader;
    POINT3 WorldLocation;
    COLOR3 Ambient;
    COLOR3 Diffuse;
    COLOR3 Specular;
    BOOL CastsShadows;
    FLOAT ConstantAttenuation;
    FLOAT LinearAttenuation;
    FLOAT QuadraticAttenuation;
} ATTENUATED_POINT_PHONG_LIGHT, *PATTENUATED_POINT_PHONG_LIGHT;

typedef CONST ATTENUATED_POINT_PHONG_LIGHT *PCATTENUATED_POINT_PHONG_LIGHT;

typedef struct _POINT_PHONG_SPOT_LIGHT {
    PHONG_LIGHT PhongLightHeader;
    POINT3 WorldLocation;
    COLOR3 Ambient;
    COLOR3 Diffuse;
    COLOR3 Specular;
    BOOL CastsShadows;
    VECTOR3 WorldSpotLightDirection;
    FLOAT SpotLightExponent;
    FLOAT SpotLightCutoff;
} POINT_PHONG_SPOT_LIGHT, *PPOINT_PHONG_SPOT_LIGHT;

typedef CONST POINT_PHONG_SPOT_LIGHT *PCPOINT_PHONG_SPOT_LIGHT;

typedef struct _ATTENUATED_POINT_PHONG_SPOT_LIGHT {
    PHONG_LIGHT PhongLightHeader;
    POINT3 WorldLocation;
    COLOR3 Ambient;
    COLOR3 Diffuse;
    COLOR3 Specular;
    BOOL CastsShadows;
    FLOAT ConstantAttenuation;
    FLOAT LinearAttenuation;
    FLOAT QuadraticAttenuation;
    VECTOR3 WorldSpotLightDirection;
    FLOAT SpotLightExponent;
    FLOAT SpotLightCutoff;
} ATTENUATED_POINT_PHONG_SPOT_LIGHT, *PATTENUATED_POINT_PHONG_SPOT_LIGHT;

typedef CONST ATTENUATED_POINT_PHONG_SPOT_LIGHT *PCATTENUATED_POINT_PHONG_SPOT_LIGHT;

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhongDirectShaderShadeLight(
    _In_ PCVOID Context,
    _In_ PCVOID Light,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _In_ PCVECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 Direct
    )
{
    PCPHONG_SHADER PhongShader;
    PCPHONG_LIGHT PhongLight;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(Light != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(WorldViewer != NULL);
    ASSERT(ModelViewer != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(Direct != NULL);

    PhongShader = (PCPHONG_SHADER) Context;
    PhongLight = (PCPHONG_LIGHT) Light;

    Status = PhongLight->PhongLightVTable->ShadeRoutine(Light,
                                                        &PhongShader->Ambient,
                                                        &PhongShader->Diffuse,
                                                        &PhongShader->Specular,
                                                        PhongShader->Shininess,
                                                        WorldHitPoint,
                                                        WorldViewer,
                                                        SurfaceNormal,
                                                        Rng,
                                                        VisibilityTester,
                                                        Direct);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PhongDirectShaderShade(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _In_ PCVECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 Direct
    )
{
    PCPHONG_SHADER PhongShader;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(WorldViewer != NULL);
    ASSERT(ModelViewer != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(Direct != NULL);

    PhongShader = (PCPHONG_SHADER) Context;

    Status = PhongShader->LightSelectionRoutine(Context,
                                                PhongShader->Lights,
                                                PhongShader->NumberOfLights,
                                                PhongDirectShaderShadeLight,
                                                WorldHitPoint,
                                                ModelHitPoint,
                                                WorldViewer,
                                                ModelViewer,
                                                AdditionalData,
                                                SurfaceNormal,
                                                Rng,
                                                VisibilityTester,
                                                Direct);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
DirectionalPhongLightShade(
    _In_ PCVOID Context,
    _In_ PCCOLOR3 Ambient,
    _In_ PCCOLOR3 Diffuse,
    _In_ PCCOLOR3 Specular,
    _In_ FLOAT Shininess,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 LightColor
    )
{
    PCDIRECTIONAL_PHONG_LIGHT DirectionalPhongLight;
    VECTOR3 NormalizedWorldSurfaceNormal;
    VECTOR3 ViewerToLightHalfAngle;
    FLOAT SpecularCoefficient;
    FLOAT DiffuseCoefficient;
    COLOR3 ReflectedSpecular;
    COLOR3 ReflectedDiffuse;
    VECTOR3 NegatedViewer;
    BOOL LightVisible;
    RAY RayToLight;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(Ambient != NULL);
    ASSERT(Diffuse != NULL);
    ASSERT(Specular != NULL);
    ASSERT(IsNormalFloat(Shininess) != FALSE);
    ASSERT(IsFiniteFloat(Shininess) != FALSE);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(WorldViewer != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(LightColor != NULL);

    DirectionalPhongLight = (PCDIRECTIONAL_PHONG_LIGHT) Context;

    Color3ScaleByColor(Ambient,
                       &DirectionalPhongLight->Ambient,
                       LightColor);

    if (DirectionalPhongLight->CastsShadows != FALSE)
    {
        RayInitialize(&RayToLight, 
                      WorldHitPoint,
                      &DirectionalPhongLight->WorldDirectionToLight);

        Status = VisibilityTesterTestVisibilityAnyDistance(VisibilityTester,
                                                           &RayToLight,
                                                           &LightVisible);

        if (Status != ISTATUS_SUCCESS ||
            LightVisible == FALSE)
        {
            return Status;
        }
    }

    Status = SurfaceNormalGetNormalizedWorldNormal(SurfaceNormal,
                                                   &NormalizedWorldSurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    DiffuseCoefficient = VectorDotProduct(&DirectionalPhongLight->WorldDirectionToLight,
                                          &NormalizedWorldSurfaceNormal);

    if (DiffuseCoefficient > (FLOAT) 0.0)
    {
        Color3ScaleByColor(&DirectionalPhongLight->Diffuse,
                           Diffuse,
                           &ReflectedDiffuse);

        Color3AddScaled(LightColor,
                        &ReflectedDiffuse,
                        DiffuseCoefficient,
                        LightColor);
    }

    VectorNegate(WorldViewer, &NegatedViewer);

    //
    // WorldViewer and WorldDirectionToLight should already be normalized.
    //

    VectorHalfAngle(&NegatedViewer,
                    &DirectionalPhongLight->WorldDirectionToLight,
                    &ViewerToLightHalfAngle);

    SpecularCoefficient = VectorDotProduct(&ViewerToLightHalfAngle,
                                           &NormalizedWorldSurfaceNormal);

    if (SpecularCoefficient > (FLOAT) 0.0)
    {
        SpecularCoefficient = PowFloat(SpecularCoefficient, Shininess);

        Color3ScaleByColor(&DirectionalPhongLight->Specular,
                           Specular,
                           &ReflectedSpecular);

        Color3AddScaled(LightColor,
                        &ReflectedSpecular,
                        SpecularCoefficient,
                        LightColor);
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PointPhongLightShade(
    _In_ PCVOID Context,
    _In_ PCCOLOR3 Ambient,
    _In_ PCCOLOR3 Diffuse,
    _In_ PCCOLOR3 Specular,
    _In_ FLOAT Shininess,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 LightColor
    )
{
    PCPOINT_PHONG_LIGHT PointPhongLight;
    VECTOR3 NormalizedWorldSurfaceNormal;
    VECTOR3 ViewerToLightHalfAngle;
    VECTOR3 NormalizedWorldToLight;
    FLOAT SpecularCoefficient;
    FLOAT DiffuseCoefficient;
    COLOR3 ReflectedSpecular;
    COLOR3 ReflectedDiffuse;
    VECTOR3 NegatedViewer;
    FLOAT DistanceToLight;
    BOOL LightVisible;
    RAY RayToLight;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(Ambient != NULL);
    ASSERT(Diffuse != NULL);
    ASSERT(Specular != NULL);
    ASSERT(IsNormalFloat(Shininess) != FALSE);
    ASSERT(IsFiniteFloat(Shininess) != FALSE);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(WorldViewer != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(LightColor != NULL);

    PointPhongLight = (PCPOINT_PHONG_LIGHT) Context;

    Color3ScaleByColor(Ambient,
                       &PointPhongLight->Ambient,
                       LightColor);

    if (PointPhongLight->CastsShadows != FALSE)
    {
        PointSubtract(&PointPhongLight->WorldLocation,
                      WorldHitPoint,
                      &NormalizedWorldToLight);

        VectorNormalizeWithLength(&NormalizedWorldToLight,
                                  &DistanceToLight,
                                  &NormalizedWorldToLight);

        RayInitialize(&RayToLight, 
                      WorldHitPoint,
                      &NormalizedWorldToLight);

        Status = VisibilityTesterTestVisibility(VisibilityTester,
                                                &RayToLight,
                                                DistanceToLight,
                                                &LightVisible);

        if (Status != ISTATUS_SUCCESS ||
            LightVisible == FALSE)
        {
            return Status;
        }
    }

    Status = SurfaceNormalGetNormalizedWorldNormal(SurfaceNormal,
                                                   &NormalizedWorldSurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    DiffuseCoefficient = VectorDotProduct(&NormalizedWorldToLight,
                                          &NormalizedWorldSurfaceNormal);

    if (DiffuseCoefficient > (FLOAT) 0.0)
    {
        Color3ScaleByColor(&PointPhongLight->Diffuse,
                           Diffuse,
                           &ReflectedDiffuse);

        Color3AddScaled(LightColor,
                        &ReflectedDiffuse,
                        DiffuseCoefficient,
                        LightColor);
    }

    VectorNegate(WorldViewer, &NegatedViewer);

    //
    // WorldViewer and WorldDirectionToLight should already be normalized.
    //

    VectorHalfAngle(&NegatedViewer,
                    &NormalizedWorldToLight,
                    &ViewerToLightHalfAngle);

    SpecularCoefficient = VectorDotProduct(&ViewerToLightHalfAngle,
                                           &NormalizedWorldSurfaceNormal);

    if (SpecularCoefficient > (FLOAT) 0.0)
    {
        SpecularCoefficient = PowFloat(SpecularCoefficient, Shininess);

        Color3ScaleByColor(&PointPhongLight->Specular,
                           Specular,
                           &ReflectedSpecular);

        Color3AddScaled(LightColor,
                        &ReflectedSpecular,
                        SpecularCoefficient,
                        LightColor);
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
AttenuatedPointPhongLightShade(
    _In_ PCVOID Context,
    _In_ PCCOLOR3 Ambient,
    _In_ PCCOLOR3 Diffuse,
    _In_ PCCOLOR3 Specular,
    _In_ FLOAT Shininess,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 LightColor
    )
{
    PCATTENUATED_POINT_PHONG_LIGHT AttenuatedPointPhongLight;
    VECTOR3 NormalizedWorldSurfaceNormal;
    VECTOR3 ViewerToLightHalfAngle;
    VECTOR3 NormalizedWorldToLight;
    FLOAT SpecularCoefficient;
    FLOAT DiffuseCoefficient;
    COLOR3 ReflectedSpecular;
    COLOR3 ReflectedDiffuse;
    VECTOR3 NegatedViewer;
    FLOAT DistanceToLight;
    FLOAT Attenuation;
    BOOL LightVisible;
    RAY RayToLight;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(Ambient != NULL);
    ASSERT(Diffuse != NULL);
    ASSERT(Specular != NULL);
    ASSERT(IsNormalFloat(Shininess) != FALSE);
    ASSERT(IsFiniteFloat(Shininess) != FALSE);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(WorldViewer != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(LightColor != NULL);

    AttenuatedPointPhongLight = (PCATTENUATED_POINT_PHONG_LIGHT) Context;

    Color3ScaleByColor(Ambient,
                       &AttenuatedPointPhongLight->Ambient,
                       LightColor);

    if (AttenuatedPointPhongLight->CastsShadows != FALSE)
    {
        PointSubtract(&AttenuatedPointPhongLight->WorldLocation,
                      WorldHitPoint,
                      &NormalizedWorldToLight);

        VectorNormalizeWithLength(&NormalizedWorldToLight,
                                  &DistanceToLight,
                                  &NormalizedWorldToLight);

        RayInitialize(&RayToLight, 
                      WorldHitPoint,
                      &NormalizedWorldToLight);

        Status = VisibilityTesterTestVisibility(VisibilityTester,
                                                &RayToLight,
                                                DistanceToLight,
                                                &LightVisible);

        if (Status != ISTATUS_SUCCESS ||
            LightVisible == FALSE)
        {
            return Status;
        }
    }

    Status = SurfaceNormalGetNormalizedWorldNormal(SurfaceNormal,
                                                   &NormalizedWorldSurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    DiffuseCoefficient = VectorDotProduct(&NormalizedWorldToLight,
                                          &NormalizedWorldSurfaceNormal);

    if (DiffuseCoefficient > (FLOAT) 0.0)
    {
        Color3ScaleByColor(&AttenuatedPointPhongLight->Diffuse,
                           Diffuse,
                           &ReflectedDiffuse);

        Color3AddScaled(LightColor,
                        &ReflectedDiffuse,
                        DiffuseCoefficient,
                        LightColor);
    }

    VectorNegate(WorldViewer, &NegatedViewer);

    //
    // WorldViewer and WorldDirectionToLight should already be normalized.
    //

    VectorHalfAngle(&NegatedViewer,
                    &NormalizedWorldToLight,
                    &ViewerToLightHalfAngle);

    SpecularCoefficient = VectorDotProduct(&ViewerToLightHalfAngle,
                                           &NormalizedWorldSurfaceNormal);

    if (SpecularCoefficient > (FLOAT) 0.0)
    {
        SpecularCoefficient = PowFloat(SpecularCoefficient, Shininess);

        Color3ScaleByColor(&AttenuatedPointPhongLight->Specular,
                           Specular,
                           &ReflectedSpecular);

        Color3AddScaled(LightColor,
                        &ReflectedSpecular,
                        SpecularCoefficient,
                        LightColor);
    }

    Attenuation = AttenuatedPointPhongLight->ConstantAttenuation + 
                  AttenuatedPointPhongLight->LinearAttenuation * DistanceToLight +
                  AttenuatedPointPhongLight->QuadraticAttenuation * DistanceToLight * DistanceToLight;

    Attenuation = (FLOAT) 1.0 / Attenuation;

    Color3ScaleByScalar(LightColor, Attenuation, LightColor);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
PointPhongSpotLightShade(
    _In_ PCVOID Context,
    _In_ PCCOLOR3 Ambient,
    _In_ PCCOLOR3 Diffuse,
    _In_ PCCOLOR3 Specular,
    _In_ FLOAT Shininess,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 LightColor
    )
{
    PCPOINT_PHONG_SPOT_LIGHT PointPhongSpotLight;
    VECTOR3 NormalizedWorldSurfaceNormal;
    VECTOR3 NormalizedWorldFromLight;
    VECTOR3 ViewerToLightHalfAngle;
    VECTOR3 NormalizedWorldToLight;
    FLOAT SpotlightCoefficient;
    FLOAT SpecularCoefficient;
    FLOAT DiffuseCoefficient;
    COLOR3 ReflectedSpecular;
    COLOR3 ReflectedDiffuse;
    VECTOR3 NegatedViewer;
    FLOAT DistanceToLight;
    BOOL LightVisible;
    RAY RayToLight;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(Ambient != NULL);
    ASSERT(Diffuse != NULL);
    ASSERT(Specular != NULL);
    ASSERT(IsNormalFloat(Shininess) != FALSE);
    ASSERT(IsFiniteFloat(Shininess) != FALSE);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(WorldViewer != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(LightColor != NULL);

    PointPhongSpotLight = (PCPOINT_PHONG_SPOT_LIGHT) Context;

    if (PointPhongSpotLight->SpotLightCutoff < (FLOAT) 1.0)
    {
        VectorNegate(&NormalizedWorldToLight,
                     &NormalizedWorldFromLight);

        //
        // WorldSpotLightDirection should already be normalized.
        //

        SpotlightCoefficient = VectorDotProduct(&PointPhongSpotLight->WorldSpotLightDirection,
                                                &NormalizedWorldFromLight);

        if (SpotlightCoefficient >= PointPhongSpotLight->SpotLightCutoff)
        {
            SpotlightCoefficient = PowFloat(SpotlightCoefficient,
                                            PointPhongSpotLight->SpotLightExponent);
        }
        else
        {
            Color3InitializeBlack(LightColor);
            return ISTATUS_SUCCESS;
        }
    }
    else
    {
        SpotlightCoefficient = (FLOAT) 1.0;
    }

    Color3ScaleByColor(Ambient,
                       &PointPhongSpotLight->Ambient,
                       LightColor);

    if (PointPhongSpotLight->CastsShadows != FALSE)
    {
        PointSubtract(&PointPhongSpotLight->WorldLocation,
                      WorldHitPoint,
                      &NormalizedWorldToLight);

        VectorNormalizeWithLength(&NormalizedWorldToLight,
                                  &DistanceToLight,
                                  &NormalizedWorldToLight);

        RayInitialize(&RayToLight, 
                      WorldHitPoint,
                      &NormalizedWorldToLight);

        Status = VisibilityTesterTestVisibility(VisibilityTester,
                                                &RayToLight,
                                                DistanceToLight,
                                                &LightVisible);

        if (Status != ISTATUS_SUCCESS ||
            LightVisible == FALSE)
        {
            return Status;
        }
    }

    Status = SurfaceNormalGetNormalizedWorldNormal(SurfaceNormal,
                                                   &NormalizedWorldSurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    DiffuseCoefficient = VectorDotProduct(&NormalizedWorldToLight,
                                          &NormalizedWorldSurfaceNormal);

    if (DiffuseCoefficient > (FLOAT) 0.0)
    {
        Color3ScaleByColor(&PointPhongSpotLight->Diffuse,
                           Diffuse,
                           &ReflectedDiffuse);

        Color3AddScaled(LightColor,
                        &ReflectedDiffuse,
                        DiffuseCoefficient,
                        LightColor);
    }

    VectorNegate(WorldViewer, &NegatedViewer);

    //
    // WorldViewer and WorldDirectionToLight should already be normalized.
    //

    VectorHalfAngle(&NegatedViewer,
                    &NormalizedWorldToLight,
                    &ViewerToLightHalfAngle);

    SpecularCoefficient = VectorDotProduct(&ViewerToLightHalfAngle,
                                           &NormalizedWorldSurfaceNormal);

    if (SpecularCoefficient > (FLOAT) 0.0)
    {
        SpecularCoefficient = PowFloat(SpecularCoefficient, Shininess);

        Color3ScaleByColor(&PointPhongSpotLight->Specular,
                           Specular,
                           &ReflectedSpecular);

        Color3AddScaled(LightColor,
                        &ReflectedSpecular,
                        SpecularCoefficient,
                        LightColor);
    }

    Color3ScaleByScalar(LightColor, SpotlightCoefficient, LightColor);

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
AttenuatedPointPhongSpotLightShade(
    _In_ PCVOID Context,
    _In_ PCCOLOR3 Ambient,
    _In_ PCCOLOR3 Diffuse,
    _In_ PCCOLOR3 Specular,
    _In_ FLOAT Shininess,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 LightColor
    )
{
    PCATTENUATED_POINT_PHONG_SPOT_LIGHT AttenuatedPointPhongSpotLight;
    VECTOR3 NormalizedWorldSurfaceNormal;
    VECTOR3 NormalizedWorldFromLight;
    VECTOR3 ViewerToLightHalfAngle;
    VECTOR3 NormalizedWorldToLight;
    FLOAT SpotlightCoefficient;
    FLOAT SpecularCoefficient;
    FLOAT DiffuseCoefficient;
    COLOR3 ReflectedSpecular;
    COLOR3 ReflectedDiffuse;
    VECTOR3 NegatedViewer;
    FLOAT DistanceToLight;
    FLOAT Attenuation;
    BOOL LightVisible;
    RAY RayToLight;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(Ambient != NULL);
    ASSERT(Diffuse != NULL);
    ASSERT(Specular != NULL);
    ASSERT(IsNormalFloat(Shininess) != FALSE);
    ASSERT(IsFiniteFloat(Shininess) != FALSE);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(WorldViewer != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(LightColor != NULL);

    AttenuatedPointPhongSpotLight = (PCATTENUATED_POINT_PHONG_SPOT_LIGHT) Context;

    if (AttenuatedPointPhongSpotLight->SpotLightCutoff < (FLOAT) 1.0)
    {
        VectorNegate(&NormalizedWorldToLight,
                     &NormalizedWorldFromLight);

        //
        // WorldSpotLightDirection should already be normalized.
        //

        SpotlightCoefficient = VectorDotProduct(&AttenuatedPointPhongSpotLight->WorldSpotLightDirection,
                                                &NormalizedWorldFromLight);

        if (SpotlightCoefficient >= AttenuatedPointPhongSpotLight->SpotLightCutoff)
        {
            SpotlightCoefficient = PowFloat(SpotlightCoefficient,
                                            AttenuatedPointPhongSpotLight->SpotLightExponent);
        }
        else
        {
            Color3InitializeBlack(LightColor);
            return ISTATUS_SUCCESS;
        }
    }
    else
    {
        SpotlightCoefficient = (FLOAT) 1.0;
    }

    Color3ScaleByColor(Ambient,
                       &AttenuatedPointPhongSpotLight->Ambient, 
                       LightColor);

    if (AttenuatedPointPhongSpotLight->CastsShadows != FALSE)
    {
        PointSubtract(&AttenuatedPointPhongSpotLight->WorldLocation,
                      WorldHitPoint,
                      &NormalizedWorldToLight);

        VectorNormalizeWithLength(&NormalizedWorldToLight,
                                  &DistanceToLight,
                                  &NormalizedWorldToLight);

        RayInitialize(&RayToLight, 
                      WorldHitPoint,
                      &NormalizedWorldToLight);

        Status = VisibilityTesterTestVisibility(VisibilityTester,
                                                &RayToLight,
                                                DistanceToLight,
                                                &LightVisible);

        if (Status != ISTATUS_SUCCESS ||
            LightVisible == FALSE)
        {
            return Status;
        }
    }

    Status = SurfaceNormalGetNormalizedWorldNormal(SurfaceNormal,
                                                   &NormalizedWorldSurfaceNormal);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    DiffuseCoefficient = VectorDotProduct(&NormalizedWorldToLight,
                                          &NormalizedWorldSurfaceNormal);

    if (DiffuseCoefficient > (FLOAT) 0.0)
    {
        Color3ScaleByColor(&AttenuatedPointPhongSpotLight->Diffuse,
                           Diffuse,
                           &ReflectedDiffuse);

        Color3AddScaled(LightColor,
                        &ReflectedDiffuse,
                        DiffuseCoefficient,
                        LightColor);
    }

    VectorNegate(WorldViewer, &NegatedViewer);

    //
    // WorldViewer and WorldDirectionToLight should already be normalized.
    //

    VectorHalfAngle(&NegatedViewer,
                    &NormalizedWorldToLight,
                    &ViewerToLightHalfAngle);

    SpecularCoefficient = VectorDotProduct(&ViewerToLightHalfAngle,
                                           &NormalizedWorldSurfaceNormal);

    if (SpecularCoefficient > (FLOAT) 0.0)
    {
        SpecularCoefficient = PowFloat(SpecularCoefficient, Shininess);

        Color3ScaleByColor(&AttenuatedPointPhongSpotLight->Specular,
                           Specular,
                           &ReflectedSpecular);

        Color3AddScaled(LightColor,
                        &ReflectedSpecular,
                        SpecularCoefficient,
                        LightColor);
    }

    Attenuation = AttenuatedPointPhongSpotLight->ConstantAttenuation + 
                  AttenuatedPointPhongSpotLight->LinearAttenuation * DistanceToLight +
                  AttenuatedPointPhongSpotLight->QuadraticAttenuation * DistanceToLight * DistanceToLight;

    Attenuation = (FLOAT) 1.0 / Attenuation;

    Color3ScaleByScalar(LightColor, Attenuation * SpotlightCoefficient, LightColor);

    return ISTATUS_SUCCESS;
}

//
// Static Variables
//

CONST STATIC DIRECT_SHADER_VTABLE PhongShaderVTable = {
    PhongDirectShaderShade,
    free
};

CONST STATIC PHONG_LIGHT_VTABLE DirectionalPhongLightVTable = {
    DirectionalPhongLightShade,
    free
};

CONST STATIC PHONG_LIGHT_VTABLE PointPhongLightVTable = {
    PointPhongLightShade,
    free
};

CONST STATIC PHONG_LIGHT_VTABLE AttenuatedPointPhongLightVTable = {
    AttenuatedPointPhongLightShade,
    free
};

CONST STATIC PHONG_LIGHT_VTABLE PointPhongSpotLightVTable = {
    PointPhongSpotLightShade,
    free
};

CONST STATIC PHONG_LIGHT_VTABLE AttenuatedPointPhongSpotLightVTable = {
    AttenuatedPointPhongSpotLightShade,
    free
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PDIRECT_SHADER
PhongDirectShaderAllocate(
    _In_reads_(NumberOfLights) PCPHONG_LIGHT CONST *Lights,
    _In_ SIZE_T NumberOfLights,
    _In_ PLIGHT_SELECTION_ROUTINE LightSelectionRoutine,
    _In_ PCOLOR3 Ambient,
    _In_ PCOLOR3 Diffuse,
    _In_ PCOLOR3 Specular,
    _In_ FLOAT Shininess
    )
{
    PPHONG_SHADER PhongShader;

    if (Lights == NULL ||
        NumberOfLights == 0 ||
        LightSelectionRoutine == NULL ||
        Ambient == NULL ||
        Diffuse == NULL ||
        Specular == NULL ||
        IsFiniteFloat(Shininess) == FALSE ||
        IsNormalFloat(Shininess) == FALSE)
    {
        return NULL;
    }

    PhongShader = (PPHONG_SHADER) malloc(sizeof(PHONG_SHADER));

    PhongShader->DirectShaderHeader.DirectShaderVTable = &PhongShaderVTable;
    PhongShader->LightSelectionRoutine = LightSelectionRoutine;
    PhongShader->Lights = Lights;
    PhongShader->NumberOfLights = NumberOfLights;
    PhongShader->Ambient = *Ambient;
    PhongShader->Diffuse = *Diffuse;
    PhongShader->Specular = *Specular;
    PhongShader->Shininess = Shininess;

    return (PDIRECT_SHADER) PhongShader;
}

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PPHONG_LIGHT
DirectionalPhongLightAllocate(
    _In_ PVECTOR3 WorldDirectionToLight,
    _In_ PCOLOR3 Ambient,
    _In_ PCOLOR3 Diffuse,
    _In_ PCOLOR3 Specular,
    _In_ BOOL CastsShadows
    )
{
    PDIRECTIONAL_PHONG_LIGHT PhongLight;

    if (WorldDirectionToLight == NULL ||
        Ambient == NULL ||
        Diffuse == NULL ||
        Specular == NULL)
    {
        return NULL;
    }

    PhongLight = (PDIRECTIONAL_PHONG_LIGHT) malloc(sizeof(DIRECTIONAL_PHONG_LIGHT));

    PhongLight->PhongLightHeader.PhongLightVTable = &DirectionalPhongLightVTable;
    PhongLight->Ambient = *Ambient;
    PhongLight->Diffuse = *Diffuse;
    PhongLight->Specular = *Specular;
    PhongLight->CastsShadows = CastsShadows;

    VectorNormalize(WorldDirectionToLight,
                    &PhongLight->WorldDirectionToLight);

    return (PPHONG_LIGHT) PhongLight;
}

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PPHONG_LIGHT
PointPhongLightAllocate(
    _In_ PCPOINT3 WorldLocation,
    _In_ PCOLOR3 Ambient,
    _In_ PCOLOR3 Diffuse,
    _In_ PCOLOR3 Specular,
    _In_ BOOL CastsShadows
    )
{
    PPOINT_PHONG_LIGHT PhongLight;

    if (WorldLocation == NULL ||
        Ambient == NULL ||
        Diffuse == NULL ||
        Specular == NULL)
    {
        return NULL;
    }

    PhongLight = (PPOINT_PHONG_LIGHT) malloc(sizeof(POINT_PHONG_LIGHT));

    PhongLight->PhongLightHeader.PhongLightVTable = &DirectionalPhongLightVTable;
    PhongLight->WorldLocation = *WorldLocation;
    PhongLight->Ambient = *Ambient;
    PhongLight->Diffuse = *Diffuse;
    PhongLight->Specular = *Specular;
    PhongLight->CastsShadows = CastsShadows;

    return (PPHONG_LIGHT) PhongLight;
}

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PPHONG_LIGHT
AttenuatedPointPhongLightAllocate(
    _In_ PCPOINT3 WorldLocation,
    _In_ PCOLOR3 Ambient,
    _In_ PCOLOR3 Diffuse,
    _In_ PCOLOR3 Specular,
    _In_ BOOL CastsShadows,
    _In_ FLOAT ConstantAttenuation,
    _In_ FLOAT LinearAttenuation,
    _In_ FLOAT QuadraticAttenuation
    )
{
    PATTENUATED_POINT_PHONG_LIGHT PhongLight;

    if (WorldLocation == NULL ||
        Ambient == NULL ||
        Diffuse == NULL ||
        Specular == NULL ||
        IsNormalFloat(ConstantAttenuation) == FALSE ||
        IsFiniteFloat(ConstantAttenuation) == FALSE ||
        IsNormalFloat(LinearAttenuation) == FALSE ||
        IsFiniteFloat(LinearAttenuation) == FALSE ||
        IsNormalFloat(QuadraticAttenuation) == FALSE ||
        IsFiniteFloat(QuadraticAttenuation) == FALSE)
    {
        return NULL;
    }

    PhongLight = (PATTENUATED_POINT_PHONG_LIGHT) malloc(sizeof(ATTENUATED_POINT_PHONG_LIGHT));

    PhongLight->PhongLightHeader.PhongLightVTable = &DirectionalPhongLightVTable;
    PhongLight->WorldLocation = *WorldLocation;
    PhongLight->Ambient = *Ambient;
    PhongLight->Diffuse = *Diffuse;
    PhongLight->Specular = *Specular;
    PhongLight->CastsShadows = CastsShadows;
    PhongLight->ConstantAttenuation = ConstantAttenuation;
    PhongLight->LinearAttenuation = LinearAttenuation;
    PhongLight->QuadraticAttenuation = QuadraticAttenuation;

    return (PPHONG_LIGHT) PhongLight;
}

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PPHONG_LIGHT
PointPhongSpotLightAllocate(
    _In_ PCPOINT3 WorldLocation,
    _In_ PCOLOR3 Ambient,
    _In_ PCOLOR3 Diffuse,
    _In_ PCOLOR3 Specular,
    _In_ BOOL CastsShadows,
    _In_ PCVECTOR3 WorldSpotLightDirection,
    _In_ FLOAT SpotLightExponent,
    _In_ FLOAT SpotLightCutoff
    )
{
    PPOINT_PHONG_SPOT_LIGHT PhongLight;

    if (WorldLocation == NULL ||
        Ambient == NULL ||
        Diffuse == NULL ||
        Specular == NULL ||
        WorldSpotLightDirection == NULL ||
        IsNormalFloat(SpotLightExponent) == FALSE ||
        IsFiniteFloat(SpotLightExponent) == FALSE ||
        IsNormalFloat(SpotLightCutoff) == FALSE ||
        IsFiniteFloat(SpotLightCutoff) == FALSE)
    {
        return NULL;
    }

    PhongLight = (PPOINT_PHONG_SPOT_LIGHT) malloc(sizeof(POINT_PHONG_SPOT_LIGHT));

    PhongLight->PhongLightHeader.PhongLightVTable = &DirectionalPhongLightVTable;
    PhongLight->WorldLocation = *WorldLocation;
    PhongLight->Ambient = *Ambient;
    PhongLight->Diffuse = *Diffuse;
    PhongLight->Specular = *Specular;
    PhongLight->CastsShadows = CastsShadows;
    PhongLight->SpotLightExponent = SpotLightExponent;
    PhongLight->SpotLightCutoff = SpotLightCutoff;

    VectorNormalize(WorldSpotLightDirection,
                    &PhongLight->WorldSpotLightDirection);

    return (PPHONG_LIGHT) PhongLight;
}

_Check_return_
_Ret_maybenull_
IRISTOOLKITAPI
PPHONG_LIGHT
AttenuatedPhongSpotLightAllocate(
    _In_ PCPOINT3 WorldLocation,
    _In_ PCOLOR3 Ambient,
    _In_ PCOLOR3 Diffuse,
    _In_ PCOLOR3 Specular,
    _In_ BOOL CastsShadows,
    _In_ FLOAT ConstantAttenuation,
    _In_ FLOAT LinearAttenuation,
    _In_ FLOAT QuadraticAttenuation,
    _In_ PCVECTOR3 WorldSpotLightDirection,
    _In_ FLOAT SpotLightExponent,
    _In_ FLOAT SpotLightCutoff
    )
{
    PATTENUATED_POINT_PHONG_SPOT_LIGHT PhongLight;

    if (WorldLocation == NULL ||
        Ambient == NULL ||
        Diffuse == NULL ||
        Specular == NULL ||
        IsNormalFloat(ConstantAttenuation) == FALSE ||
        IsFiniteFloat(ConstantAttenuation) == FALSE ||
        IsNormalFloat(LinearAttenuation) == FALSE ||
        IsFiniteFloat(LinearAttenuation) == FALSE ||
        IsNormalFloat(QuadraticAttenuation) == FALSE ||
        IsFiniteFloat(QuadraticAttenuation) == FALSE ||
        WorldSpotLightDirection == NULL ||
        IsNormalFloat(SpotLightExponent) == FALSE ||
        IsFiniteFloat(SpotLightExponent) == FALSE ||
        IsNormalFloat(SpotLightCutoff) == FALSE ||
        IsFiniteFloat(SpotLightCutoff) == FALSE)
    {
        return NULL;
    }

    PhongLight = (PATTENUATED_POINT_PHONG_SPOT_LIGHT) malloc(sizeof(ATTENUATED_POINT_PHONG_SPOT_LIGHT));

    PhongLight->PhongLightHeader.PhongLightVTable = &DirectionalPhongLightVTable;
    PhongLight->WorldLocation = *WorldLocation;
    PhongLight->Ambient = *Ambient;
    PhongLight->Diffuse = *Diffuse;
    PhongLight->Specular = *Specular;
    PhongLight->CastsShadows = CastsShadows;
    PhongLight->ConstantAttenuation = ConstantAttenuation;
    PhongLight->LinearAttenuation = LinearAttenuation;
    PhongLight->QuadraticAttenuation = QuadraticAttenuation;
    PhongLight->SpotLightExponent = SpotLightExponent;
    PhongLight->SpotLightCutoff = SpotLightCutoff;

    VectorNormalize(WorldSpotLightDirection,
                    &PhongLight->WorldSpotLightDirection);

    return (PPHONG_LIGHT) PhongLight;
}