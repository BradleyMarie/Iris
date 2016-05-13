/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    phong.cpp

Abstract:

    Definitions for phong test routines.

--*/

#include <iristest.h>

//
// Macros
//

#define PHONG_EMISSIVE_R 1.0f
#define PHONG_EMISSIVE_G 2.0f
#define PHONG_EMISSIVE_B 3.0f
#define PHONG_DIFFUSE_R 4.0f
#define PHONG_DIFFUSE_G 5.0f
#define PHONG_DIFFUSE_B 6.0f
#define PHONG_SPECULAR_R 7.0f
#define PHONG_SPECULAR_G 8.0f
#define PHONG_SPECULAR_B 9.0f

//
// Functions
//

IrisAdvanced::Color3
PhongToRGB(
    _In_ IrisSpectrum::SpectrumReference Spectra
    )
{
    FLOAT R = Spectra.Sample(PHONG_EMISSIVE_R) + 
              Spectra.Sample(PHONG_DIFFUSE_R) +
              Spectra.Sample(PHONG_SPECULAR_R);
              
    FLOAT G = Spectra.Sample(PHONG_EMISSIVE_G) + 
              Spectra.Sample(PHONG_DIFFUSE_G) +
              Spectra.Sample(PHONG_SPECULAR_G);
              
    FLOAT B = Spectra.Sample(PHONG_EMISSIVE_B) + 
              Spectra.Sample(PHONG_DIFFUSE_B) +
              Spectra.Sample(PHONG_SPECULAR_B);
    
    return IrisAdvanced::Color3(R, G, B);
}

PhongEmissiveReflector::PhongEmissiveReflector(
    _In_ const IrisAdvanced::Color3 & Emissive
    )
    : e(Emissive)
{ }

IrisSpectrum::Reflector
PhongEmissiveReflector::Create(
    _In_ const IrisAdvanced::Color3 & Emissive
    )
{
    return ReflectorBase::Create(std::unique_ptr<PhongEmissiveReflector>(new PhongEmissiveReflector(Emissive)));
}

FLOAT
PhongEmissiveReflector::Reflect(
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity
    ) const
{
    if (Wavelength == PHONG_EMISSIVE_R)
    {
        return e.Red();
    }
    
    if (Wavelength == PHONG_EMISSIVE_G)
    {
        return e.Green();
    }
    
    if (Wavelength <= PHONG_EMISSIVE_B)
    {
        return e.Blue();
    }
    
    return 0.0f;
}

PhongDiffuseReflector::PhongDiffuseReflector(
    _In_ const IrisAdvanced::Color3 & Diffuse
    )
    : d(Diffuse)
{ }

IrisSpectrum::Reflector
PhongDiffuseReflector::Create(
    _In_ const IrisAdvanced::Color3 & Diffuse
    )
{
    return ReflectorBase::Create(std::unique_ptr<PhongDiffuseReflector>(new PhongDiffuseReflector(Diffuse)));
}

FLOAT
PhongDiffuseReflector::Reflect(
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity
    ) const
{
    if (Wavelength <= PHONG_DIFFUSE_R)
    {
        return IncomingIntensity * d.Red();
    }
    
    if (Wavelength <= PHONG_DIFFUSE_G)
    {
        return IncomingIntensity * d.Green();
    }
    
    if (Wavelength <= PHONG_DIFFUSE_B)
    {
        return IncomingIntensity * d.Blue();
    }
    
    return 0.0f;
}

PhongSpecularReflector::PhongSpecularReflector(
    _In_ const IrisAdvanced::Color3 & Specular
    )
    : s(Specular)
{ }

IrisSpectrum::Reflector
PhongSpecularReflector::Create(
    _In_ const IrisAdvanced::Color3 & Specular
    )
{
    return ReflectorBase::Create(std::unique_ptr<PhongSpecularReflector>(new PhongSpecularReflector(Specular)));
}

FLOAT
PhongSpecularReflector::Reflect(
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity
    ) const
{
    if (Wavelength <= PHONG_EMISSIVE_R)
    {
        return IncomingIntensity * s.Red();
    }

    if (Wavelength <= PHONG_EMISSIVE_G)
    {
        return IncomingIntensity * s.Green();
    }

    if (Wavelength <= PHONG_EMISSIVE_B)
    {
        return IncomingIntensity * s.Blue();
    }

    if (Wavelength <= PHONG_DIFFUSE_R)
    {
        return IncomingIntensity * s.Red();
    }

    if (Wavelength <= PHONG_DIFFUSE_G)
    {
        return IncomingIntensity * s.Green();
    }

    if (Wavelength <= PHONG_DIFFUSE_R)
    {
        return IncomingIntensity * s.Blue();
    }

    if (Wavelength <= PHONG_SPECULAR_B)
    {
        return IncomingIntensity * s.Red();
    }
    
    if (Wavelength <= PHONG_SPECULAR_G)
    {
        return IncomingIntensity * s.Green();
    }
    
    if (Wavelength <= PHONG_SPECULAR_B)
    {
        return IncomingIntensity * s.Blue();
    }
    
    return 0.0f;
}

