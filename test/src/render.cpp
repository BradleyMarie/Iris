/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    render.c

Abstract:

    This file contains tests for the rendering paths.

--*/

#include <iristest.h>

TEST(RenderConstantRedWorldSphere)
{
    PEMISSIVE_SHADER ConstantShader;
    PTRACER RecursiveRayTracer;
    PFRAMEBUFFER Framebuffer;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    PDRAWING_SHAPE Sphere;
    POINT3 SphereCenter;
    COLOR3 SphereColor;
    PTEXTURE Texture;
    ISTATUS Status;
    PSCENE Scene;
    bool Success;
    PRANDOM Rng;
    VECTOR3 Up;

    SphereCenter = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);

    Color3InitializeFromComponents(&SphereColor, 
                                   (FLOAT) 1.0, 
                                   (FLOAT) 0.0, 
                                   (FLOAT) 0.0);

    ConstantShader = ConstantEmissiveShaderAllocate(&SphereColor);

    Texture = ConstantTextureAllocate(ConstantShader, NULL, NULL, NULL);

    Sphere = SphereAllocate(SphereCenter,
                            (FLOAT) 1.0,
                            Texture,
                            NULL,
                            NULL,
                            NULL);

    Scene = ListSceneAllocate();

    Status = SceneAddWorldObject(Scene, Sphere);
    
    Rng = MultiplyWithCarryRngAllocate();

    Framebuffer = FramebufferAllocate(&SphereColor, 500, 500);

    RecursiveRayTracer = RecursiveNonRouletteRayTracerAllocate(Scene,
                                                               Rng,
                                                               (FLOAT) 0.0005,
                                                               0);

    PinholeLocation = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 4.0);
    CameraDirection = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT)-1.0);
    Up = VectorCreate((FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 0.0);

    Status = PinholeCameraRender(PinholeLocation,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 CameraDirection,
                                 Up,
                                 0,
                                 500,
                                 0,
                                 0,
                                 FALSE,
                                 NULL,
                                 RecursiveRayTracer,
                                 Framebuffer);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    Success = WritePfm(Framebuffer, "RenderConstantRedWorldSphere.pfm");
}

TEST(RenderConstantRedModelSphere)
{
    PEMISSIVE_SHADER ConstantShader;
    PTRACER RecursiveRayTracer;
    PFRAMEBUFFER Framebuffer;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    PDRAWING_SHAPE Sphere;
    POINT3 SphereCenter;
    COLOR3 SphereColor;
    PTEXTURE Texture;
    ISTATUS Status;
    PSCENE Scene;
    bool Success;
    PRANDOM Rng;
    VECTOR3 Up;

    SphereCenter = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);

    Color3InitializeFromComponents(&SphereColor, 
                                   (FLOAT) 1.0, 
                                   (FLOAT) 0.0, 
                                   (FLOAT) 0.0);

    ConstantShader = ConstantEmissiveShaderAllocate(&SphereColor);

    Texture = ConstantTextureAllocate(ConstantShader, NULL, NULL, NULL);

    Sphere = SphereAllocate(SphereCenter,
                            (FLOAT) 1.0,
                            Texture,
                            NULL,
                            NULL,
                            NULL);

    Scene = ListSceneAllocate();

    Status = SceneAddObject(Scene, Sphere, NULL, FALSE);

    Rng = MultiplyWithCarryRngAllocate();

    Framebuffer = FramebufferAllocate(&SphereColor, 500, 500);

    RecursiveRayTracer = RecursiveNonRouletteRayTracerAllocate(Scene,
                                                               Rng,
                                                               (FLOAT) 0.0005,
                                                               0);

    PinholeLocation = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 4.0);
    CameraDirection = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT)-1.0);
    Up = VectorCreate((FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 0.0);

    Status = PinholeCameraRender(PinholeLocation,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 CameraDirection,
                                 Up,
                                 0,
                                 500,
                                 0,
                                 0,
                                 FALSE,
                                 NULL,
                                 RecursiveRayTracer,
                                 Framebuffer);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    Success = WritePfm(Framebuffer, "RenderConstantRedModelSphere.pfm");
}

