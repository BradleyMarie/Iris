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

    ASSERT(IsFiniteFloat(nf) != FALSE);
    ASSERT(IsFiniteFloat(fPdf) != FALSE);
    ASSERT(IsFiniteFloat(ng) != FALSE);
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
    _Inout_ PSPECTRUM_VISIBILITY_TESTER VisibilityTester,
    _Inout_ PRANDOM Rng,
    _Inout_ PSPECTRUM_COMPOSITOR SpectrumCompositor,
    _Inout_ PREFLECTOR_COMPOSITOR ReflectorCompositor,
    _Out_ PCSPECTRUM *Spectrum
    )
{
    BOOL IsGreaterThanZero;
    PSPECTRUM LightSpectrum;
    PCREFLECTOR Reflector;
    FLOAT Attenuation;
    FLOAT DotProduct;
    PSPECTRUM Output;
    PSPECTRUM Emissive;
    VECTOR3 ToLight;
    RAY RayToLight;
    ISTATUS Status;
    FLOAT LightPdf;
    FLOAT BrdfPdf;
    FLOAT Weight;

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

    IsGreaterThanZero = IsGreaterThanZeroFloat(LightPdf);

    if (IsInfiniteFloat(LightPdf) != FALSE)
    {
        if (IsGreaterThanZero == FALSE ||
            LightSpectrum == NULL)
        {
            *Spectrum = NULL;
            return ISTATUS_SUCCESS;
        }

        Status = BrdfComputeReflectance(Brdf,
                                        IncidentDirection,
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
                                                           Spectrum);

        return Status;
    }

    //
    // Light is an area light
    //

    if (IsGreaterThanZero != FALSE)
    {
        Status = BrdfComputeReflectanceWithPdf(Brdf,
                                               IncidentDirection,
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
     
    //
    // Sample the BRDF
    //

    Status = BrdfSample(Brdf,
                        IncidentDirection,
                        Rng,
                        ReflectorCompositor,
                        &Reflector,
                        &ToLight,
                        &BrdfPdf);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    if (IsLessThanZeroFloat(BrdfPdf) != FALSE ||
        Reflector == NULL)
    {
        *Spectrum = Output;
        return ISTATUS_SUCCESS;
    }
    
    RayToLight = RayCreate(WorldHitPoint, ToLight);

    if (IsInfiniteFloat(BrdfPdf) != FALSE)
    {
        Status = LightComputeEmissive(Light,
                                      RayToLight,
                                      VisibilityTester,
                                      &Emissive);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        if (Emissive == NULL)
        {
            *Spectrum = Output;
            return ISTATUS_SUCCESS;
        }

        DotProduct = VectorDotProduct(SurfaceNormal, ToLight);

        Attenuation = DotProduct / BrdfPdf;
    }
    else
    {
        Status = LightComputeEmissiveWithPdf(Light,
                                             RayToLight,
                                             VisibilityTester,
                                             &Emissive,
                                             &LightPdf);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        if (IsLessThanZeroFloat(LightPdf) != FALSE ||
            Emissive == NULL)
        {
            *Spectrum = Output;
            return ISTATUS_SUCCESS;
        }

        DotProduct = VectorDotProduct(SurfaceNormal, ToLight);

        Weight = PowerHeuristic((FLOAT) 1.0f, 
                                BrdfPdf,
                                (FLOAT) 1.0f,
                                LightPdf);

        Attenuation = (DotProduct * Weight) / BrdfPdf;
    }

    Status = SpectrumCompositorAttenuatedAddReflection(SpectrumCompositor,
                                                       Emissive,
                                                       Reflector,
                                                       Attenuation,
                                                       &Emissive);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = SpectrumCompositorAddSpectrums(SpectrumCompositor,
                                            Output,
                                            Emissive,
                                            &Output);
                                            
    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *Spectrum = Output;
    return ISTATUS_SUCCESS;
}