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

#define R_WAVELENGTH ((FLOAT) 1.0f)
#define G_WAVELENGTH ((FLOAT) 2.0f)
#define B_WAVELENGTH ((FLOAT) 3.0f)

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
        _In_ Geometry g,
        _In_ Matrix m = Matrix::Identity(),
        _In_ bool p = false
        )
    {
        GeometryList.push_back(g);
        Matrices.push_back(m);
        Premultiplied.push_back(p);
    }

private:
    std::vector<Geometry> GeometryList;
    std::vector<Matrix> Matrices;
    std::vector<bool> Premultiplied;
};

class CheckerBoardMaterial final : public IrisPhysx::MaterialBase {
private:
    CheckerBoardMaterial(
        _In_ const IrisPhysx::Material & m0,
        _In_ const IrisPhysx::Material & m1
        )
    : M0(m0), M1(m1)
    { }

public:
    static
    IrisPhysx::Material
    Create(
        _In_ const IrisPhysx::Material & m0,
        _In_ const IrisPhysx::Material & m1
        )
    {
        std::unique_ptr<MaterialBase> Checkerboard(new CheckerBoardMaterial(m0, m1));
        return MaterialBase::Create(std::move(Checkerboard));
    }

    virtual
    std::tuple<IrisPhysx::BRDFReference, Iris::Vector>
    Sample(
        _In_ const Iris::Point & ModelHitPoint,
        _In_ const Iris::Vector & ModelShadingNormal,
        _In_ const Iris::Vector & WorldShadingNormal,
        _In_ PCVOID AdditionalData,
        _In_ const Iris::MatrixReference & ModelToWorld,
        _In_ IrisPhysx::BRDFAllocator Allocator
        ) const
    {
        FLOAT CheckerboardIndex;
        FLOAT ManhattanDistance;

        ManhattanDistance = FloorFloat(ModelHitPoint.Z()) + 
                            FloorFloat(ModelHitPoint.X());

        CheckerboardIndex = ModFloat(ManhattanDistance, (FLOAT) 2.0);

        if (IsZeroFloat(CheckerboardIndex) == FALSE)
        {
            return M0.Sample(ModelHitPoint, ModelShadingNormal, WorldShadingNormal, AdditionalData, ModelToWorld, Allocator);
        }
        
        return M1.Sample(ModelHitPoint, ModelShadingNormal, WorldShadingNormal, AdditionalData, ModelToWorld, Allocator);
    }

private:
    IrisPhysx::Material M0;
    IrisPhysx::Material M1;
};

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
        return ReflectorBase::Create(std::unique_ptr<ReflectorBase>(new ConstantRGBReflector(r, g, b)));
    }
    
    _Ret_
    virtual
    FLOAT
    Reflect(
        _In_ FLOAT Wavelength,
        _In_ FLOAT IncomingIntensity
        ) const
    {
        if (Wavelength == R_WAVELENGTH)
        {
            return R * IncomingIntensity;
        }
        
        if (Wavelength == G_WAVELENGTH)
        {
            return G * IncomingIntensity;
        }
        
        if (Wavelength == B_WAVELENGTH)
        {
            return B * IncomingIntensity;
        }
        
        throw std::runtime_error("Wavelength");
    }
    
private:
    FLOAT R, G, B;
};

class ConstantRGBSpectrum final : public SpectrumBase
{
private:
    ConstantRGBSpectrum(
        _In_ FLOAT r,
        _In_ FLOAT g,
        _In_ FLOAT b
        )
    : R(r), G(g), B(b)
    { }

public:
    static
    Spectrum
    Create(
        _In_ FLOAT r,
        _In_ FLOAT g,
        _In_ FLOAT b
        )
    {
        return SpectrumBase::Create(std::unique_ptr<SpectrumBase>(new ConstantRGBSpectrum(r, g, b)));
    }

    _Ret_
    virtual
    FLOAT
    Sample(
        _In_ FLOAT Wavelength
        ) const
    {
        if (Wavelength == R_WAVELENGTH)
        {
            return R;
        }

        if (Wavelength == G_WAVELENGTH)
        {
            return G;
        }

        if (Wavelength == B_WAVELENGTH)
        {
            return B;
        }

        throw std::runtime_error("Wavelength");
    }

private:
    FLOAT R, G, B;
};

TEST(PhysxRenderConstantRedWorldSphere)
{
    Material ConstantMaterial = PhongMaterial::Create(Color3(1.0f, 0.0f, 0.0f),
                                                      Color3::CreateBlack(),
                                                      Color3::CreateBlack(),
                                                      1.0f,
                                                      Color3::CreateBlack());

    Geometry SphereRadiusOne = Sphere::Create(Point(0.0f, 0.0f, 0.0f),
                                              1.0f,
                                              ConstantMaterial,
                                              ConstantMaterial);

    TestListScene Scene;
    Scene.AddGeometry(SphereRadiusOne);

    ProcessHitRoutine ProcessHitFunc = [](GeometryReference Geom,
                                          UINT32 FaceHit,
                                          Iris::MatrixReference ModelToWorld,
                                          PCVOID AdditionalData,
                                          const Iris::Vector & ModelViewer,
                                          const Iris::Point & ModelHitPoint,
                                          const Iris::Point & WorldHitPoint,
                                          const Iris::Ray & WorldRay,
                                          std::optional<LightListReference> Lights,
                                          std::optional<IrisPhysx::RayTracer> RayTracerPtr,
                                          VisibilityTester Tester,
                                          BRDFAllocator Allocator,
                                          IrisSpectrum::SpectrumCompositorReference SpectrumCompositor,
                                          IrisSpectrum::ReflectorCompositorReference ReflectorCompositor,
                                          IrisAdvanced::RandomReference Rng) -> SpectrumReference
    {
        Vector ModelSurfaceNormal = Geom.ComputeNormal(ModelHitPoint, FaceHit);
        Vector WorldSurfaceNormal = Vector::InverseTransposedMultiply(ModelToWorld, ModelSurfaceNormal);
        MaterialReference Mat = *(Geom.GetMaterial(FaceHit));
        auto MaterialResult = Mat.Sample(ModelHitPoint, ModelSurfaceNormal, WorldSurfaceNormal, AdditionalData, ModelToWorld, Allocator);
        auto Ref = std::get<0>(MaterialResult).Sample(WorldRay.Direction(), WorldSurfaceNormal, Rng, ReflectorCompositor);
        return SpectrumCompositor.Reflect(SpectrumReference(nullptr), std::get<0>(Ref));
    };

    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectra) -> Color3
    {
        return PhongToRGB(Spectra);
    };

    std::vector<Random> StrongRngReferences;

    CreateStateRoutine CreateState = [&](std::vector<RandomReference> & Rngs,
                                         std::vector<ProcessHitRoutine> & ProcessHits,
                                         std::vector<ToneMappingRoutine> & ToneMap,
                                         SIZE_T NumberOfThreads)
    {
        for (SIZE_T Index = 0; Index < NumberOfThreads; Index++)
        {
            StrongRngReferences.push_back(MultiplyWithCarry::Create());
            Rngs.push_back(StrongRngReferences[Index].AsRandomReference());
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
                          CreateState,
                          Fb);

    FramebufferIO::SaveAsPFM(Fb, "RenderConstantRedWorldSpherePlusPlus.pfm");
}