TEST(RenderConstantRedPremultipliedSphere)
{
    PEMISSIVE_SHADER ConstantShader;
    PTRACER RecursiveRayTracer;
    PFRAMEBUFFER Framebuffer;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    PDRAWING_SHAPE Sphere;
    POINT3 SphereCenter;
    COLOR3 SphereColor;
    PTEXTURE Texture;
    ISTATUS Status;
    PSCENE Scene;
    bool Success;
    PRANDOM Rng;
    VECTOR3 Up;

    SphereCenter = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);

    Color3InitializeFromComponents(&SphereColor, 
                                   (FLOAT) 1.0, 
                                   (FLOAT) 0.0, 
                                   (FLOAT) 0.0);

    ConstantShader = ConstantEmissiveShaderAllocate(&SphereColor);

    Texture = ConstantTextureAllocate(ConstantShader, NULL, NULL, NULL);

    Sphere = SphereAllocate(SphereCenter,
                            (FLOAT) 1.0,
                            Texture,
                            NULL,
                            NULL,
                            NULL);

    Scene = ListSceneAllocate();

    Status = SceneAddObject(Scene, Sphere, NULL, TRUE);

    Rng = MultiplyWithCarryRngAllocate();

    Framebuffer = FramebufferAllocate(&SphereColor, 500, 500);

    RecursiveRayTracer = RecursiveNonRouletteRayTracerAllocate(Scene,
                                                               Rng,
                                                               (FLOAT) 0.0005,
                                                               0);

    PinholeLocation = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 4.0);
    CameraDirection = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT)-1.0);
    Up = VectorCreate((FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 0.0);

    Status = PinholeCameraRender(PinholeLocation,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 CameraDirection,
                                 Up,
                                 0,
                                 500,
                                 0,
                                 0,
                                 FALSE,
                                 NULL,
                                 RecursiveRayTracer,
                                 Framebuffer);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    Success = WritePfm(Framebuffer, "RenderConstantRedPremultipliedSphere.pfm");
}

TEST(RenderConstantRedWorldTriangle)
{
    PEMISSIVE_SHADER ConstantShader;
    PTRACER RecursiveRayTracer;
    PFRAMEBUFFER Framebuffer;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    PDRAWING_SHAPE Triangle;
    POINT3 TriangleVertex0;
    POINT3 TriangleVertex1;
    POINT3 TriangleVertex2;
    COLOR3 TriangleColor;
    PTEXTURE Texture;
    ISTATUS Status;
    PSCENE Scene;
    bool Success;
    PRANDOM Rng;
    VECTOR3 Up;

    TriangleVertex0 = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    TriangleVertex1 = PointCreate((FLOAT) 0.0, (FLOAT) 1.5, (FLOAT) 0.0);
    TriangleVertex2 = PointCreate((FLOAT) 1.5, (FLOAT) 0.0, (FLOAT) 0.0);

    Color3InitializeFromComponents(&TriangleColor, 
                                   (FLOAT) 1.0, 
                                   (FLOAT) 0.0, 
                                   (FLOAT) 0.0);

    ConstantShader = ConstantEmissiveShaderAllocate(&TriangleColor);

    Texture = ConstantTextureAllocate(ConstantShader, NULL, NULL, NULL);

    Triangle = TriangleAllocate(TriangleVertex0,
                                TriangleVertex1,
                                TriangleVertex2,
                                Texture,
                                NULL,
                                NULL,
                                NULL);

    Scene = ListSceneAllocate();

    Status = SceneAddWorldObject(Scene, Triangle);

    Rng = MultiplyWithCarryRngAllocate();

    Framebuffer = FramebufferAllocate(&TriangleColor, 500, 500);

    RecursiveRayTracer = RecursiveNonRouletteRayTracerAllocate(Scene,
                                                               Rng,
                                                               (FLOAT) 0.0005,
                                                               0);

    PinholeLocation = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 4.0);
    CameraDirection = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT)-1.0);
    Up = VectorCreate((FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 0.0);

    Status = PinholeCameraRender(PinholeLocation,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 CameraDirection,
                                 Up,
                                 0,
                                 500,
                                 0,
                                 0,
                                 FALSE,
                                 NULL,
                                 RecursiveRayTracer,
                                 Framebuffer);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    Success = WritePfm(Framebuffer, "RenderConstantRedWorldTriangle.pfm");
}

