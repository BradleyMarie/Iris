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

    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectra) -> Color3
    {
        return PhongToRGB(Spectra);
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
                                              ConstantMaterial,
                                              nullopt,
                                              nullopt);

    TestListScene Scene;
    Scene.AddGeometry(SphereRadiusOne, Matrix::Scalar(0.5f, 0.5f, 0.5f));

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

    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectra) -> Color3
    {
        return PhongToRGB(Spectra);
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

    Fb.SaveAsPFM("RenderConstantRedModelSpherePlusPlus.pfm");
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
                                              ConstantMaterial,
                                              nullopt,
                                              nullopt);

    TestListScene Scene;
    Scene.AddGeometry(SphereRadiusOne, Matrix::Scalar(1.0f, 1.0f, 1.0f), true);

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

    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectra) -> Color3
    {
        return PhongToRGB(Spectra);
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

    Fb.SaveAsPFM("RenderConstantRedPremultipliedSpherePlusPlus.pfm");
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
                                        ConstantMaterial,
                                        nullopt,
                                        nullopt);

    Scene.AddGeometry(SphereOne);

    Material RefletiveMaterial = PhongMaterial::Create(Color3::CreateBlack(),
                                                       Color3::CreateBlack(),
                                                       Color3::CreateBlack(),
                                                       1.0f,
                                                       Color3(1.0f, 1.0f, 1.0f));

    Geometry SphereTwo = Sphere::Create(Point(-1.0f, 0.0f, -1.0f),
                                        1.0f,
                                        RefletiveMaterial,
                                        RefletiveMaterial,
                                        nullopt,
                                        nullopt);

    Scene.AddGeometry(SphereTwo);

    ProcessHitRoutine ProcessHitFunc = [&](GeometryReference Geom,
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
        
        SpectrumReference ReflectedLight(nullptr);

        if (std::get<2>(Ref) > 0.0 && RayTracerPtr != nullptr)
        {
            ReflectedLight = RayTracerPtr->TraceClosestHit(Ray(WorldHitPoint, std::get<1>(Ref)), ProcessHitFunc);
        }

        return SpectrumCompositor.Reflect(ReflectedLight, std::get<0>(Ref));
    };

    ToneMappingRoutine ToneMappingFunc = [](SpectrumReference Spectra) -> Color3
    {
        return PhongToRGB(Spectra);
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
                          1,
                          false,
                          false,
                          Scene.GetTestRoutine(),
                          nullopt,
                          CreateState,
                          Fb);

    Fb.SaveAsPFM("RenderPerfectSpecularWorldSpherePlusPlus.pfm");
}