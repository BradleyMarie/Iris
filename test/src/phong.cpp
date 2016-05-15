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
    
    if (Wavelength == PHONG_EMISSIVE_B)
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
    if (Wavelength == PHONG_DIFFUSE_R)
    {
        return IncomingIntensity * d.Red();
    }
    
    if (Wavelength == PHONG_DIFFUSE_G)
    {
        return IncomingIntensity * d.Green();
    }
    
    if (Wavelength == PHONG_DIFFUSE_B)
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
    if (Wavelength == PHONG_SPECULAR_R)
    {
        return IncomingIntensity * s.Red();
    }
    
    if (Wavelength == PHONG_SPECULAR_G)
    {
        return IncomingIntensity * s.Green();
    }
    
    if (Wavelength == PHONG_SPECULAR_B)
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
    if (Wavelength == PHONG_EMISSIVE_R)
    {
        return IncomingIntensity * r.Red();
    }

    if (Wavelength == PHONG_EMISSIVE_G)
    {
        return IncomingIntensity * r.Green();
    }

    if (Wavelength == PHONG_EMISSIVE_B)
    {
        return IncomingIntensity * r.Blue();
    }

    if (Wavelength == PHONG_DIFFUSE_R)
    {
        return IncomingIntensity * r.Red();
    }

    if (Wavelength == PHONG_DIFFUSE_G)
    {
        return IncomingIntensity * r.Green();
    }

    if (Wavelength == PHONG_DIFFUSE_R)
    {
        return IncomingIntensity * r.Blue();
    }

    if (Wavelength == PHONG_SPECULAR_R)
    {
        return IncomingIntensity * r.Red();
    }

    if (Wavelength == PHONG_SPECULAR_G)
    {
        return IncomingIntensity * r.Green();
    }

    if (Wavelength == PHONG_SPECULAR_B)
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
    return Sample(Incoming, Rng, Compositor);
}