TEST(RenderInterpolatedRedWorldTriangle)
{
    PEMISSIVE_SHADER InterpolatedEmissiveShader;
    PEMISSIVE_SHADER ConstantShader0;
    PEMISSIVE_SHADER ConstantShader1;
    PEMISSIVE_SHADER ConstantShader2;
    PTRACER RecursiveRayTracer;
    PFRAMEBUFFER Framebuffer;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    PDRAWING_SHAPE Triangle;
    POINT3 TriangleVertex0;
    POINT3 TriangleVertex1;
    POINT3 TriangleVertex2;
    PTEXTURE Texture;
    COLOR3 Red;
    COLOR3 Green;
    COLOR3 Blue;
    ISTATUS Status;
    PSCENE Scene;
    bool Success;
    PRANDOM Rng;
    VECTOR3 Up;

    TriangleVertex0 = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    TriangleVertex1 = PointCreate((FLOAT) 0.0, (FLOAT) 1.5, (FLOAT) 0.0);
    TriangleVertex2 = PointCreate((FLOAT) 1.5, (FLOAT) 0.0, (FLOAT) 0.0);

    Color3InitializeFromComponents(&Red, 
                                   (FLOAT) 1.0, 
                                   (FLOAT) 0.0, 
                                   (FLOAT) 0.0);

    Color3InitializeFromComponents(&Green, 
                                   (FLOAT) 0.0, 
                                   (FLOAT) 1.0, 
                                   (FLOAT) 0.0);

    Color3InitializeFromComponents(&Blue, 
                                   (FLOAT) 0.0, 
                                   (FLOAT) 0.0, 
                                   (FLOAT) 1.0);

    ConstantShader0 = ConstantEmissiveShaderAllocate(&Red);
    ConstantShader1 = ConstantEmissiveShaderAllocate(&Green);
    ConstantShader2 = ConstantEmissiveShaderAllocate(&Blue);

    InterpolatedEmissiveShader = InterpolatedTriangleEmissiveShaderAllocate(ConstantShader0,
                                                                            ConstantShader1,
                                                                            ConstantShader2);

    Texture = ConstantTextureAllocate(InterpolatedEmissiveShader,
                                      NULL,
                                      NULL,
                                      NULL);

    Triangle = TriangleAllocate(TriangleVertex0,
                                TriangleVertex1,
                                TriangleVertex2,
                                Texture,
                                NULL,
                                NULL,
                                NULL);

    Scene = ListSceneAllocate();

    Status = SceneAddWorldObject(Scene, Triangle);

    Rng = MultiplyWithCarryRngAllocate();

    Framebuffer = FramebufferAllocate(&Red, 500, 500);

    RecursiveRayTracer = RecursiveNonRouletteRayTracerAllocate(Scene,
                                                               Rng,
                                                               (FLOAT) 0.0005,
                                                               0);

    PinholeLocation = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 4.0);
    CameraDirection = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT)-1.0);
    Up = VectorCreate((FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 0.0);

    Status = PinholeCameraRender(PinholeLocation,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 CameraDirection,
                                 Up,
                                 0,
                                 500,
                                 0,
                                 0,
                                 FALSE,
                                 NULL,
                                 RecursiveRayTracer,
                                 Framebuffer);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    Success = WritePfm(Framebuffer, "RenderInterpolatedRedWorldTriangle.pfm");
}