TEST(PhysxRenderConstantRedModelSphere)
{
    Material ConstantMaterial = PhongMaterial::Create(Color3(1.0f, 0.0f, 0.0f),
                                                      Color3::CreateBlack(),
                                                      Color3::CreateBlack(),
                                                      1.0f,
                                                      Color3::CreateBlack());

    Geometry SphereRadiusOne = Sphere::Create(Point(0.0f, 0.0f, 0.0f),
                                              1.0f,
                                              ConstantMaterial,
                                              ConstantMaterial);

    TestListScene Scene;
    Scene.AddGeometry(SphereRadiusOne, Matrix::Scalar(0.5f, 0.5f, 0.5f));

    ProcessHitRoutine ProcessHitFunc = [](GeometryReference Geom,
                                          UINT32 FaceHit,
                                          Iris::MatrixReference ModelToWorld,
                                          PCVOID AdditionalData,
                                          const Iris::Vector & ModelViewer,
                                          const Iris::Point & ModelHitPoint,
                                          const Iris::Point & WorldHitPoint,
                                          const Iris::Ray & WorldRay,
                                          std::optional<LightListReference> Lights,
                                          std::optional<IrisPhysx::RayTracer> RayTracerPtr,
                                          VisibilityTester Tester,
                                          BRDFAllocator Allocator,
                                          IrisSpectrum::SpectrumCompositorReference SpectrumCompositor,
                                          IrisSpectrum::ReflectorCompositorReference ReflectorCompositor,
                                          IrisAdvanced::RandomReference Rng) -> SpectrumReference
    {
        Vector ModelSurfaceNormal = Geom.ComputeNormal(ModelHitPoint, FaceHit);
        Vector WorldSurfaceNormal = Vector::InverseTransposedMultiply(ModelToWorld, ModelSurfaceNormal);
        MaterialReference Mat = *(Geom.GetMaterial(FaceHit));
        auto MaterialResult = Mat.Sample(ModelHitPoint, ModelSurfaceNormal, WorldSurfaceNormal, AdditionalData, ModelToWorld, Allocator);
        auto Ref = std::get<0>(MaterialResult).Sample(WorldRay.Direction(), WorldSurfaceNormal, Rng, ReflectorCompositor);
        return SpectrumCompositor.Reflect(SpectrumReference(nullptr), std::get<0>(Ref));
    };

    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectra) -> Color3
    {
        return PhongToRGB(Spectra);
    };

    std::vector<Random> StrongRngReferences;

    CreateStateRoutine CreateState = [&](std::vector<RandomReference> & Rngs,
                                         std::vector<ProcessHitRoutine> & ProcessHits,
                                         std::vector<ToneMappingRoutine> & ToneMap,
                                         SIZE_T NumberOfThreads)
    {
        for (SIZE_T Index = 0; Index < NumberOfThreads; Index++)
        {
            StrongRngReferences.push_back(MultiplyWithCarry::Create());
            Rngs.push_back(StrongRngReferences[Index].AsRandomReference());
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
                          CreateState,
                          Fb);

    FramebufferIO::SaveAsPFM(Fb, "RenderConstantRedModelSpherePlusPlus.pfm");
}

TEST(PhysxRenderConstantRedPremultipliedSphere)
{
    Material ConstantMaterial = PhongMaterial::Create(Color3(1.0f, 0.0f, 0.0f),
                                                      Color3::CreateBlack(),
                                                      Color3::CreateBlack(),
                                                      1.0f,
                                                      Color3::CreateBlack());

    Geometry SphereRadiusOne = Sphere::Create(Point(0.0f, 0.0f, 0.0f),
                                              1.0f,
                                              ConstantMaterial,
                                              ConstantMaterial);

    TestListScene Scene;
    Scene.AddGeometry(SphereRadiusOne, Matrix::Scalar(1.0f, 1.0f, 1.0f), true);

    ProcessHitRoutine ProcessHitFunc = [](GeometryReference Geom,
                                          UINT32 FaceHit,
                                          Iris::MatrixReference ModelToWorld,
                                          PCVOID AdditionalData,
                                          const Iris::Vector & ModelViewer,
                                          const Iris::Point & ModelHitPoint,
                                          const Iris::Point & WorldHitPoint,
                                          const Iris::Ray & WorldRay,
                                          std::optional<LightListReference> Lights,
                                          std::optional<IrisPhysx::RayTracer> RayTracerPtr,
                                          VisibilityTester Tester,
                                          BRDFAllocator Allocator,
                                          IrisSpectrum::SpectrumCompositorReference SpectrumCompositor,
                                          IrisSpectrum::ReflectorCompositorReference ReflectorCompositor,
                                          IrisAdvanced::RandomReference Rng) -> SpectrumReference
    {
        Vector ModelSurfaceNormal = Geom.ComputeNormal(ModelHitPoint, FaceHit);
        Vector WorldSurfaceNormal = Vector::InverseTransposedMultiply(ModelToWorld, ModelSurfaceNormal);
        MaterialReference Mat = *(Geom.GetMaterial(FaceHit));
        auto MaterialResult = Mat.Sample(ModelHitPoint, ModelSurfaceNormal, WorldSurfaceNormal, AdditionalData, ModelToWorld, Allocator);
        auto Ref = std::get<0>(MaterialResult).Sample(WorldRay.Direction(), WorldSurfaceNormal, Rng, ReflectorCompositor);
        return SpectrumCompositor.Reflect(SpectrumReference(nullptr), std::get<0>(Ref));
    };

    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectra) -> Color3
    {
        return PhongToRGB(Spectra);
    };

    std::vector<Random> StrongRngReferences;

    CreateStateRoutine CreateState = [&](std::vector<RandomReference> & Rngs,
                                         std::vector<ProcessHitRoutine> & ProcessHits,
                                         std::vector<ToneMappingRoutine> & ToneMap,
                                         SIZE_T NumberOfThreads)
    {
        for (SIZE_T Index = 0; Index < NumberOfThreads; Index++)
        {
            StrongRngReferences.push_back(MultiplyWithCarry::Create());
            Rngs.push_back(StrongRngReferences[Index].AsRandomReference());
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
                          CreateState,
                          Fb);

    FramebufferIO::SaveAsPFM(Fb, "RenderConstantRedPremultipliedSpherePlusPlus.pfm");
}

