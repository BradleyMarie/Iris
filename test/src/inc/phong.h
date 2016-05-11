/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    phong.h

Abstract:

    This file contains the prototypes for the PhysX Phong functions

--*/

#ifndef _IRIS_TEST_PHONG_HEADER_
#define _IRIS_TEST_PHONG_HEADER_

#include <iristest.h>

IrisAdvanced::Color3
PhongToRGB(
    _In_ IrisSpectrum::SpectrumReference Spectra
    );

class PhongEmissiveReflector final : public IrisSpectrum::ReflectorBase{
private:
    PhongEmissiveReflector(
        _In_ const IrisAdvanced::Color3 & Emissive
        );

public:
    static
    IrisSpectrum::Reflector
    Create(
        _In_ const IrisAdvanced::Color3 & Emissive
        );
        
    virtual
    FLOAT
    Reflect(
        _In_ FLOAT Wavelength,
        _In_ FLOAT IncomingIntensity
        ) const;

private:
    IrisAdvanced::Color3 e;
};

class PhongDiffuseReflector final : public IrisSpectrum::ReflectorBase {
private:
    PhongDiffuseReflector(
        _In_ const IrisAdvanced::Color3 & Diffuse
        );

public:
    static
    IrisSpectrum::Reflector
    Create(
        _In_ const IrisAdvanced::Color3 & Diffuse
        );
        
    virtual
    FLOAT
    Reflect(
        _In_ FLOAT Wavelength,
        _In_ FLOAT IncomingIntensity
        ) const;

private:
    IrisAdvanced::Color3 d;
};

class PhongSpecularReflector final : public IrisSpectrum::ReflectorBase {
private:
    PhongSpecularReflector(
        _In_ const IrisAdvanced::Color3 & Specular
        );

public:
    static
    IrisSpectrum::Reflector
    Create(
        _In_ const IrisAdvanced::Color3 & Specular
        );
        
    virtual
    FLOAT
    Reflect(
        _In_ FLOAT Wavelength,
        _In_ FLOAT IncomingIntensity
        ) const;

private:
    IrisAdvanced::Color3 s;
};

class PhongBRDF final {
public:
    PhongBRDF(
        _In_ IrisSpectrum::ReflectorReference Emissive,
        _In_ IrisSpectrum::ReflectorReference Diffuse,
        _In_ IrisSpectrum::ReflectorReference Specular,
        _In_ const FLOAT S,
        _In_ const Iris::Vector & N
        );

    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    Sample(
        _In_ const Iris::Vector & Incoming,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
        
    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    SampleWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    IrisSpectrum::ReflectorReference
    ComputeReflectance(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    IrisSpectrum::ReflectorReference
    ComputeReflectanceWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdf(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdfWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;

private:
    Iris::Vector SurfaceNormal;
    IrisSpectrum::ReflectorReference EmissiveReflector;
    IrisSpectrum::ReflectorReference DiffuseReflector;
    IrisSpectrum::ReflectorReference SpecularReflector;
    FLOAT Shininess;
};

class PhongMaterial final : public IrisPhysx::MaterialBase {
private:
    PhongMaterial(
        _In_ IrisAdvanced::Color3 Emissive,
        _In_ IrisAdvanced::Color3 Diffuse,
        _In_ IrisAdvanced::Color3 Specular,
        _In_ const FLOAT S
        );

public:
    static
    IrisPhysx::Material
    Create(
        _In_ IrisAdvanced::Color3 Emissive,
        _In_ IrisAdvanced::Color3 Diffuse,
        _In_ IrisAdvanced::Color3 Specular,
        _In_ const FLOAT S
        );

    virtual
    IrisPhysx::BRDFReference
    Sample(
        _In_ const Iris::Point & ModelHitPoint,
        _In_ PCVOID AdditionalData,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::MatrixReference & ModelToWorld,
        _In_ IrisPhysx::BRDFAllocator Allocator
        ) const;

private:
    IrisSpectrum::Reflector EmissiveReflector;
    IrisSpectrum::Reflector DiffuseReflector;
    IrisSpectrum::Reflector SpecularReflector;
    FLOAT Shininess;
};

#endif // _IRIS_TEST_PHONG_HEADER_