TEST(RenderPhongWorldSphere)
{
    PTRACER RecursiveRayTracer;
    PDIRECT_SHADER PhongShader;
    COLOR3 AmbientShaderColor;
    COLOR3 DiffuseShaderColor;
    PNORMAL SphereFrontNormal;
    PFRAMEBUFFER Framebuffer;
    COLOR3 DiffuseLightColor;
    PPHONG_LIGHT PhongLight;
    VECTOR3 CameraDirection;
    VECTOR3 LightDirection;
    POINT3 PinholeLocation;
    PDRAWING_SHAPE Sphere;
    POINT3 SphereCenter;
    PTEXTURE Texture;
    ISTATUS Status;
    PSCENE Scene;
    COLOR3 Black;
    bool Success;
    PRANDOM Rng;
    VECTOR3 Up;

    SphereCenter = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);

    Color3InitializeFromComponents(&DiffuseLightColor, 
                                   (FLOAT) 1.0, 
                                   (FLOAT) 1.0, 
                                   (FLOAT) 1.0);

    Color3InitializeFromComponents(&AmbientShaderColor, 
                                   (FLOAT) 0.2, 
                                   (FLOAT) 0.2, 
                                   (FLOAT) 0.2);

    Color3InitializeFromComponents(&DiffuseShaderColor, 
                                   (FLOAT) 0.8, 
                                   (FLOAT) 0.8, 
                                   (FLOAT) 0.8);

    Color3InitializeBlack(&Black);

    LightDirection = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);

    PhongLight = DirectionalPhongLightAllocate(LightDirection,
                                               &Black,
                                               &DiffuseLightColor,
                                               &Black,
                                               FALSE);

    PhongShader = PhongDirectShaderAllocate(&PhongLight,
                                            1,
                                            LightShaderEvaluateAllLights,
                                            &AmbientShaderColor,
                                            &DiffuseShaderColor,
                                            &Black,
                                            (FLOAT) 0.0);

    Texture = ConstantTextureAllocate(NULL, PhongShader, NULL, NULL);

    SphereFrontNormal = SphereNormalAllocate(SphereCenter, TRUE);

    Sphere = SphereAllocate(SphereCenter,
                            (FLOAT) 1.0,
                            Texture,
                            SphereFrontNormal,
                            NULL,
                            NULL);

    Scene = ListSceneAllocate();

    Status = SceneAddWorldObject(Scene, Sphere);

    Rng = MultiplyWithCarryRngAllocate();

    Framebuffer = FramebufferAllocate(&Black, 500, 500);

    RecursiveRayTracer = RecursiveNonRouletteRayTracerAllocate(Scene,
                                                               Rng,
                                                               (FLOAT) 0.0005,
                                                               0);

    PinholeLocation = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 4.0);
    CameraDirection = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT)-1.0);
    Up = VectorCreate((FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 0.0);

    Status = PinholeCameraRender(PinholeLocation,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 CameraDirection,
                                 Up,
                                 0,
                                 500,
                                 0,
                                 0,
                                 FALSE,
                                 NULL,
                                 RecursiveRayTracer,
                                 Framebuffer);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    Success = WritePfm(Framebuffer, "RenderPhongWorldSphere.pfm");
}

TEST(RenderPerfectSpecularWorldSphere)
{
    PINDIRECT_SHADER PerfectSpecularShader;
    PEMISSIVE_SHADER ConstantShader;
    PTRACER RecursiveRayTracer;
    PNORMAL SphereFrontNormal;
    PFRAMEBUFFER Framebuffer;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    PDRAWING_SHAPE Sphere;
    POINT3 SphereCenter2;
    POINT3 SphereCenter;
    COLOR3 SphereColor;
    COLOR3 Reflectance;
    PTEXTURE Texture;
    ISTATUS Status;
    PSCENE Scene;
    bool Success;
    PRANDOM Rng;
    VECTOR3 Up;

    SphereCenter = PointCreate((FLOAT) 1.0, (FLOAT) 0.0, (FLOAT)-1.0);

    Color3InitializeFromComponents(&SphereColor, 
                                   (FLOAT) 1.0, 
                                   (FLOAT) 0.0, 
                                   (FLOAT) 0.0);

    ConstantShader = ConstantEmissiveShaderAllocate(&SphereColor);

    Texture = ConstantTextureAllocate(ConstantShader, NULL, NULL, NULL);

    Sphere = SphereAllocate(SphereCenter,
                            (FLOAT) 1.0,
                            Texture,
                            NULL,
                            NULL,
                            NULL);

    Scene = ListSceneAllocate();

    Status = SceneAddWorldObject(Scene, Sphere);

    SphereCenter2 = PointCreate((FLOAT)-1.0, (FLOAT) 0.0, (FLOAT)-1.0);

    Color3InitializeWhite(&Reflectance);

    PerfectSpecularShader = PerfectSpecularIndirectShaderAllocate(&Reflectance);

    Texture = ConstantTextureAllocate(NULL, NULL, PerfectSpecularShader, NULL);

    SphereFrontNormal = SphereNormalAllocate(SphereCenter2, TRUE);

    Sphere = SphereAllocate(SphereCenter2,
                            (FLOAT) 1.0,
                            Texture,
                            SphereFrontNormal,
                            NULL,
                            NULL);

    Status = SceneAddWorldObject(Scene, Sphere);

    Rng = MultiplyWithCarryRngAllocate();

    Framebuffer = FramebufferAllocate(&SphereColor, 500, 500);

    RecursiveRayTracer = RecursiveNonRouletteRayTracerAllocate(Scene,
                                                               Rng,
                                                               (FLOAT) 0.0005,
                                                               1);

    PinholeLocation = PointCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 4.0);
    CameraDirection = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT)-1.0);
    Up = VectorCreate((FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 0.0);

    Status = PinholeCameraRender(PinholeLocation,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 CameraDirection,
                                 Up,
                                 0,
                                 500,
                                 0,
                                 0,
                                 FALSE,
                                 NULL,
                                 RecursiveRayTracer,
                                 Framebuffer);

    CHECK_EQUAL(ISTATUS_SUCCESS, Status);

    Success = WritePfm(Framebuffer, "RenderPerfectSpecularWorldSphere.pfm");
}