TEST(PhysxRenderPerfectSpecularSphere)
{
    TestListScene Scene;

    Material ConstantMaterial = PhongMaterial::Create(Color3(1.0f, 0.0f, 0.0f),
                                                      Color3::CreateBlack(),
                                                      Color3::CreateBlack(),
                                                      1.0f,
                                                      Color3::CreateBlack());

    Geometry SphereOne = Sphere::Create(Point(1.0f, 0.0f, -1.0f),
                                        1.0f,
                                        ConstantMaterial,
                                        ConstantMaterial);

    Scene.AddGeometry(SphereOne);

    Material RefletiveMaterial = PhongMaterial::Create(Color3::CreateBlack(),
                                                       Color3::CreateBlack(),
                                                       Color3::CreateBlack(),
                                                       1.0f,
                                                       Color3(1.0f, 1.0f, 1.0f));

    Geometry SphereTwo = Sphere::Create(Point(-1.0f, 0.0f, -1.0f),
                                        1.0f,
                                        RefletiveMaterial,
                                        RefletiveMaterial);

    Scene.AddGeometry(SphereTwo);

    ProcessHitRoutine ProcessHitFunc = [&](GeometryReference Geom,
                                           UINT32 FaceHit,
                                           Iris::MatrixReference ModelToWorld,
                                           PCVOID AdditionalData,
                                           const Iris::Vector & ModelViewer,
                                           const Iris::Point & ModelHitPoint,
                                           const Iris::Point & WorldHitPoint,
                                           const Iris::Ray & WorldRay,
                                           std::optional<LightListReference> Lights,
                                           std::optional<IrisPhysx::RayTracer> RayTracerPtr,
                                           VisibilityTester Tester,
                                           BRDFAllocator Allocator,
                                           IrisSpectrum::SpectrumCompositorReference SpectrumCompositor,
                                           IrisSpectrum::ReflectorCompositorReference ReflectorCompositor,
                                           IrisAdvanced::RandomReference Rng) -> SpectrumReference
    {
        Vector ModelSurfaceNormal = Geom.ComputeNormal(ModelHitPoint, FaceHit);
        Vector WorldSurfaceNormal = Vector::InverseTransposedMultiply(ModelToWorld, ModelSurfaceNormal);
        MaterialReference Mat = *(Geom.GetMaterial(FaceHit));
        auto MaterialResult = Mat.Sample(ModelHitPoint, ModelSurfaceNormal, WorldSurfaceNormal, AdditionalData, ModelToWorld, Allocator);
        auto Ref = std::get<0>(MaterialResult).Sample(WorldRay.Direction(), WorldSurfaceNormal, Rng, ReflectorCompositor);
        
        SpectrumReference ReflectedLight(nullptr);

        if (std::get<2>(Ref) > 0.0 && RayTracerPtr)
        {
            ReflectedLight = RayTracerPtr->TraceClosestHit(Ray(WorldHitPoint, std::get<1>(Ref)), ProcessHitFunc);
        }

        if (std::get<2>(Ref) > 0.0 && RayTracerPtr)
        {
            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(RayTracerPtr->TraceClosestHit(Ray(WorldHitPoint, std::get<1>(Ref)), ProcessHitFunc), std::get<0>(Ref)));
        }
        else
        {
            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(SpectrumReference(nullptr), std::get<0>(Ref)));
        }

        return ReflectedLight;
    };

    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectra) -> Color3
    {
        return PhongToRGB(Spectra);
    };

    std::vector<Random> StrongRngReferences;

    CreateStateRoutine CreateState = [&](std::vector<RandomReference> & Rngs,
                                         std::vector<ProcessHitRoutine> & ProcessHits,
                                         std::vector<ToneMappingRoutine> & ToneMap,
                                         SIZE_T NumberOfThreads)
    {
        for (SIZE_T Index = 0; Index < NumberOfThreads; Index++)
        {
            StrongRngReferences.push_back(MultiplyWithCarry::Create());
            Rngs.push_back(StrongRngReferences[Index].AsRandomReference());
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
                          1,
                          false,
                          false,
                          Scene.GetTestRoutine(),
                          CreateState,
                          Fb);

    FramebufferIO::SaveAsPFM(Fb, "RenderPerfectSpecularWorldSpherePlusPlus.pfm");
}