PhongReflectiveReflector::PhongReflectiveReflector(
    _In_ const IrisAdvanced::Color3 & Reflective
    )
    : r(Reflective)
{ }

IrisSpectrum::Reflector
PhongReflectiveReflector::Create(
    _In_ const IrisAdvanced::Color3 & Reflective
    )
{
    if (Reflective.IsBlack())
    {
        return IrisSpectrum::Reflector(nullptr, false);
    }

    return ReflectorBase::Create(std::unique_ptr<PhongReflectiveReflector>(new PhongReflectiveReflector(Reflective)));
}

FLOAT
PhongReflectiveReflector::Reflect(
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity
    ) const
{
    if (Wavelength <= PHONG_SPECULAR_R)
    {
        return IncomingIntensity * r.Red();
    }

    if (Wavelength <= PHONG_SPECULAR_G)
    {
        return IncomingIntensity * r.Green();
    }

    if (Wavelength <= PHONG_SPECULAR_B)
    {
        return IncomingIntensity * r.Blue();
    }

    return 0.0f;
}

PhongBRDF::PhongBRDF(
    _In_ IrisSpectrum::ReflectorReference Emissive,
    _In_ IrisSpectrum::ReflectorReference Diffuse,
    _In_ IrisSpectrum::ReflectorReference Specular,
    _In_ const FLOAT S,
    _In_ IrisSpectrum::ReflectorReference Reflective,
    _In_ const Iris::Vector & N
    )
    : EmissiveReflector(Emissive),
      DiffuseReflector(Diffuse),
      SpecularReflector(Specular),
      Shininess(S),
      ReflectiveReflector(Reflective),
      SurfaceNormal(N)
{ }

std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
PhongBRDF::Sample(
    _In_ const Iris::Vector & Incoming,
    _In_ IrisAdvanced::RandomReference Rng,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    if (ReflectiveReflector.AsPCREFLECTOR() != nullptr)
    {
        Iris::Vector Reflected = Iris::Vector::Reflect(Incoming, SurfaceNormal);
        IrisSpectrum::ReflectorReference Result = Compositor.Add(ReflectiveReflector, EmissiveReflector);
        return make_tuple(Result, Reflected, 1.0f);
    }

    return make_tuple(EmissiveReflector, -Incoming, 0.0f);
}
    
std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
PhongBRDF::SampleWithLambertianFalloff(
    _In_ const Iris::Vector & Incoming,
    _In_ IrisAdvanced::RandomReference Rng,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    if (ReflectiveReflector.AsPCREFLECTOR() != nullptr)
    {
        Iris::Vector Reflected = Iris::Vector::Reflect(Incoming, SurfaceNormal);
        IrisSpectrum::ReflectorReference Result = Compositor.Add(ReflectiveReflector, EmissiveReflector);
        return make_tuple(Result, Reflected, 1.0f);
    }

    return make_tuple(EmissiveReflector, -Incoming, 0.0f);
}

IrisSpectrum::ReflectorReference
PhongBRDF::ComputeReflectance(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    IrisSpectrum::ReflectorReference Result = EmissiveReflector;
    
    FLOAT LambertianAttenuation = Iris::Vector::DotProduct(Incoming, Outgoing);
    Result = Compositor.Add(Compositor.Attenuate(DiffuseReflector, LambertianAttenuation),
                            Result);
    
    Iris::Vector HalfAngle = Iris::Vector::HalfAngle(-Incoming, Outgoing);
    FLOAT SpecularPower = Iris::Vector::DotProduct(HalfAngle, SurfaceNormal);
    
    if (SpecularPower > (FLOAT) 0.0)
    {
        SpecularPower = PowFloat(SpecularPower, Shininess);
        Result = Compositor.Add(Compositor.Attenuate(SpecularReflector, SpecularPower),
                                Result);
    }
    
    return Result;
}