TEST(RenderMirrorPhongCheckerboardSpheres)
{
    PPHONG_LIGHT Lights[4];
    POINT3 Sphere0Center;
    POINT3 Sphere1Center;
    POINT3 Light0Location;
    POINT3 Light1Location;
    POINT3 Light2Location;
    POINT3 Light3Location;
    POINT3 InfinitePlaneLocation;
    VECTOR3 InfinitePlaneNormal;
    COLOR3 White;
    COLOR3 Black;
    COLOR3 Red;
    COLOR3 Green;
    COLOR3 Blue;
    COLOR3 GreyishGreen;
    COLOR3 Grey;
    COLOR3 SphereMirrorReflectance;
    COLOR3 InfinitePlaneWhiteMirrorReflectance;
    COLOR3 InfinitePlaneBlackMirrorReflectance;
    PNORMAL Sphere0Normal;
    PNORMAL Sphere1Normal;
    PNORMAL InfinitePlaneSurfaceNormal;
    PDIRECT_SHADER SpherePhongShader;
    PDIRECT_SHADER InfinitePlaneWhitePhongShader;
    PDIRECT_SHADER InfinitePlaneBlackPhongShader;
    PINDIRECT_SHADER SphereIndirectShader;
    PINDIRECT_SHADER InfinitePlaneWhiteIndirectShader;
    PINDIRECT_SHADER InfinitePlaneBlackIndirectShader;
    PDRAWING_SHAPE Sphere0;
    PDRAWING_SHAPE Sphere1;
    PDRAWING_SHAPE InfinitePlane;
    PTEXTURE SphereTexture;
    PTEXTURE InfinitePlaneTexture;
    PSCENE Scene;
    PFRAMEBUFFER Framebuffer;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    POINT3 LookAt;
    VECTOR3 Up;
    PTRACER RecursiveRayTracer;
    ISTATUS Status;
    PRANDOM Rng;

    InfinitePlaneLocation = PointCreate((FLOAT) 0.0,
                                        (FLOAT) 0.0,
                                        (FLOAT) 0.0);

    Sphere0Center = PointCreate((FLOAT) 0.0,
                                (FLOAT) 1.0,
                                (FLOAT) 0.0);

    Sphere1Center= PointCreate((FLOAT) -1.0,
                               (FLOAT) 0.5,
                               (FLOAT) 1.5);

    Light0Location = PointCreate((FLOAT) -2.0,
                                 (FLOAT) 2.5,
                                 (FLOAT) 0.0);

    Light1Location = PointCreate((FLOAT) 1.5,
                                 (FLOAT) 2.5,
                                 (FLOAT) 1.5);

    Light2Location = PointCreate((FLOAT) 1.5,
                                 (FLOAT) 2.5,
                                 (FLOAT) -1.5);

    Light3Location = PointCreate((FLOAT) 0.0,
                                 (FLOAT) 3.5,
                                 (FLOAT) 0.0);

    InfinitePlaneNormal = VectorCreate((FLOAT) 0.0,
                                       (FLOAT) 1.0,
                                       (FLOAT) 0.0);

    Color3InitializeFromComponents(&Red,
                                   (FLOAT) 0.49,
                                   (FLOAT) 0.07,
                                   (FLOAT) 0.07);

    Color3InitializeFromComponents(&Green,
                                   (FLOAT) 0.07,
                                   (FLOAT) 0.49,
                                   (FLOAT) 0.07);

    Color3InitializeFromComponents(&Blue,
                                   (FLOAT) 0.07,
                                   (FLOAT) 0.07,
                                   (FLOAT) 0.49);

    Color3InitializeFromComponents(&GreyishGreen,
                                   (FLOAT) 0.21,
                                   (FLOAT) 0.21,
                                   (FLOAT) 0.35);

    Color3InitializeFromComponents(&Grey,
                                   (FLOAT) 0.5,
                                   (FLOAT) 0.5,
                                   (FLOAT) 0.5);

    Color3InitializeFromComponents(&SphereMirrorReflectance,
                                  (FLOAT) 0.6,
                                  (FLOAT) 0.6,
                                  (FLOAT) 0.6);

    Color3InitializeFromComponents(&InfinitePlaneBlackMirrorReflectance,
                                   (FLOAT) 0.1,
                                   (FLOAT) 0.1, 
                                   (FLOAT) 0.1);

    Color3InitializeFromComponents(&InfinitePlaneWhiteMirrorReflectance,
                                   (FLOAT) 0.7,
                                   (FLOAT) 0.7, 
                                   (FLOAT) 0.7);

    Color3InitializeBlack(&Black);

    Color3InitializeWhite(&White);

    Lights[0] = PointPhongLightAllocate(Light0Location,
                                        &Black,
                                        &Red,
                                        &Red,
                                        TRUE);

    Lights[1] = PointPhongLightAllocate(Light1Location,
                                        &Black,
                                        &Blue,
                                        &Blue,
                                        TRUE);

    Lights[2] = PointPhongLightAllocate(Light2Location,
                                        &Black,
                                        &Green,
                                        &Green,
                                        TRUE);

    Lights[3] = PointPhongLightAllocate(Light3Location,
                                        &Black,
                                        &GreyishGreen,
                                        &GreyishGreen,
                                        TRUE);

    InfinitePlaneSurfaceNormal = ConstantNormalAllocate(InfinitePlaneNormal,
                                                        FALSE);

    Sphere0Normal = SphereNormalAllocate(Sphere0Center, TRUE);

    Sphere1Normal = SphereNormalAllocate(Sphere1Center, TRUE);

    SpherePhongShader = PhongDirectShaderAllocate(Lights,
                                                  4,
                                                  LightShaderEvaluateAllLights,
                                                  &Black,
                                                  &White,
                                                  &Grey,
                                                  (FLOAT) 50.0);

    InfinitePlaneWhitePhongShader = PhongDirectShaderAllocate(Lights,
                                                              4,
                                                              LightShaderEvaluateAllLights,
                                                              &Black,
                                                              &White,
                                                              &Black,
                                                              (FLOAT) 150.0);

    InfinitePlaneBlackPhongShader = PhongDirectShaderAllocate(Lights,
                                                              4,
                                                              LightShaderEvaluateAllLights,
                                                              &Black,
                                                              &Black,
                                                              &Black,
                                                              (FLOAT) 150.0);

    InfinitePlaneWhiteIndirectShader = PerfectSpecularIndirectShaderAllocate(&InfinitePlaneWhiteMirrorReflectance);

    InfinitePlaneBlackIndirectShader = PerfectSpecularIndirectShaderAllocate(&InfinitePlaneBlackMirrorReflectance);

    SphereIndirectShader = PerfectSpecularIndirectShaderAllocate(&SphereMirrorReflectance);

    InfinitePlaneTexture = XZCheckerboardTextureAllocate(NULL,
                                                         InfinitePlaneWhitePhongShader,
                                                         InfinitePlaneWhiteIndirectShader,
                                                         NULL,
                                                         NULL,
                                                         InfinitePlaneBlackPhongShader,
                                                         InfinitePlaneBlackIndirectShader,
                                                         NULL);

    SphereTexture = ConstantTextureAllocate(NULL,
                                            SpherePhongShader,
                                            SphereIndirectShader,
                                            NULL);

    Sphere0 = SphereAllocate(Sphere0Center,
                             (FLOAT) 1.0,
                             SphereTexture,
                             Sphere0Normal,
                             NULL,
                             NULL);

    Sphere1 = SphereAllocate(Sphere1Center,
                             (FLOAT) 0.5,
                             SphereTexture,
                             Sphere1Normal,
                             NULL,
                             NULL);

    InfinitePlane = InfinitePlaneAllocate(InfinitePlaneLocation,
                                          InfinitePlaneNormal,
                                          InfinitePlaneTexture,
                                          InfinitePlaneSurfaceNormal,
                                          NULL,
                                          NULL);

    Scene = ListSceneAllocate();

    Status = SceneAddWorldObject(Scene, Sphere0);

    Status = SceneAddWorldObject(Scene, Sphere1);

    Status = SceneAddWorldObject(Scene, InfinitePlane);

    Rng = MultiplyWithCarryRngAllocate();

    Framebuffer = FramebufferAllocate(&Black, 500, 500);

    RecursiveRayTracer = RecursiveNonRouletteRayTracerAllocate(Scene,
                                                               Rng,
                                                               (FLOAT) 0.0005,
                                                               5);

    Up = VectorCreate((FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 0.0);
    PinholeLocation = PointCreate((FLOAT) 3.0, (FLOAT) 2.0, (FLOAT) 4.0);
    LookAt = PointCreate((FLOAT)-1.0, (FLOAT) 0.45, (FLOAT) 0.0);
    CameraDirection = PointSubtract(LookAt, PinholeLocation);

    Status = PinholeCameraRender(PinholeLocation,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 CameraDirection,
                                 Up,
                                 0,
                                 500,
                                 0,
                                 0,
                                 FALSE,
                                 NULL,
                                 RecursiveRayTracer,
                                 Framebuffer);

    WritePfm(Framebuffer, "RenderMirrorPhongCheckerboardSpheres.pfm");
}

TEST(RenderCornellBox)
{
    COLOR3 ShaderColor;
    PFRAMEBUFFER Framebuffer;
    PRANDOM Rng;
    PEMISSIVE_SHADER EmissiveShader;
    PINDIRECT_SHADER IndirectShader;
    PTEXTURE Texture;
    PSCENE Scene;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    VECTOR3 Up;
    PTRACER PathTracer;
    ISTATUS Status;

    Scene = ListSceneAllocate();

    //
    // Light Source
    //

    Color3InitializeFromComponents(&ShaderColor, 12.0f, 12.0f, 12.0f);

    EmissiveShader = ConstantEmissiveShaderAllocate(&ShaderColor);

    Texture = ConstantTextureAllocate(EmissiveShader, 
                                      NULL,
                                      NULL,
                                      NULL);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
                   303.000000f,
                   549.900000f,
                   247.000000f,
                   303.000000f,
                   549.900000f,
                   303.000000f,
                   247.000000f,
                   549.900000f,
                   303.000000f,
                   247.000000f,
                   549.900000f,
                   247.000000f,
                   Texture,
                   Texture);

    //
    // Tall Box
    //

    Color3InitializeFromComponents(&ShaderColor, 0.75f, 0.75f, 0.75f);

    IndirectShader = LambertianIndirectShaderAllocate(&ShaderColor);

    Texture = ConstantTextureAllocate(NULL, 
                                      NULL,
                                      IndirectShader,
                                      NULL);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
423.000000f, 330.000000f, 247.000000f,
265.000000f, 330.000000f, 296.000000f,
314.000000f, 330.000000f, 456.000000f, 
472.000000f, 330.000000f, 406.000000f,
                   Texture,
                   Texture);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