TEST(PhysxRenderConstantRedWorldTriangle)
{
    TestListScene Scene;

    Material ConstantMaterial = PhongMaterial::Create(Color3(1.0f, 0.0f, 0.0f),
                                                      Color3::CreateBlack(),
                                                      Color3::CreateBlack(),
                                                      1.0f,
                                                      Color3::CreateBlack());

    Geometry SphereOne = Triangle::Create(Point(0.0f, 0.0f, 0.0f),
                                          Point(0.0f, 1.5f, 0.0f),
                                          Point(1.5f, 0.0f, 0.0f),
                                          ConstantMaterial,
                                          ConstantMaterial);

    Scene.AddGeometry(SphereOne);

    ProcessHitRoutine ProcessHitFunc = [&](GeometryReference Geom,
                                           UINT32 FaceHit,
                                           Iris::MatrixReference ModelToWorld,
                                           PCVOID AdditionalData,
                                           const Iris::Vector & ModelViewer,
                                           const Iris::Point & ModelHitPoint,
                                           const Iris::Point & WorldHitPoint,
                                           const Iris::Ray & WorldRay,
                                           std::optional<LightListReference> Lights,
                                           std::optional<IrisPhysx::RayTracer> RayTracerPtr,
                                           VisibilityTester Tester,
                                           BRDFAllocator Allocator,
                                           IrisSpectrum::SpectrumCompositorReference SpectrumCompositor,
                                           IrisSpectrum::ReflectorCompositorReference ReflectorCompositor,
                                           IrisAdvanced::RandomReference Rng) -> SpectrumReference
    {
        Vector ModelSurfaceNormal = Geom.ComputeNormal(ModelHitPoint, FaceHit);
        Vector WorldSurfaceNormal = Vector::InverseTransposedMultiply(ModelToWorld, ModelSurfaceNormal);
        MaterialReference Mat = *(Geom.GetMaterial(FaceHit));
        auto MaterialResult = Mat.Sample(ModelHitPoint, ModelSurfaceNormal, WorldSurfaceNormal, AdditionalData, ModelToWorld, Allocator);
        auto Ref = std::get<0>(MaterialResult).Sample(WorldRay.Direction(), WorldSurfaceNormal, Rng, ReflectorCompositor);
        
        SpectrumReference ReflectedLight(nullptr);

        if (std::get<2>(Ref) > 0.0 && RayTracerPtr)
        {
            ReflectedLight = RayTracerPtr->TraceClosestHit(Ray(WorldHitPoint, std::get<1>(Ref)), ProcessHitFunc);
        }

        if (std::get<2>(Ref) > 0.0 && RayTracerPtr)
        {
            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(RayTracerPtr->TraceClosestHit(Ray(WorldHitPoint, std::get<1>(Ref)), ProcessHitFunc), std::get<0>(Ref)));
        }
        else
        {
            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(SpectrumReference(nullptr), std::get<0>(Ref)));
        }

        return ReflectedLight;
    };

    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectra) -> Color3
    {
        return PhongToRGB(Spectra);
    };

    std::vector<Random> StrongRngReferences;

    CreateStateRoutine CreateState = [&](std::vector<RandomReference> & Rngs,
                                         std::vector<ProcessHitRoutine> & ProcessHits,
                                         std::vector<ToneMappingRoutine> & ToneMap,
                                         SIZE_T NumberOfThreads)
    {
        for (SIZE_T Index = 0; Index < NumberOfThreads; Index++)
        {
            StrongRngReferences.push_back(MultiplyWithCarry::Create());
            Rngs.push_back(StrongRngReferences[Index].AsRandomReference());
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
                          1,
                          false,
                          false,
                          Scene.GetTestRoutine(),
                          CreateState,
                          Fb);

    FramebufferIO::SaveAsPFM(Fb, "RenderConstantRedWorldTrianglePlusPlus.pfm");
}

TEST(PhysxRenderInterpolatedRedWorldTriangle)
{
    TestListScene Scene;

    Material ConstantMaterial = TriangleInterpolatedPhongMaterial::Create(Color3(1.0f, 0.0f, 0.0f),
                                                                          Color3::CreateBlack(),
                                                                          Color3::CreateBlack(),
                                                                          1.0f,
                                                                          Color3::CreateBlack(),
                                                                          Color3(0.0f, 1.0f, 0.0f),
                                                                          Color3::CreateBlack(),
                                                                          Color3::CreateBlack(),
                                                                          1.0f,
                                                                          Color3::CreateBlack(),
                                                                          Color3(0.0f, 0.0f, 1.0f),
                                                                          Color3::CreateBlack(),
                                                                          Color3::CreateBlack(),
                                                                          1.0f,
                                                                          Color3::CreateBlack());

    Geometry SphereOne = Triangle::Create(Point(0.0f, 0.0f, 0.0f),
                                          Point(0.0f, 1.5f, 0.0f),
                                          Point(1.5f, 0.0f, 0.0f),
                                          ConstantMaterial,
                                          ConstantMaterial);

    Scene.AddGeometry(SphereOne);

    ProcessHitRoutine ProcessHitFunc = [&](GeometryReference Geom,
                                           UINT32 FaceHit,
                                           Iris::MatrixReference ModelToWorld,
                                           PCVOID AdditionalData,
                                           const Iris::Vector & ModelViewer,
                                           const Iris::Point & ModelHitPoint,
                                           const Iris::Point & WorldHitPoint,
                                           const Iris::Ray & WorldRay,
                                           std::optional<LightListReference> Lights,
                                           std::optional<IrisPhysx::RayTracer> RayTracerPtr,
                                           VisibilityTester Tester,
                                           BRDFAllocator Allocator,
                                           IrisSpectrum::SpectrumCompositorReference SpectrumCompositor,
                                           IrisSpectrum::ReflectorCompositorReference ReflectorCompositor,
                                           IrisAdvanced::RandomReference Rng) -> SpectrumReference
    {
        Vector ModelSurfaceNormal = Geom.ComputeNormal(ModelHitPoint, FaceHit);
        Vector WorldSurfaceNormal = Vector::InverseTransposedMultiply(ModelToWorld, ModelSurfaceNormal);
        MaterialReference Mat = *(Geom.GetMaterial(FaceHit));
        auto MaterialResult = Mat.Sample(ModelHitPoint, ModelSurfaceNormal, WorldSurfaceNormal, AdditionalData, ModelToWorld, Allocator);
        auto Ref = std::get<0>(MaterialResult).Sample(WorldRay.Direction(), WorldSurfaceNormal, Rng, ReflectorCompositor);
        
        SpectrumReference ReflectedLight(nullptr);

        if (std::get<2>(Ref) > 0.0 && RayTracerPtr)
        {
            ReflectedLight = RayTracerPtr->TraceClosestHit(Ray(WorldHitPoint, std::get<1>(Ref)), ProcessHitFunc);
        }

        if (std::get<2>(Ref) > 0.0 && RayTracerPtr)
        {
            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(RayTracerPtr->TraceClosestHit(Ray(WorldHitPoint, std::get<1>(Ref)), ProcessHitFunc), std::get<0>(Ref)));
        }
        else
        {
            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(SpectrumReference(nullptr), std::get<0>(Ref)));
        }

        return ReflectedLight;
    };

    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectra) -> Color3
    {
        return PhongToRGB(Spectra);
    };

    std::vector<Random> StrongRngReferences;

    CreateStateRoutine CreateState = [&](std::vector<RandomReference> & Rngs,
                                         std::vector<ProcessHitRoutine> & ProcessHits,
                                         std::vector<ToneMappingRoutine> & ToneMap,
                                         SIZE_T NumberOfThreads)
    {
        for (SIZE_T Index = 0; Index < NumberOfThreads; Index++)
        {
            StrongRngReferences.push_back(MultiplyWithCarry::Create());
            Rngs.push_back(StrongRngReferences[Index].AsRandomReference());
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
                          1,
                          false,
                          false,
                          Scene.GetTestRoutine(),
                          CreateState,
                          Fb);

    FramebufferIO::SaveAsPFM(Fb, "RenderInterpolatedRedWorldTrianglePlusPlus.pfm");
}

