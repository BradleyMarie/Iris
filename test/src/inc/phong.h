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

class PhongReflectiveReflector final : public IrisSpectrum::ReflectorBase {
private:
    PhongReflectiveReflector(
        _In_ const IrisAdvanced::Color3 & Reflective
        );

public:
    static
    IrisSpectrum::Reflector
    Create(
        _In_ const IrisAdvanced::Color3 & Reflective
        );

    virtual
    FLOAT
    Reflect(
        _In_ FLOAT Wavelength,
        _In_ FLOAT IncomingIntensity
        ) const;

private:
    IrisAdvanced::Color3 r;
};

class PhongBRDF final {
public:
    PhongBRDF(
        _In_ IrisSpectrum::ReflectorReference Emissive,
        _In_ IrisSpectrum::ReflectorReference Diffuse,
        _In_ IrisSpectrum::ReflectorReference Specular,
        _In_ const FLOAT S,
        _In_ IrisSpectrum::ReflectorReference Reflective
        );

    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    Sample(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
        
    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    SampleWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    IrisSpectrum::ReflectorReference
    ComputeReflectance(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    IrisSpectrum::ReflectorReference
    ComputeReflectanceWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdf(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdfWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;

private:
    IrisSpectrum::ReflectorReference EmissiveReflector;
    IrisSpectrum::ReflectorReference DiffuseReflector;
    IrisSpectrum::ReflectorReference SpecularReflector;
    FLOAT Shininess;
    IrisSpectrum::ReflectorReference ReflectiveReflector;
};

class PhongMaterial final : public IrisPhysx::MaterialBase {
private:
    PhongMaterial(
        _In_ const IrisAdvanced::Color3 & Emissive,
        _In_ const IrisAdvanced::Color3 & Diffuse,
        _In_ const IrisAdvanced::Color3 & Specular,
        _In_ FLOAT S,
        _In_ const IrisAdvanced::Color3 & Reflective
        );

public:
    static
    IrisPhysx::Material
    Create(
        _In_ const IrisAdvanced::Color3 & Emissive,
        _In_ const IrisAdvanced::Color3 & Diffuse,
        _In_ const IrisAdvanced::Color3 & Specular,
        _In_ FLOAT S,
        _In_ const IrisAdvanced::Color3 & Reflective
        );

    virtual
    std::tuple<IrisPhysx::BRDFReference, Iris::Vector>
    Sample(
        _In_ const Iris::Point & ModelHitPoint,
        _In_ const Iris::Vector & WorldSurfaceNormal,
        _In_ const Iris::Vector & ModelSurfaceNormal,
        _In_ PCVOID AdditionalData,
        _In_ const Iris::MatrixReference & ModelToWorld,
        _In_ IrisPhysx::BRDFAllocator Allocator
        ) const;

private:
    IrisSpectrum::Reflector EmissiveReflector;
    IrisSpectrum::Reflector DiffuseReflector;
    IrisSpectrum::Reflector SpecularReflector;
    FLOAT Shininess;
    IrisSpectrum::Reflector ReflectiveReflector;
};

class TriangleInterpolatedPhongBRDF final {
public:
    TriangleInterpolatedPhongBRDF(
        _In_ IrisSpectrum::ReflectorReference Emissiv0,
        _In_ IrisSpectrum::ReflectorReference Diffuse0,
        _In_ IrisSpectrum::ReflectorReference Specular0,
        _In_ FLOAT S0,
        _In_ IrisSpectrum::ReflectorReference Reflective0,
        _In_ FLOAT Scale0,
        _In_ IrisSpectrum::ReflectorReference Emissive1,
        _In_ IrisSpectrum::ReflectorReference Diffuse1,
        _In_ IrisSpectrum::ReflectorReference Specular1,
        _In_ FLOAT S1,
        _In_ IrisSpectrum::ReflectorReference Reflective1,
        _In_ FLOAT Scale1,
        _In_ IrisSpectrum::ReflectorReference Emissive2,
        _In_ IrisSpectrum::ReflectorReference Diffuse2,
        _In_ IrisSpectrum::ReflectorReference Specular2,
        _In_ FLOAT S2,
        _In_ IrisSpectrum::ReflectorReference Reflective2,
        _In_ FLOAT Scale2
        );

    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    Sample(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
        
    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    SampleWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    IrisSpectrum::ReflectorReference
    ComputeReflectance(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    IrisSpectrum::ReflectorReference
    ComputeReflectanceWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdf(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;
    
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdfWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const;

private:
    IrisSpectrum::ReflectorReference EmissiveReflector0;
    IrisSpectrum::ReflectorReference DiffuseReflector0;
    IrisSpectrum::ReflectorReference SpecularReflector0;
    FLOAT Shininess0;
    IrisSpectrum::ReflectorReference ReflectiveReflector0;
    FLOAT Scalar0;
    IrisSpectrum::ReflectorReference EmissiveReflector1;
    IrisSpectrum::ReflectorReference DiffuseReflector1;
    IrisSpectrum::ReflectorReference SpecularReflector1;
    FLOAT Shininess1;
    IrisSpectrum::ReflectorReference ReflectiveReflector1;
    FLOAT Scalar1;
    IrisSpectrum::ReflectorReference EmissiveReflector2;
    IrisSpectrum::ReflectorReference DiffuseReflector2;
    IrisSpectrum::ReflectorReference SpecularReflector2;
    FLOAT Shininess2;
    IrisSpectrum::ReflectorReference ReflectiveReflector2;
    FLOAT Scalar2;
};

class TriangleInterpolatedPhongMaterial final : public IrisPhysx::MaterialBase {
private:
    TriangleInterpolatedPhongMaterial(
        _In_ const IrisAdvanced::Color3 & Emissive0,
        _In_ const IrisAdvanced::Color3 & Diffuse0,
        _In_ const IrisAdvanced::Color3 & Specular0,
        _In_ FLOAT S0,
        _In_ const IrisAdvanced::Color3 & Reflective0,
        _In_ const IrisAdvanced::Color3 & Emissive1,
        _In_ const IrisAdvanced::Color3 & Diffuse1,
        _In_ const IrisAdvanced::Color3 & Specular1,
        _In_ FLOAT S1,
        _In_ const IrisAdvanced::Color3 & Reflective1,
        _In_ const IrisAdvanced::Color3 & Emissive2,
        _In_ const IrisAdvanced::Color3 & Diffuse2,
        _In_ const IrisAdvanced::Color3 & Specular2,
        _In_ FLOAT S2,
        _In_ const IrisAdvanced::Color3 & Reflective2
        );

public:
    static
    IrisPhysx::Material
    Create(
        _In_ const IrisAdvanced::Color3 & Emissive0,
        _In_ const IrisAdvanced::Color3 & Diffuse0,
        _In_ const IrisAdvanced::Color3 & Specular0,
        _In_ FLOAT S0,
        _In_ const IrisAdvanced::Color3 & Reflective0,
        _In_ const IrisAdvanced::Color3 & Emissive1,
        _In_ const IrisAdvanced::Color3 & Diffuse1,
        _In_ const IrisAdvanced::Color3 & Specular1,
        _In_ FLOAT S1,
        _In_ const IrisAdvanced::Color3 & Reflective1,
        _In_ const IrisAdvanced::Color3 & Emissive2,
        _In_ const IrisAdvanced::Color3 & Diffuse2,
        _In_ const IrisAdvanced::Color3 & Specular2,
        _In_ FLOAT S2,
        _In_ const IrisAdvanced::Color3 & Reflective2
        );

    virtual
    std::tuple<IrisPhysx::BRDFReference, Iris::Vector>
    Sample(
        _In_ const Iris::Point & ModelHitPoint,
        _In_ const Iris::Vector & WorldSurfaceNormal,
        _In_ const Iris::Vector & ModelSurfaceNormal,
        _In_ PCVOID AdditionalData,
        _In_ const Iris::MatrixReference & ModelToWorld,
        _In_ IrisPhysx::BRDFAllocator Allocator
        ) const;

private:
    IrisSpectrum::Reflector EmissiveReflector0;
    IrisSpectrum::Reflector DiffuseReflector0;
    IrisSpectrum::Reflector SpecularReflector0;
    FLOAT Shininess0;
    IrisSpectrum::Reflector ReflectiveReflector0;
    IrisSpectrum::Reflector EmissiveReflector1;
    IrisSpectrum::Reflector DiffuseReflector1;
    IrisSpectrum::Reflector SpecularReflector1;
    FLOAT Shininess1;
    IrisSpectrum::Reflector ReflectiveReflector1;
    IrisSpectrum::Reflector EmissiveReflector2;
    IrisSpectrum::Reflector DiffuseReflector2;
    IrisSpectrum::Reflector SpecularReflector2;
    FLOAT Shininess2;
    IrisSpectrum::Reflector ReflectiveReflector2;
};

class PhongDiffuseSpectrum final : public IrisSpectrum::SpectrumBase {
private:
    PhongDiffuseSpectrum(
        _In_ const IrisAdvanced::Color3 & Diffuse
        );

public:
    static
    IrisSpectrum::Spectrum
    Create(
        _In_ const IrisAdvanced::Color3 & Diffuse
        );

    virtual
    FLOAT
    Sample(
        _In_ FLOAT Wavelength
        ) const;

private:
    IrisAdvanced::Color3 d;
};

class PhongSpecularSpectrum final : public IrisSpectrum::SpectrumBase {
private:
    PhongSpecularSpectrum(
        _In_ const IrisAdvanced::Color3 & Specular
        );

public:
    static
    IrisSpectrum::Spectrum
    Create(
        _In_ const IrisAdvanced::Color3 & Specular
        );

    virtual
    FLOAT
    Sample(
        _In_ FLOAT Wavelength
        ) const;

private:
    IrisAdvanced::Color3 s;
};

class PhongPointLight final : public IrisPhysx::LightBase {
private:
    PhongPointLight(
        _In_ const IrisAdvanced::Color3 & Diffuse,
        _In_ const IrisAdvanced::Color3 & Specular,
        _In_ const Iris::Point & Loc
        );

public:
    static
    IrisPhysx::Light
    Create(
        _In_ const IrisAdvanced::Color3 & Diffuse,
        _In_ const IrisAdvanced::Color3 & Specular,
        _In_ const Iris::Point & Loc
        );

    virtual
    std::tuple<IrisSpectrum::SpectrumReference, Iris::Vector, FLOAT>
    Sample(
        _In_ const Iris::Point & HitPoint,
        _In_ IrisPhysx::VisibilityTester Tester,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::SpectrumCompositorReference Compositor
        ) const;

    virtual
    IrisSpectrum::SpectrumReference
    ComputeEmissive(
        _In_ const Iris::Ray & ToLight,
        _In_ IrisPhysx::VisibilityTester Tester,
        _In_ IrisSpectrum::SpectrumCompositorReference Compositor
        ) const;
    
    virtual
    std::tuple<IrisSpectrum::SpectrumReference, FLOAT>
    ComputeEmissiveWithPdf(
        _In_ const Iris::Ray & ToLight,
        _In_ IrisPhysx::VisibilityTester Tester,
        _In_ IrisSpectrum::SpectrumCompositorReference Compositor
        ) const;

private:
    IrisSpectrum::Spectrum DiffuseSpectrum;
    IrisSpectrum::Spectrum SpecularSpectrum;
    Iris::Point Location;
};

#endif // _IRIS_TEST_PHONG_HEADER_
