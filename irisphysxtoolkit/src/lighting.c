/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    lighting.h

Abstract:

    This file contains the definitions for lighting routines.

--*/

#include <irisphysxtoolkitp.h>

//
// Functions
//

SFORCEINLINE
FLOAT
PowerHeuristic(
    _In_ FLOAT nf,
    _In_ FLOAT fPdf,
    _In_ FLOAT ng,
    _In_ FLOAT gPdf
    )
{
    FLOAT Result;
    FLOAT F;
    FLOAT G;

    ASSERT(IsNormalFloat(nf) != FALSE);
    ASSERT(IsFiniteFloat(nf) != FALSE);
    ASSERT(IsNormalFloat(fdf) != FALSE);
    ASSERT(IsFiniteFloat(fPdf) != FALSE);
    ASSERT(IsNormalFloat(ng) != FALSE);
    ASSERT(IsFiniteFloat(ng) != FALSE);
    ASSERT(IsNormalFloat(gPdf) != FALSE);
    ASSERT(IsFiniteFloat(gPdf) != FALSE);

    F = nf * fPdf;
    G = ng * gPdf;

    Result = (F * F) / (F * F + G * G);

    return Result;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ComputeDirectLighting(
    _In_ PCLIGHT Light,
    _In_ PCBRDF Brdf,
    _In_ POINT3 WorldHitPoint,
    _In_ VECTOR3 IncidentDirection,
    _In_ VECTOR3 SurfaceNormal,
    _Inout_ PSPECTRUM_RAYTRACER RayTracer,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_ PRANDOM Rng,
    _Inout_ PSPECTRUM_COMPOSITOR SpectrumCompositor,
    _Inout_ PREFLECTOR_COMPOSITOR ReflectorCompositor,
    _Out_ PSPECTRUM *Spectrum
    )
{
    PCSPECTRUM_SHAPE_HIT ShapeHit;
    PSPECTRUM LightSpectrum;
    PREFLECTOR Reflector;
    FLOAT Attenuation;
    FLOAT DotProduct;
    VECTOR3 ToCamera;
    PSPECTRUM Output;
    PSPECTRUM Emissive;
    VECTOR3 ToLight;
    RAY RayToLight;
    ISTATUS Status;
    FLOAT LightPdf;
    FLOAT BrdfPdf;

    Status = LightSample(Light,
                         WorldHitPoint,
                         VisibilityTester,
                         Rng,
                         SpectrumCompositor,
                         &LightSpectrum,
                         &ToLight,
                         &LightPdf);
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    //
    // Light sampling routine is a delta function
    //

    if (IsFiniteFloat(LightPdf) == FALSE)
    {
        if (LightSpectrum == NULL)
        {
            *Spectrum = NULL;
            return ISTATUS_SUCCESS;
        }
        
        ToCamera = VectorNegate(IncidentDirection);

        Status = BrdfComputeReflectance(Brdf,
                                        ToCamera,
                                        ToLight,
                                        ReflectorCompositor,
                                        &Reflector);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        if (Reflector == NULL)
        {
            *Spectrum = NULL;
            return ISTATUS_SUCCESS;
        }

        DotProduct = VectorDotProduct(SurfaceNormal, ToLight);

        Status = SpectrumCompositorAttenuatedAddReflection(SpectrumCompositor,
                                                           LightSpectrum,
                                                           Reflector,
                                                           DotProduct,
                                                           &Spectrum);

        return Status;
    }

    if (LighPdf > (FLOAT) 0.0f)
    {
        Status = BrdfComputeReflectanceWithPdf(Brdf,
                                               ToCamera,
                                               ToLight,
                                               ReflectorCompositor,
                                               &Reflector,
                                               &BrdfPdf);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        if (Reflector == NULL)
        {
            *Spectrum = NULL;
            return ISTATUS_SUCCESS;
        }

        DotProduct = VectorDotProduct(SurfaceNormal, ToLight);

        Weight = PowerHeuristic((FLOAT) 1.0f, 
                                BrdfPdf,
                                (FLOAT) 1.0f,
                                LightPdf);

        Attenuation = (DotProduct * Weight) / LightPdf;

        Status = SpectrumCompositorAttenuatedAddReflection(SpectrumCompositor,
                                                           LightSpectrum,
                                                           Reflector,
                                                           Attenuation,
                                                           &Output);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
    }
    else
    {
        Output = NULL;
    }
        
    ToCamera = VectorNegate(IncidentDirection);

    Status = BrdfSample(Brdf,
                        ToCamera,
                        Rng,
                        ReflectorCompositor,
                        &Reflector,
                        &ToLight,
                        &BrdfPdf);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (Reflector == NULL || BsdfPdf <= (FLOAT) 0.0f)
    {
        *Spectrum = Output;
        return ISTATUS_SUCCESS;
    }

/*
    if (!(sampledType & BSDF_SPECULAR)) {
        lightPdf = light->Pdf(p, wi);
        if (lightPdf == 0.)
            return Ld;
        weight = PowerHeuristic(1, bsdfPdf, 1, lightPdf);
    } 
    else
        weight = 1.;
*/

    if (IsFiniteFloat(BsdfPdf) == FALSE)
    {
        Weight = (FLOAT) 1.0f;
    }
    else if (LightPdf)
    {
        
    }
    
    RayToLight = RayCreate(WorldHitPoint, ToLight);
    
    Status = SpectrumRayTracerTraceRay(RayTracer,
                                       RayToLight,
                                       TRUE);
                                       
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    SpectrumRayTracerSort(RayTracer);
    
    Status = SpectrumRayTracerGetNextHit(RayTracer,
                                         &ShapeHit,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL);
                                         
    if (Status != ISTATUS_NO_MORE_DATA)
    {
        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
        
        Light = SpectrumShapeGetLight(ShapeHit->Shape,
                                      ShapeHit->FaceHit);
    }

    Status = LightComputeEmissive(Light,
                                  RayToLight,
                                  &Emissive);         
    
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }
    
    if (Emissive != NULL)
    {
        Attenuation = (DotProduct * Weight) / BsdfPdf;

        Status = SpectrumCompositorAttenuatedAddReflection(SpectrumCompositor,
                                                           Emissive,
                                                           Reflector,
                                                           Attenuation,
                                                           &Emissive);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }
        
        if (Emissive != NULL)
        {
            Status = SpectrumCompositorAddSpectrums(SpectrumCompositor,
                                                    Output,
                                                    Emissive,
                                                    &Output);
                                                    
            if (Status != ISTATUS_SUCCESS)
            {
                return Status;
            }
        }
    }
    
    *Spectrum = Output;
    
    return ISTATUS_SUCCESS;
}