TEST(PhysxRenderPhongWorldSphere)
{
    Material ConstantMaterial = PhongMaterial::Create(Color3::CreateBlack(),
                                                      Color3(0.8f, 0.8f, 0.8f),
                                                      Color3::CreateBlack(),
                                                      1.0f,
                                                      Color3::CreateBlack());

    Geometry SphereRadiusOne = Sphere::Create(Point(0.0f, 0.0f, 0.0f),
                                              1.0f,
                                              ConstantMaterial,
                                              ConstantMaterial);

    TestListScene Scene;
    Scene.AddGeometry(SphereRadiusOne);

    LightList Lights = LightList::Create();
    Lights.Add(PhongPointLight::Create(Color3(1.0f, 1.0f, 1.0f),
                                       Color3::CreateBlack(),
                                       Point(0.0f, 0.0f, 1000.0f)));

    ProcessHitRoutine ProcessHitFunc = [&](GeometryReference Geom,
                                           UINT32 FaceHit,
                                           Iris::MatrixReference ModelToWorld,
                                           PCVOID AdditionalData,
                                           const Iris::Vector & ModelViewer,
                                           const Iris::Point & ModelHitPoint,
                                           const Iris::Point & WorldHitPoint,
                                           const Iris::Ray & WorldRay,
                                           std::optional<LightListReference> Lights,
                                           std::optional<IrisPhysx::RayTracer> RayTracerPtr,
                                           VisibilityTester Tester,
                                           BRDFAllocator Allocator,
                                           IrisSpectrum::SpectrumCompositorReference SpectrumCompositor,
                                           IrisSpectrum::ReflectorCompositorReference ReflectorCompositor,
                                           IrisAdvanced::RandomReference Rng) -> SpectrumReference
    {
        Vector ModelSurfaceNormal = Geom.ComputeNormal(ModelHitPoint, FaceHit);
        Vector WorldSurfaceNormal = Vector::InverseTransposedMultiply(ModelToWorld, ModelSurfaceNormal);
        MaterialReference Mat = *(Geom.GetMaterial(FaceHit));
        BRDFReference Brdf = std::get<0>(Mat.Sample(ModelHitPoint, ModelSurfaceNormal, WorldSurfaceNormal, AdditionalData, ModelToWorld, Allocator));

        SpectrumReference ReflectedLight(nullptr);

        for (SIZE_T Index = 0; Index < Lights->Size(); Index++)
        {
            LightReference Light = Lights->Get(Index);

            auto Result = Light.Sample(WorldHitPoint,
                                       Tester,
                                       Rng,
                                       SpectrumCompositor);

            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(std::get<0>(Result), Brdf.ComputeReflectance(WorldRay.Direction(), WorldSurfaceNormal, std::get<1>(Result), ReflectorCompositor)));
        }

        auto Ref = Brdf.Sample(WorldRay.Direction(), WorldSurfaceNormal, Rng, ReflectorCompositor);

        if (std::get<2>(Ref) > 0.0 && RayTracerPtr)
        {
            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(RayTracerPtr->TraceClosestHit(Ray(WorldHitPoint, std::get<1>(Ref)), ProcessHitFunc), std::get<0>(Ref)));
        }
        else
        {
            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(SpectrumReference(nullptr), std::get<0>(Ref)));
        }

        return ReflectedLight;
    };

    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectra) -> Color3
    {
        return PhongToRGB(Spectra);
    };

    std::vector<Random> StrongRngReferences;

    CreateStateRoutine CreateState = [&](std::vector<RandomReference> & Rngs,
                                         std::vector<ProcessHitRoutine> & ProcessHits,
                                         std::vector<ToneMappingRoutine> & ToneMap,
                                         SIZE_T NumberOfThreads)
    {
        for (SIZE_T Index = 0; Index < NumberOfThreads; Index++)
        {
            StrongRngReferences.push_back(MultiplyWithCarry::Create());
            Rngs.push_back(StrongRngReferences[Index].AsRandomReference());
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
                          Lights,
                          CreateState,
                          Fb);

    FramebufferIO::SaveAsPFM(Fb, "RenderPhongWorldSpherePlusPlus.pfm");
}