IrisSpectrum::ReflectorReference
PhongBRDF::ComputeReflectanceWithLambertianFalloff(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    IrisSpectrum::ReflectorReference Result = EmissiveReflector;

    FLOAT LambertianAttenuation = Iris::Vector::DotProduct(Incoming, Outgoing);
    Result = Compositor.Add(Compositor.Attenuate(DiffuseReflector, LambertianAttenuation),
                            Result);

    Iris::Vector HalfAngle = Iris::Vector::HalfAngle(-Incoming, Outgoing);
    FLOAT SpecularPower = Iris::Vector::DotProduct(HalfAngle, SurfaceNormal);

    if (SpecularPower > (FLOAT) 0.0)
    {
        SpecularPower = PowFloat(SpecularPower, Shininess);
        Result = Compositor.Add(Compositor.Attenuate(SpecularReflector, SpecularPower),
                                Result);
    }
    
    return Result;
}

std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
PhongBRDF::ComputeReflectanceWithPdf(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    IrisSpectrum::ReflectorReference Result = EmissiveReflector;

    FLOAT LambertianAttenuation = Iris::Vector::DotProduct(Incoming, Outgoing);
    Result = Compositor.Add(Compositor.Attenuate(DiffuseReflector, LambertianAttenuation),
                            Result);

    Iris::Vector HalfAngle = Iris::Vector::HalfAngle(-Incoming, Outgoing);
    FLOAT SpecularPower = Iris::Vector::DotProduct(HalfAngle, SurfaceNormal);

    if (SpecularPower > (FLOAT) 0.0)
    {
        SpecularPower = PowFloat(SpecularPower, Shininess);
        Result = Compositor.Add(Compositor.Attenuate(SpecularReflector, SpecularPower),
                                Result);
    }
    
    return make_tuple(Result, 1.0f);
}

std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
PhongBRDF::ComputeReflectanceWithPdfWithLambertianFalloff(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    IrisSpectrum::ReflectorReference Result = EmissiveReflector;

    FLOAT LambertianAttenuation = Iris::Vector::DotProduct(Incoming, Outgoing);
    Result = Compositor.Add(Compositor.Attenuate(DiffuseReflector, LambertianAttenuation),
                            Result);

    Iris::Vector HalfAngle = Iris::Vector::HalfAngle(-Incoming, Outgoing);
    FLOAT SpecularPower = Iris::Vector::DotProduct(HalfAngle, SurfaceNormal);

    if (SpecularPower > (FLOAT) 0.0)
    {
        SpecularPower = PowFloat(SpecularPower, Shininess);
        Result = Compositor.Add(Compositor.Attenuate(SpecularReflector, SpecularPower),
                                Result);
    }
    
    return make_tuple(Result, 1.0f);
}

PhongMaterial::PhongMaterial(
    _In_ const IrisAdvanced::Color3 & Emissive,
    _In_ const IrisAdvanced::Color3 & Diffuse,
    _In_ const IrisAdvanced::Color3 & Specular,
    _In_ FLOAT S,
    _In_ const IrisAdvanced::Color3 & Reflective
    )
: EmissiveReflector(PhongEmissiveReflector::Create(Emissive)),
  DiffuseReflector(PhongDiffuseReflector::Create(Diffuse)),
  SpecularReflector(PhongSpecularReflector::Create(Diffuse)),
  Shininess(S),
  ReflectiveReflector(PhongReflectiveReflector::Create(Reflective))
{ }

IrisPhysx::Material
PhongMaterial::Create(
    _In_ const IrisAdvanced::Color3 & Emissive,
    _In_ const IrisAdvanced::Color3 & Diffuse,
    _In_ const IrisAdvanced::Color3 & Specular,
    _In_ FLOAT S,
    _In_ const IrisAdvanced::Color3 & Reflective
    )
{
    return MaterialBase::Create(std::unique_ptr<PhongMaterial>(new PhongMaterial(Emissive, Diffuse, Specular, S, Reflective)));
}

IrisPhysx::BRDFReference
PhongMaterial::Sample(
    _In_ const Iris::Point & ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _In_ const Iris::Vector & SurfaceNormal,
    _In_ const Iris::MatrixReference & ModelToWorld,
    _In_ IrisPhysx::BRDFAllocator Allocator
    ) const
{
    PhongBRDF Brdf(EmissiveReflector.AsReflectorReference(),
                   DiffuseReflector.AsReflectorReference(),
                   SpecularReflector.AsReflectorReference(),
                   Shininess,
                   ReflectiveReflector.AsReflectorReference(),
                   SurfaceNormal);

    return Allocator.Allocate(Brdf);
}