423.000000f, 0.000000f, 247.000000f,
423.000000f, 330.000000f, 247.000000f,
472.000000f, 330.000000f, 406.000000f,
472.000000f, 0.000000f, 406.000000f,
                   Texture,
                   Texture);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
472.000000f, 0.000000f, 406.000000f,
472.000000f, 330.000000f, 406.000000f,
314.000000f, 330.000000f, 456.000000f,
314.000000f, 0.000000f, 456.000000f,
                   Texture,
                   Texture);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
314.000000f, 0.000000f, 456.000000f,
314.000000f, 330.000000f, 456.000000f, 
265.000000f, 330.000000f, 296.000000f, 
265.000000f, 0.000000f, 296.000000f,
                   Texture,
                   Texture);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
265.000000f, 0.000000f, 296.000000f,
265.000000f, 330.000000f, 296.000000f,
423.000000f, 330.000000f, 247.000000f,
423.000000f, 0.000000f, 247.000000f,
                   Texture,
                   Texture);

    //
    // Short Box
    //

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
130.000000f, 165.000000f, 65.000000f,
82.000000f, 165.000000f, 225.000000f, 
240.000000f, 165.000000f, 272.000000f, 
290.000000f, 165.000000f, 114.000000f,
                   Texture,
                   Texture);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