TEST(PhysxRenderMirrorPhongCheckerboardSpheres)
{
    Material SphereMaterial = PhongMaterial::Create(Color3::CreateBlack(),
                                                    Color3(1.0f, 1.0f, 1.0f),
                                                    Color3(0.5f, 0.5f, 0.5f),
                                                    55.0f,
                                                    Color3(0.6f, 0.6f, 0.6f));

    Material PlaneBlack = PhongMaterial::Create(Color3::CreateBlack(),
                                                Color3::CreateBlack(),
                                                Color3::CreateBlack(),
                                                150.0f,
                                                Color3::CreateBlack());

    Material PlaneWhite = PhongMaterial::Create(Color3::CreateBlack(),
                                                Color3(1.0f, 1.0f, 1.0f),
                                                Color3::CreateBlack(),
                                                150.0f,
                                                Color3(0.7f, 0.7f, 0.7f));

    Material Checkerboard = CheckerBoardMaterial::Create(PlaneWhite, PlaneBlack);

    Geometry SphereOne = Sphere::Create(Point(0.0f, 1.0f, 0.0f),
                                        1.0f,
                                        SphereMaterial,
                                        SphereMaterial);

    Geometry SphereTwo = Sphere::Create(Point(-1.0f, 0.5f, 1.5f),
                                        0.5f,
                                        SphereMaterial,
                                        SphereMaterial);

    Geometry InfinitePlane = InfinitePlane::Create(Point(0.0f, 0.0f, 0.0f),
                                                   Vector(0.0f, 1.0f, 0.0f),
                                                   Checkerboard,
                                                   Checkerboard);

    TestListScene Scene;
    Scene.AddGeometry(SphereOne);
    Scene.AddGeometry(SphereTwo);
    Scene.AddGeometry(InfinitePlane);

    LightList Lights = LightList::Create();

    Lights.Add(PhongPointLight::Create(Color3(0.49f, 0.07f, 0.07f),
                                       Color3(0.49f, 0.07f, 0.07f),
                                       Point(-2.0f, 2.5f, 0.0f)));

    Lights.Add(PhongPointLight::Create(Color3(0.07f, 0.07f, 0.49f),
                                       Color3(0.07f, 0.07f, 0.49f),
                                       Point(1.5f, 2.5f, 1.5f)));

    Lights.Add(PhongPointLight::Create(Color3(0.07f, 0.49f, 0.07f),
                                       Color3(0.07f, 0.49f, 0.07f),
                                       Point(1.5f, 2.5f, -1.5f)));

    Lights.Add(PhongPointLight::Create(Color3(0.21f, 0.21f, 0.35f),
                                       Color3(0.21f, 0.21f, 0.35f),
                                       Point(0.0f,3.5f, 0.0f)));

    ProcessHitRoutine ProcessHitFunc = [&](GeometryReference Geom,
                                           UINT32 FaceHit,
                                           Iris::MatrixReference ModelToWorld,
                                           PCVOID AdditionalData,
                                           const Iris::Vector & ModelViewer,
                                           const Iris::Point & ModelHitPoint,
                                           const Iris::Point & WorldHitPoint,
                                           const Iris::Ray & WorldRay,
                                           std::optional<LightListReference> Lights,
                                           std::optional<IrisPhysx::RayTracer> RayTracerPtr,
                                           VisibilityTester Tester,
                                           BRDFAllocator Allocator,
                                           IrisSpectrum::SpectrumCompositorReference SpectrumCompositor,
                                           IrisSpectrum::ReflectorCompositorReference ReflectorCompositor,
                                           IrisAdvanced::RandomReference Rng) -> SpectrumReference
    {
        Vector ModelSurfaceNormal = Geom.ComputeNormal(ModelHitPoint, FaceHit);
        Vector WorldSurfaceNormal = Vector::InverseTransposedMultiply(ModelToWorld, ModelSurfaceNormal);
        MaterialReference Mat = *(Geom.GetMaterial(FaceHit));
        BRDFReference Brdf = std::get<0>(Mat.Sample(ModelHitPoint, ModelSurfaceNormal, WorldSurfaceNormal, AdditionalData, ModelToWorld, Allocator));

        SpectrumReference ReflectedLight(nullptr);

        for (SIZE_T Index = 0; Index < Lights->Size(); Index++)
        {
            LightReference Light = Lights->Get(Index);

            auto Result = Light.Sample(WorldHitPoint,
                                       Tester,
                                       Rng,
                                       SpectrumCompositor);

            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(std::get<0>(Result), Brdf.ComputeReflectance(WorldRay.Direction(), WorldSurfaceNormal, std::get<1>(Result), ReflectorCompositor)));
        }

        auto Ref = Brdf.Sample(WorldRay.Direction(), WorldSurfaceNormal, Rng, ReflectorCompositor);

        if (std::get<2>(Ref) > 0.0 && RayTracerPtr)
        {
            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(RayTracerPtr->TraceClosestHit(Ray(WorldHitPoint, std::get<1>(Ref)), ProcessHitFunc), std::get<0>(Ref)));
        }
        else
        {
            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(SpectrumReference(nullptr), std::get<0>(Ref)));
        }

        return ReflectedLight;
    };

    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectra) -> Color3
    {
        return PhongToRGB(Spectra);
    };

    std::vector<Random> StrongRngReferences;

    CreateStateRoutine CreateState = [&](std::vector<RandomReference> & Rngs,
                                         std::vector<ProcessHitRoutine> & ProcessHits,
                                         std::vector<ToneMappingRoutine> & ToneMap,
                                         SIZE_T NumberOfThreads)
    {
        for (SIZE_T Index = 0; Index < NumberOfThreads; Index++)
        {
            StrongRngReferences.push_back(MultiplyWithCarry::Create());
            Rngs.push_back(StrongRngReferences[Index].AsRandomReference());
            ProcessHits.push_back(ProcessHitFunc);
            ToneMap.push_back(ToneMappingFunc);
        }
    };

    Point PinholeLocation(3.0f, 2.0f, 4.0f);
    Point LookAt(-1.0f, 0.45f, 0.0f);
    Vector CameraDirection = LookAt - PinholeLocation;

    Framebuffer Fb = Framebuffer::Create(Color3(0.0f, 0.0f, 0.0f), 500, 500);
    PinholeCamera::Render(PinholeLocation,
                          1.0f,
                          1.0f,
                          1.0f,
                          CameraDirection,
                          Vector(0.0f, 1.0f, 0.0f),
                          0,
                          0,
                          0.01f,
                          5,
                          false,
                          false,
                          Scene.GetTestRoutine(),
                          Lights,
                          CreateState,
                          Fb);

    FramebufferIO::SaveAsPFM(Fb, "RenderMirrorPhongCheckerboardSpheresPlusPlus.pfm");
}

void
CreateQuad(
    _In_ TestListScene & Scene,
    _In_ const Point & Vertex0,
    _In_ const Point & Vertex1,
    _In_ const Point & Vertex2,
    _In_ const Point & Vertex3,
    _In_ Material & FrontMaterial,
    _In_ Material & BackMaterial
    )
{
    Geometry Triangle0 = Triangle::Create(Vertex0,
                                          Vertex1,
                                          Vertex2,
                                          FrontMaterial,
                                          BackMaterial);

    Scene.AddGeometry(Triangle0);

    Geometry Triangle1 = Triangle::Create(Vertex0,
                                          Vertex2,
                                          Vertex3,
                                          FrontMaterial,
                                          BackMaterial);

    Scene.AddGeometry(Triangle1);
}

