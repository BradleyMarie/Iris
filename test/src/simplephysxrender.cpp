/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    simplephysxrender.c

Abstract:

    This file contains tests for the rendering paths.

--*/

#include <iristest.h>
using namespace Iris;
using namespace IrisAdvanced;
using namespace IrisPhysx;
using namespace IrisSpectrum;
using namespace IrisAdvancedToolkit;
using namespace IrisPhysxToolkit;

class ConstantRGBReflector final : public ReflectorBase
{
private:
    ConstantRGBReflector(
        _In_ FLOAT r,
        _In_ FLOAT g,
        _In_ FLOAT b
        )
    : R(r), G(g), B(b)
    { }

public:
    static
    Reflector
    Create(
        _In_ FLOAT r,
        _In_ FLOAT g,
        _In_ FLOAT b
        )
    {
        return ReflectorBase::Create(std::unique_ptr<ConstantRGBReflector>(new ConstantRGBReflector(r, g, b)));
    }

    _Ret_
    virtual
    FLOAT
    Reflect(
        _In_ FLOAT Wavelength,
        _In_ FLOAT IncomingIntensity
        ) const
    {
        if (Wavelength <= 1)
        {
            return R;
        }

        if (Wavelength <= 2)
        {
            return G;
        }

        return B;
    }

private:
    FLOAT R, G, B;
};

class ConstantBrdf final : public BRDFBase {
private:
    ConstantBrdf(
        _In_ FLOAT r,
        _In_ FLOAT g,
        _In_ FLOAT b
        )
    : Data(ConstantRGBReflector::Create(r, g, b))
    { }

public:
    static
    BRDF
    Create(
        _In_ FLOAT r,
        _In_ FLOAT g,
        _In_ FLOAT b
        )
    {
        return IrisPhysx::BRDFBase::Create(std::unique_ptr<IrisPhysx::BRDFBase>(new ConstantBrdf(r, g, b)));
    }

