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
        if (Wavelength == 0)
        {
            return R;
        }

        if (Wavelength == 1)
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
    PRANDOM Mwc;
    CHECK_EQUAL(ISTATUS_SUCCESS, MultiplyWithCarryRngAllocate(&Mwc));
    RandomReference Rng(RandomGetRandomReference(Mwc));

    Material ConstantMaterial = ConstantRGBMaterial::Create(1.0f, 0.0f, 1.0f);

    PPBR_GEOMETRY Sphere;
    ISTATUS Status = PhysxSphereAllocate(Point(0.0f, 0.0f, 0.0f).AsPOINT3(),
                                         1.0f,
                                         ConstantMaterial.AsPPBR_MATERIAL(),
                                         ConstantMaterial.AsPPBR_MATERIAL(),
                                         nullptr,
                                         nullptr,
                                         &Sphere);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    TestListScene Scene;
    Scene.AddGeometry(Geometry(Sphere, false));

    Integrator Integrate = Integrator::Create(0);
}