290.000000f, 0.000000f, 114.000000f,
290.000000f, 165.000000f, 114.000000f,
240.000000f, 165.000000f, 272.000000f,
240.000000f, 0.000000f, 272.000000f,
                   Texture,
                   Texture);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
130.000000f, 0.000000f, 65.000000f, 
130.000000f, 165.000000f, 65.000000f,
290.000000f, 165.000000f, 114.000000f,
290.000000f, 0.000000f, 114.000000f,
                   Texture,
                   Texture);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
82.000000f, 0.000000f, 225.000000f,
82.000000f, 165.000000f, 225.000000f,
130.000000f, 165.000000f, 65.000000f,
130.000000f, 0.000000f, 65.000000f,
                   Texture,
                   Texture);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
240.000000f, 0.000000f, 272.000000f, 
240.000000f, 165.000000f, 272.000000f,
82.000000f, 165.000000f, 225.000000f,
82.000000f, 0.000000f, 225.000000f,
                   Texture,
                   Texture);

    //
    // White Walls
    //

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
550.000000f, 0.000000f, 560.000000f,
0.000000f, 0.000000f, 560.000000f, 
0.000000f, 550.000000f, 560.000000f, 
560.000000f, 550.000000f, 560.000000f,
                   Texture,
                   Texture);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