IrisSpectrum::ReflectorReference
PhongBRDF::ComputeReflectance(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{    
    FLOAT LambertianAttenuation = MaxFloat(0.0f, Iris::Vector::DotProduct(SurfaceNormal, Outgoing));
    IrisSpectrum::ReflectorReference Result = Compositor.Attenuate(DiffuseReflector, LambertianAttenuation);
    
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
    return ComputeReflectance(Incoming, Outgoing, Compositor);
}

std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
PhongBRDF::ComputeReflectanceWithPdf(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    return make_tuple(ComputeReflectance(Incoming, Outgoing, Compositor), 1.0f);
}

std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
PhongBRDF::ComputeReflectanceWithPdfWithLambertianFalloff(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    return make_tuple(ComputeReflectance(Incoming, Outgoing, Compositor), 1.0f);
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

TriangleInterpolatedPhongBRDF::TriangleInterpolatedPhongBRDF(
    _In_ IrisSpectrum::ReflectorReference Emissive0,
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
    _In_ FLOAT Scale2,
    _In_ const Iris::Vector & N
    )
: EmissiveReflector0(Emissive0),
  DiffuseReflector0(Diffuse0),
  SpecularReflector0(Specular0),
  Shininess0(S0),
  ReflectiveReflector0(Reflective0),
  Scalar0(Scale0),
  EmissiveReflector1(Emissive1),
  DiffuseReflector1(Diffuse1),
  SpecularReflector1(Specular1),
  Shininess1(S1),
  ReflectiveReflector1(Reflective1),
  Scalar1(Scale1),
  EmissiveReflector2(Emissive2),
  DiffuseReflector2(Diffuse2),
  SpecularReflector2(Specular2),
  Shininess2(S2),
  ReflectiveReflector2(Reflective2),
  Scalar2(Scale2),
  SurfaceNormal(N)
{ }

std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
TriangleInterpolatedPhongBRDF::Sample(
    _In_ const Iris::Vector & Incoming,
    _In_ IrisAdvanced::RandomReference Rng,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    IrisSpectrum::ReflectorReference Result = Compositor.Attenuate(EmissiveReflector0, Scalar0);
    Result = Compositor.Add(Result, Compositor.Attenuate(EmissiveReflector1, Scalar1));
    Result = Compositor.Add(Result, Compositor.Attenuate(EmissiveReflector2, Scalar2));

    if (ReflectiveReflector0.AsPCREFLECTOR() != nullptr ||
        ReflectiveReflector1.AsPCREFLECTOR() != nullptr ||
        ReflectiveReflector2.AsPCREFLECTOR() != nullptr)
    {
        Iris::Vector Reflected = Iris::Vector::Reflect(Incoming, SurfaceNormal);
        Result = Compositor.Add(Result, Compositor.Attenuate(ReflectiveReflector0, Scalar0));
        Result = Compositor.Add(Result, Compositor.Attenuate(ReflectiveReflector1, Scalar1));
        Result = Compositor.Add(Result, Compositor.Attenuate(ReflectiveReflector2, Scalar2));
        return make_tuple(Result, Reflected, 1.0f);
    }

    return make_tuple(Result, -Incoming, 0.0f);
}

std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
TriangleInterpolatedPhongBRDF::SampleWithLambertianFalloff(
    _In_ const Iris::Vector & Incoming,
    _In_ IrisAdvanced::RandomReference Rng,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    return Sample(Incoming, Rng, Compositor);
}

IrisSpectrum::ReflectorReference
TriangleInterpolatedPhongBRDF::ComputeReflectance(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    IrisSpectrum::ReflectorReference Result = Compositor.Attenuate(EmissiveReflector0, Scalar0);
    Result = Compositor.Add(Result, Compositor.Attenuate(EmissiveReflector1, Scalar1));
    Result = Compositor.Add(Result, Compositor.Attenuate(EmissiveReflector2, Scalar2));
    
    FLOAT LambertianAttenuation = Iris::Vector::DotProduct(Incoming, Outgoing);
    Result = Compositor.Add(Result, Compositor.Attenuate(DiffuseReflector0, Scalar0 * LambertianAttenuation));
    Result = Compositor.Add(Result, Compositor.Attenuate(DiffuseReflector1, Scalar1 * LambertianAttenuation));
    Result = Compositor.Add(Result, Compositor.Attenuate(DiffuseReflector2, Scalar2 * LambertianAttenuation));
    
    Iris::Vector HalfAngle = Iris::Vector::HalfAngle(-Incoming, Outgoing);
    FLOAT SpecularPower = Iris::Vector::DotProduct(HalfAngle, SurfaceNormal);
    
    if (SpecularPower > (FLOAT) 0.0)
    {
        SpecularPower = Scalar0 * PowFloat(SpecularPower, Shininess0) +
                        Scalar1 * PowFloat(SpecularPower, Shininess1) +
                        Scalar2 * PowFloat(SpecularPower, Shininess2);

        Result = Compositor.Add(Result, Compositor.Attenuate(SpecularReflector0, Scalar0 * SpecularPower));
        Result = Compositor.Add(Result, Compositor.Attenuate(SpecularReflector1, Scalar1 * SpecularPower));
        Result = Compositor.Add(Result, Compositor.Attenuate(SpecularReflector2, Scalar2 * SpecularPower));
    }
    
    return Result;
}

IrisSpectrum::ReflectorReference
TriangleInterpolatedPhongBRDF::ComputeReflectanceWithLambertianFalloff(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    return ComputeReflectance(Incoming, Outgoing, Compositor);
}

std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
TriangleInterpolatedPhongBRDF::ComputeReflectanceWithPdf(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    return make_tuple(ComputeReflectance(Incoming, Outgoing, Compositor), 0.0f);
}

std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
TriangleInterpolatedPhongBRDF::ComputeReflectanceWithPdfWithLambertianFalloff(
    _In_ const Iris::Vector & Incoming,
    _In_ const Iris::Vector & Outgoing,
    _In_ IrisSpectrum::ReflectorCompositorReference Compositor
    ) const
{
    return make_tuple(ComputeReflectance(Incoming, Outgoing, Compositor), 0.0f);
}

TriangleInterpolatedPhongMaterial::TriangleInterpolatedPhongMaterial(
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
    )
: EmissiveReflector0(PhongEmissiveReflector::Create(Emissive0)),
  DiffuseReflector0(PhongDiffuseReflector::Create(Diffuse0)),
  SpecularReflector0(PhongSpecularReflector::Create(Diffuse0)),
  Shininess0(S0),
  ReflectiveReflector0(PhongReflectiveReflector::Create(Reflective0)),
  EmissiveReflector1(PhongEmissiveReflector::Create(Emissive1)),
  DiffuseReflector1(PhongDiffuseReflector::Create(Diffuse1)),
  SpecularReflector1(PhongSpecularReflector::Create(Diffuse1)),
  Shininess1(S1),
  ReflectiveReflector1(PhongReflectiveReflector::Create(Reflective1)),
  EmissiveReflector2(PhongEmissiveReflector::Create(Emissive2)),
  DiffuseReflector2(PhongDiffuseReflector::Create(Diffuse2)),
  SpecularReflector2(PhongSpecularReflector::Create(Diffuse2)),
  Shininess2(S2),
  ReflectiveReflector2(PhongReflectiveReflector::Create(Reflective2))
{ }

IrisPhysx::Material
TriangleInterpolatedPhongMaterial::Create(
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
    )
{
    IrisPhysx::Material Result = MaterialBase::Create(
            std::unique_ptr<TriangleInterpolatedPhongMaterial>(
                new TriangleInterpolatedPhongMaterial(
                    Emissive0, Diffuse0, Specular0, S0, Reflective0,
                    Emissive1, Diffuse1, Specular1, S1, Reflective1,
                    Emissive2, Diffuse2, Specular2, S2, Reflective2)));

    return Result;
}

IrisPhysx::BRDFReference
TriangleInterpolatedPhongMaterial::Sample(
    _In_ const Iris::Point & ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _In_ const Iris::Vector & SurfaceNormal,
    _In_ const Iris::MatrixReference & ModelToWorld,
    _In_ IrisPhysx::BRDFAllocator Allocator
    ) const
{
    const FLOAT *BarycentricCoordinates = (const FLOAT *) AdditionalData;

    TriangleInterpolatedPhongBRDF Brdf(EmissiveReflector0.AsReflectorReference(),
                                       DiffuseReflector0.AsReflectorReference(),
                                       SpecularReflector0.AsReflectorReference(),
                                       Shininess0,
                                       ReflectiveReflector0.AsReflectorReference(),
                                       BarycentricCoordinates[0],
                                       EmissiveReflector1.AsReflectorReference(),
                                       DiffuseReflector1.AsReflectorReference(),
                                       SpecularReflector1.AsReflectorReference(),
                                       Shininess1,
                                       ReflectiveReflector1.AsReflectorReference(),
                                       BarycentricCoordinates[1],
                                       EmissiveReflector2.AsReflectorReference(),
                                       DiffuseReflector2.AsReflectorReference(),
                                       SpecularReflector2.AsReflectorReference(),
                                       Shininess2,
                                       ReflectiveReflector2.AsReflectorReference(),
                                       BarycentricCoordinates[2],
                                       SurfaceNormal);

    return Allocator.Allocate(Brdf);
}

PhongDiffuseSpectrum::PhongDiffuseSpectrum(
    _In_ const IrisAdvanced::Color3 & Diffuse
    )
: d(Diffuse)
{ }

IrisSpectrum::Spectrum
PhongDiffuseSpectrum::Create(
    _In_ const IrisAdvanced::Color3 & Diffuse
    )
{
    return SpectrumBase::Create(std::unique_ptr<PhongDiffuseSpectrum>(new PhongDiffuseSpectrum(Diffuse)));
}

FLOAT
PhongDiffuseSpectrum::Sample(
    _In_ FLOAT Wavelength
    ) const
{
    if (Wavelength == PHONG_DIFFUSE_R)
    {
        return d.Red();
    }

    if (Wavelength == PHONG_DIFFUSE_G)
    {
        return d.Green();
    }

    if (Wavelength == PHONG_DIFFUSE_B)
    {
        return d.Blue();
    }

    return 0.0f;
}

PhongSpecularSpectrum::PhongSpecularSpectrum(
    _In_ const IrisAdvanced::Color3 & Specular
    )
: s(Specular)
{ }

IrisSpectrum::Spectrum
PhongSpecularSpectrum::Create(
    _In_ const IrisAdvanced::Color3 & Specular
    )
{
    return SpectrumBase::Create(std::unique_ptr<PhongSpecularSpectrum>(new PhongSpecularSpectrum(Specular)));
}

FLOAT
PhongSpecularSpectrum::Sample(
    _In_ FLOAT Wavelength
    ) const
{
    if (Wavelength == PHONG_SPECULAR_R)
    {
        return s.Red();
    }

    if (Wavelength == PHONG_SPECULAR_G)
    {
        return s.Green();
    }

    if (Wavelength == PHONG_SPECULAR_B)
    {
        return s.Blue();
    }

    return 0.0f;
}

PhongPointLight::PhongPointLight(
    _In_ const IrisAdvanced::Color3 & Diffuse,
    _In_ const IrisAdvanced::Color3 & Specular,
    _In_ const Iris::Point & Loc
    )
: DiffuseSpectrum(PhongDiffuseSpectrum::Create(Diffuse)),
  SpecularSpectrum(PhongSpecularSpectrum::Create(Specular)),
  Location(Loc)
{ }

IrisPhysx::Light
PhongPointLight::Create(
    _In_ const IrisAdvanced::Color3 & Diffuse,
    _In_ const IrisAdvanced::Color3 & Specular,
    _In_ const Iris::Point & Loc
    )
{
    return LightBase::Create( std::unique_ptr<PhongPointLight>(new PhongPointLight(Diffuse, Specular, Loc)));
}

std::tuple<IrisSpectrum::SpectrumReference, Iris::Vector, FLOAT>
PhongPointLight::Sample(
    _In_ const Iris::Point & HitPoint,
    _In_ IrisPhysx::VisibilityTester Tester,
    _In_ IrisAdvanced::RandomReference Rng,
    _In_ IrisSpectrum::SpectrumCompositorReference Compositor
    ) const
{
    Iris::Vector ToLight = Location - HitPoint;
    FLOAT Distance = ToLight.Length();
    ToLight = Iris::Vector::Normalize(ToLight);

    if (Tester.Test(Iris::Ray(HitPoint, ToLight), Distance))
    {
        return make_tuple(Compositor.Add(DiffuseSpectrum, SpecularSpectrum), ToLight, INFINITY);
    }

    return make_tuple(IrisSpectrum::SpectrumReference(nullptr), ToLight, 0.0f);
}

IrisSpectrum::SpectrumReference
PhongPointLight::ComputeEmissive(
    _In_ const Iris::Ray & ToLight,
    _In_ IrisPhysx::VisibilityTester Tester
    ) const
{
    return IrisSpectrum::SpectrumReference(nullptr);
}

std::tuple<IrisSpectrum::SpectrumReference, FLOAT>
PhongPointLight::ComputeEmissiveWithPdf(
    _In_ const Iris::Ray & ToLight,
    _In_ IrisPhysx::VisibilityTester Tester
    ) const
{
    return make_tuple(IrisSpectrum::SpectrumReference(nullptr), 0.0f);
}