TEST(PhysxRenderCornellBox)
{
    AreaLightBuilder Builder = AreaLightBuilder::Create();
    TestListScene Scene;
    
    //
    // Light
    //
    
    SIZE_T Triangle0Index = Triangle::Create(Builder,
                                             Point(303.0f, 549.9f, 247.0f),
                                             Point(303.0f, 549.9f, 303.0f),
                                             Point(247.0f, 549.9f, 303.0f),
                                             std::nullopt,
                                             std::nullopt); 

    SIZE_T Triangle1Index = Triangle::Create(Builder,
                                             Point(303.0f, 549.9f, 247.0f),
                                             Point(247.0f, 549.9f, 303.0f),
                                             Point(247.0f, 549.9f, 247.0f),
                                             std::nullopt,
                                             std::nullopt);
    
    Spectrum LightSpectrum = ConstantRGBSpectrum::Create(12.0f, 12.0f, 12.0f);
    SIZE_T LightIndex = ConstantAreaLight::Create(Builder, LightSpectrum);

    Builder.AttachLightToGeometry(Triangle0Index,
                                  PHYSX_TRIANGLE_BACK_FACE,
                                  LightIndex);

    Builder.AttachLightToGeometry(Triangle1Index,
                                  PHYSX_TRIANGLE_BACK_FACE,
                                  LightIndex);
    
    auto GeometryAndLights = Builder.Build();

    LightList Lights = LightList::Create();

    for (Light & light : std::get<1>(GeometryAndLights))
    {
        Lights.Add(light);
    }

    for (Geometry & geometry : std::get<0>(GeometryAndLights))
    {
        Scene.AddGeometry(geometry);
    }

    //
    // Back Wall, Floor, and Ceiling
    //
    
    Reflector WhiteSurfaceReflector = ConstantRGBReflector::Create(0.75f, 0.75f, 0.75f);
    Material WhiteSurfaceMaterial = LambertianMaterial::Create(WhiteSurfaceReflector);
    
    CreateQuad(Scene,
               Point(550.0f, 0.0f, 560.0f),
               Point(0.0f, 0.0f, 560.0f),
               Point(0.0f, 550.0f, 560.0f),
               Point(560.0f, 550.0f, 560.0f),
               WhiteSurfaceMaterial,
               WhiteSurfaceMaterial);
               
    CreateQuad(Scene,
               Point(560.0f, 550.0f, 0.0f),
               Point(560.0f, 550.0f, 560.0f),
               Point(0.0f, 550.0f, 560.0f),
               Point(0.0f, 550.0f, 0.0f),
               WhiteSurfaceMaterial,
               WhiteSurfaceMaterial);
               
    CreateQuad(Scene,
               Point(550.0f, 0.0f, 0.0f),
               Point(0.0f, 0.0f, 0.0f),
               Point(0.0f, 0.0f, 560.0f),
               Point(550.0f, 0.0f, 560.0f),
               WhiteSurfaceMaterial,
               WhiteSurfaceMaterial);
    
    //
    // Red Wall
    //
    
    Reflector RedSurfaceReflector = ConstantRGBReflector::Create(0.75f, 0.25f, 0.25f);
    Material RedSurfaceMaterial = LambertianMaterial::Create(RedSurfaceReflector);
    
    CreateQuad(Scene,
               Point(550.0f, 0.0f, 0.0f),
               Point(550.0f, 0.0f, 560.0f),
               Point(560.0f, 550.0f, 560.0f),
               Point(560.0f, 550.0f, 0.0f),
               RedSurfaceMaterial,
               RedSurfaceMaterial);

    //
    // Blue Wall
    //

    Reflector BlueSurfaceReflector = ConstantRGBReflector::Create(0.25f, 0.25f, 0.75f);
    Material BlueSurfaceMaterial = LambertianMaterial::Create(BlueSurfaceReflector);
    
    CreateQuad(Scene,
               Point(0.0f, 0.0f, 560.0f),
               Point(0.0f, 0.0f, 0.0f), 
               Point(0.0f, 550.0f, 0.0f), 
               Point(0.0f, 550.0f, 560.0f),
               BlueSurfaceMaterial,
               BlueSurfaceMaterial);

    //
    // Tall Box
    //

    CreateQuad(Scene,
               Point(423.0f, 330.0f, 247.0f),
               Point(265.0f, 330.0f, 296.0f),
               Point(314.0f, 330.0f, 456.0f), 
               Point(472.0f, 330.0f, 406.0f),
               WhiteSurfaceMaterial,
               WhiteSurfaceMaterial);

    CreateQuad(Scene,
               Point(423.0f, 0.0f, 247.0f),
               Point(423.0f, 330.0f, 247.0f),
               Point(472.0f, 330.0f, 406.0f),
               Point(472.0f, 0.0f, 406.0f),
               WhiteSurfaceMaterial,
               WhiteSurfaceMaterial);

    CreateQuad(Scene,
               Point(472.0f, 0.0f, 406.0f),
               Point(472.0f, 330.0f, 406.0f),
               Point(314.0f, 330.0f, 456.0f),
               Point(314.0f, 0.0f, 456.0f),
               WhiteSurfaceMaterial,
               WhiteSurfaceMaterial);

    CreateQuad(Scene,
               Point(314.0f, 0.0f, 456.0f),
               Point(314.0f, 330.0f, 456.0f), 
               Point(265.0f, 330.0f, 296.0f), 
               Point(265.0f, 0.0f, 296.0f),
               WhiteSurfaceMaterial,
               WhiteSurfaceMaterial);

    CreateQuad(Scene,
               Point(265.0f, 0.0f, 296.0f),
               Point(265.0f, 330.0f, 296.0f),
               Point(423.0f, 330.0f, 247.0f),
               Point(423.0f, 0.0f, 247.0f),
               WhiteSurfaceMaterial,
               WhiteSurfaceMaterial);

    //
    // Short Box
    //

    CreateQuad(Scene,
               Point(130.0f, 165.0f, 65.0f),
               Point(82.0f, 165.0f, 225.0f), 
               Point(240.0f, 165.0f, 272.0f), 
               Point(290.0f, 165.0f, 114.0f),
               WhiteSurfaceMaterial,
               WhiteSurfaceMaterial);

    CreateQuad(Scene,
               Point(290.0f, 0.0f, 114.0f),
               Point(290.0f, 165.0f, 114.0f),
               Point(240.0f, 165.0f, 272.0f),
               Point(240.0f, 0.0f, 272.0f),
               WhiteSurfaceMaterial,
               WhiteSurfaceMaterial);

    CreateQuad(Scene,
               Point(130.0f, 0.0f, 65.0f), 
               Point(130.0f, 165.0f, 65.0f),
               Point(290.0f, 165.0f, 114.0f),
               Point(290.0f, 0.0f, 114.0f),
               WhiteSurfaceMaterial,
               WhiteSurfaceMaterial);

    CreateQuad(Scene,
               Point(82.0f, 0.0f, 225.0f),
               Point(82.0f, 165.0f, 225.0f),
               Point(130.0f, 165.0f, 65.0f),
               Point(130.0f, 0.0f, 65.0f),
               WhiteSurfaceMaterial,
               WhiteSurfaceMaterial);

    CreateQuad(Scene,
               Point(240.0f, 0.0f, 272.0f), 
               Point(240.0f, 165.0f, 272.0f),
               Point(82.0f, 165.0f, 225.0f),
               Point(82.0f, 0.0f, 225.0f),
               WhiteSurfaceMaterial,
               WhiteSurfaceMaterial);
    
    //
    // Render
    //
    
    ProcessHitRoutine ProcessHitFunc = [&](GeometryReference Geom,
                                           UINT32 FaceHit,
                                           Iris::MatrixReference ModelToWorld,
                                           PCVOID AdditionalData,
                                           const Iris::Vector & ModelViewer,
                                           const Iris::Point & ModelHitPoint,
                                           const Iris::Point & WorldHitPoint,
                                           const Iris::Ray & WorldRay,
                                           std::optional<LightListReference> Lights,
                                           std::optional<IrisPhysx::RayTracer> RayTracerPtr,
                                           VisibilityTester Tester,
                                           BRDFAllocator Allocator,
                                           IrisSpectrum::SpectrumCompositorReference SpectrumCompositor,
                                           IrisSpectrum::ReflectorCompositorReference ReflectorCompositor,
                                           IrisAdvanced::RandomReference Rng) -> SpectrumReference
    {
        Vector ModelSurfaceNormal = Geom.ComputeNormal(ModelHitPoint, FaceHit);
        Vector WorldSurfaceNormal = Vector::InverseTransposedMultiply(ModelToWorld, ModelSurfaceNormal);

        SpectrumReference ReflectedLight(nullptr);

        auto OptionalLight = Geom.GetLight(FaceHit);

        if (OptionalLight)
        {
            ReflectedLight = OptionalLight->ComputeEmissive(WorldRay, Tester, SpectrumCompositor);
        }

        auto OptionalMaterial = Geom.GetMaterial(FaceHit);

        if (!OptionalMaterial)
        {
            return ReflectedLight;
        }

        MaterialReference Mat = *OptionalMaterial;
        BRDFReference Brdf = std::get<0>(Mat.Sample(ModelHitPoint, ModelSurfaceNormal, WorldSurfaceNormal, AdditionalData, ModelToWorld, Allocator));
        
        for (SIZE_T Index = 0; Index < Lights->Size(); Index++)
        {
            LightReference Light = Lights->Get(Index);
            
            auto Result = Light.Sample(WorldHitPoint,
                                       Tester,
                                       Rng,
                                       SpectrumCompositor);
            
            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(std::get<0>(Result), Brdf.ComputeReflectance(WorldRay.Direction(), WorldSurfaceNormal, std::get<1>(Result), ReflectorCompositor)));
        }
        
        auto Ref = Brdf.Sample(WorldRay.Direction(), WorldSurfaceNormal, Rng, ReflectorCompositor);
        
        if (std::get<2>(Ref) > 0.0 && RayTracerPtr)
        {
            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(RayTracerPtr->TraceClosestHit(Ray(WorldHitPoint, std::get<1>(Ref)), ProcessHitFunc), std::get<0>(Ref)));
        }
        else
        {
            ReflectedLight = SpectrumCompositor.Add(ReflectedLight, SpectrumCompositor.Reflect(SpectrumReference(nullptr), std::get<0>(Ref)));
        }
        
        return ReflectedLight;
    };
    
    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectra) -> Color3
    {
        return Color3(Spectra.Sample(R_WAVELENGTH), Spectra.Sample(G_WAVELENGTH), Spectra.Sample(B_WAVELENGTH));
    };
    
    std::vector<Random> StrongRngReferences;
    
    CreateStateRoutine CreateState = [&](std::vector<RandomReference> & Rngs,
                                         std::vector<ProcessHitRoutine> & ProcessHits,
                                         std::vector<ToneMappingRoutine> & ToneMap,
                                         SIZE_T NumberOfThreads)
    {
        for (SIZE_T Index = 0; Index < NumberOfThreads; Index++)
        {
            StrongRngReferences.push_back(MultiplyWithCarry::Create());
            Rngs.push_back(StrongRngReferences[Index].AsRandomReference());
            ProcessHits.push_back(ProcessHitFunc);
            ToneMap.push_back(ToneMappingFunc);
        }
    };
    
    Framebuffer Fb = Framebuffer::Create(Color3(0.0f, 0.0f, 0.0f), 500, 500);
    PinholeCamera::Render(Point(278.0f, 273.0f, -500.0f),
                          500.0f,
                          546.0f,
                          546.0f,
                          Vector(0.0f, 0.0f, 1.0f),
                          Vector(0.0f, 1.0f, 0.0f),
                          0,
                          0,
                          0.01f,
                          5,
                          false,
                          false,
                          Scene.GetTestRoutine(),
                          Lights,
                          CreateState,
                          Fb);
    
    FramebufferIO::SaveAsPFM(Fb, "RenderCornellBoxPlusPlus.pfm");
}