    _Ret_
    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    Sample(
        _In_ const Iris::Vector & Incoming,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const
    {
        return make_tuple(Data.AsReflectorReference(), Incoming, 1.0f);
    }
        
    _Ret_
    std::tuple<IrisSpectrum::ReflectorReference, Iris::Vector, FLOAT>
    SampleWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ IrisAdvanced::RandomReference Rng,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const
    {
        return make_tuple(Data.AsReflectorReference(), Incoming, 1.0f);
    }
    
    _Ret_
    IrisSpectrum::ReflectorReference
    ComputeReflectance(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const
    {
        return Data.AsReflectorReference();
    }
    
    _Ret_
    IrisSpectrum::ReflectorReference
    ComputeReflectanceWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const
    {
        return Data.AsReflectorReference();
    }
    
    _Ret_
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdf(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const
    {
        return make_tuple(Data.AsReflectorReference(), 1.0f);
    }
    
    _Ret_
    std::tuple<IrisSpectrum::ReflectorReference, FLOAT>
    ComputeReflectanceWithPdfWithLambertianFalloff(
        _In_ const Iris::Vector & Incoming,
        _In_ const Iris::Vector & Outgoing,
        _In_ IrisSpectrum::ReflectorCompositorReference Compositor
        ) const
    {
        return make_tuple(Data.AsReflectorReference(), 1.0f);
    }

private:
    Reflector Data;
};

class ConstantRGBMaterial final : public MaterialBase {
private:
    ConstantRGBMaterial(
        _In_ FLOAT r,
        _In_ FLOAT g,
        _In_ FLOAT b
        )
    : Data(ConstantBrdf::Create(r, g, b))
    { }

public:
    static
    Material
    Create(
        _In_ FLOAT r,
        _In_ FLOAT g,
        _In_ FLOAT b
        )
    { 
        return MaterialBase::Create(std::unique_ptr<MaterialBase>(new ConstantRGBMaterial(r, g, b)));
    }

    _Ret_
    virtual
    BRDFReference
    Sample(
        _In_ const Iris::Point & ModelHitPoint,
        _In_ PCVOID AdditionalData,
        _In_ const Iris::Vector & SurfaceNormal,
        _In_ const Iris::MatrixReference & ModelToWorld,
        _In_ BRDFAllocator Allocator
        ) const
    {
        return Data.AsBRDFReference();
    }

private:
    BRDF Data;
};

class TestListScene {
public:
    std::function<void(const Iris::Ray &, IrisPhysx::HitTester)>
    GetTestRoutine(
        void
        )
    {
        std::function<void(const Iris::Ray &, IrisPhysx::HitTester)> func = [&](const Iris::Ray & Ray, IrisPhysx::HitTester Tester) {
            for (size_t i = 0; i < GeometryList.size(); i++)
            {
                Tester.Test(GeometryList[i], Matrices[i], Premultiplied[i] ? TRUE : FALSE);
            }
        };

        return func;
    }

    void
    AddGeometry(
        _In_ Geometry g
        )
    {
        GeometryList.push_back(g);
        Matrices.push_back(Matrix::Identity());
        Premultiplied.push_back(false);
    }

private:
    std::vector<Geometry> GeometryList;
    std::vector<Matrix> Matrices;
    std::vector<bool> Premultiplied;
};

TEST(PhysxRenderConstantRedWorldSphere)
{
    Material ConstantMaterial = ConstantRGBMaterial::Create(1.0f, 0.0f, 0.0f);

    Geometry SphereRadiusOne = Sphere::Create(Point(0.0f, 0.0f, 0.0f),
                                              1.0f,
                                              ConstantMaterial,
                                              ConstantMaterial,
                                              nullopt,
                                              nullopt);

    TestListScene Scene;
    Scene.AddGeometry(SphereRadiusOne);

    ProcessHitRoutine ProcessHitFunc = [](GeometryReference Geom,
                                          UINT32 FaceHit,
                                          Iris::MatrixReference ModelToWorld,
                                          const Iris::Vector & ModelViewer,
                                          const Iris::Point & ModelHitPoint,
                                          const Iris::Point & WorldHitPoint,
                                          const Iris::Ray & WorldRay,
                                          PCPBR_LIGHT *PbrLights,
                                          SIZE_T NumberOfLights,
                                          IrisPhysx::RayTracer * RayTracerPtr,
                                          VisibilityTester Tester,
                                          BRDFAllocator Allocator,
                                          IrisSpectrum::SpectrumCompositorReference SpectrumCompositor,
                                          IrisSpectrum::ReflectorCompositorReference ReflectorCompositor,
                                          IrisAdvanced::RandomReference Rng) -> SpectrumReference
    {
        Vector SurfaceNormal = Geom.ComputeNormal(ModelHitPoint, FaceHit);
        MaterialReference Mat = *(Geom.GetMaterial(FaceHit));
        BRDFReference Brdf = Mat.Sample(ModelHitPoint, nullptr, SurfaceNormal, ModelToWorld, Allocator);
        auto Ref = Brdf.Sample(ModelViewer, Rng, ReflectorCompositor);
        return SpectrumCompositor.Reflect(SpectrumReference(nullptr), std::get<0>(Ref));
    };

    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectrum) -> Color3
    {
        return Color3(Spectrum.Sample(1.0f), Spectrum.Sample(2.0f), Spectrum.Sample(3.0f));
    };

    CreateStateRoutine CreateState = [&](std::vector<Random> & Rngs,
                                         std::vector<ProcessHitRoutine> & ProcessHits,
                                         std::vector<ToneMappingRoutine> & ToneMap,
                                         SIZE_T NumberOfThreads)
    {
        for (SIZE_T Index = 0; Index < NumberOfThreads; Index++)
        {
            Rngs.push_back(MultiplyWithCarry::Create());
            ProcessHits.push_back(ProcessHitFunc);
            ToneMap.push_back(ToneMappingFunc);
        }
    };

    Framebuffer Fb = Framebuffer::Create(Color3(0.0f, 0.0f, 0.0f), 500, 500);
    PinholeCamera::Render(Point(0.0f, 0.0f, 4.0f),
                          1.0f,
                          1.0f,
                          1.0f,
                          Vector(0.0f, 0.0f, -1.0f),
                          Vector(0.0f, 1.0f, 0.0f),
                          0,
                          0,
                          0.0001f,
                          0,
                          false,
                          false,
                          Scene.GetTestRoutine(),
                          nullopt,
                          CreateState,
                          Fb);

    Fb.SaveAsPFM("RenderConstantRedWorldSpherePlusPlus.pfm");
}