560.000000f, 550.000000f, 0.000000f,
560.000000f, 550.000000f, 560.000000f,
0.000000f, 550.000000f, 560.000000f,
0.000000f, 550.000000f, 0.000000f,
                   Texture,
                   Texture);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
550.000000f, 0.000000f, 0.000000f,
0.000000f, 0.000000f, 0.000000f,
0.000000f, 0.000000f, 560.000000f,
550.000000f, 0.000000f, 560.000000f,
                   Texture,
                   Texture);

    //
    // Red Wall
    //

    Color3InitializeFromComponents(&ShaderColor, 0.75f, 0.25f, 0.25f);

    IndirectShader = LambertianIndirectShaderAllocate(&ShaderColor);

    Texture = ConstantTextureAllocate(NULL, 
                                      NULL,
                                      IndirectShader,
                                      NULL);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
550.000000f, 0.000000f, 0.000000f,
550.000000f, 0.000000f, 560.000000f,
560.000000f, 550.000000f, 560.000000f,
560.000000f, 550.000000f, 0.000000f,
                   Texture,
                   Texture);

    //
    // Blue Wall
    //

    Color3InitializeFromComponents(&ShaderColor, 0.25f, 0.25f, 0.75f);

    IndirectShader = LambertianIndirectShaderAllocate(&ShaderColor);

    Texture = ConstantTextureAllocate(NULL, 
                                      NULL,
                                      IndirectShader,
                                      NULL);

    CreateFlatQuad(Scene,
                   NULL,
                   TRUE,
0.000000f, 0.000000f, 560.000000f,
0.000000f, 0.000000f, 0.000000f, 
0.000000f, 550.000000f, 0.000000f, 
0.000000f, 550.000000f, 560.000000f, 
                   Texture,
                   Texture);

    //
    // Render
    //

    Rng = MultiplyWithCarryRngAllocate();

    Color3InitializeBlack(&ShaderColor);

    Framebuffer = FramebufferAllocate(&ShaderColor, 500, 500);

    PathTracer = PathTracerAllocate(Scene,
                                    Rng,
                                    0.0005f,
                                    0.0f,
                                    0.5f,
                                    5,
                                    10);

    Up = VectorCreate(0.0f, 1.0f, 0.0f);
    PinholeLocation = PointCreate(278.0f, 273.0f, -500.0f);
    CameraDirection = VectorCreate(0.0f, 0.0f, 1.0f);

    Status = PinholeCameraRender(PinholeLocation,
                                 500,
                                 546,
                                 546,
                                 CameraDirection,
                                 Up,
                                 0,
                                 500,
                                 0,
                                 0,
                                 FALSE,
                                 NULL,
                                 PathTracer,
                                 Framebuffer);

    WritePfm(Framebuffer, "RenderCornellBox.pfm");
}