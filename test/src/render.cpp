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
    PRAYSHADER RecursiveRayTracer;
    PFRAMEBUFFER Framebuffer;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    PDRAWING_SHAPE Sphere;
    SHADER SphereShader;
    POINT3 SphereCenter;
    COLOR3 SphereColor;
    ISTATUS Status;
    PSCENE Scene;
    bool Success;
    VECTOR3 Up;

    PointInitialize(&SphereCenter, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);

    Color3InitializeFromComponents(&SphereColor, 
                                   (FLOAT) 1.0, 
                                   (FLOAT) 0.0, 
                                   (FLOAT) 0.0);

    ConstantShader = ConstantEmissiveShaderAllocate(&SphereColor);

    SphereShader.EmissiveShader = ConstantShader;
    SphereShader.DirectShader = NULL;
    SphereShader.IndirectShader = NULL;
    SphereShader.TranslucentShader = NULL;

    Sphere = SphereAllocate(&SphereCenter,
                            (FLOAT) 1.0,
                            &SphereShader,
                            NULL,
                            NULL,
                            NULL);

    Scene = ListSceneAllocate();

    Status = SceneAddWorldObject(Scene, Sphere);

    Framebuffer = FramebufferAllocate(&SphereColor, 500, 500);

    RecursiveRayTracer = RecursiveNonRouletteRayTracerAllocate(Scene,
                                                               (PRANDOM) Scene, // HACK
                                                               (FLOAT) 0.0005,
                                                               0);

    PointInitialize(&PinholeLocation, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 4.0);
    VectorInitialize(&CameraDirection, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) -1.0);
    VectorInitialize(&Up, (FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 0.0);

    Status = PinholeCameraRender(&PinholeLocation,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 &CameraDirection,
                                 &Up,
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
    INVERTIBLE_MATRIX IdentityMatrix;
    PEMISSIVE_SHADER ConstantShader;
    PRAYSHADER RecursiveRayTracer;
    PFRAMEBUFFER Framebuffer;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    PDRAWING_SHAPE Sphere;
    SHADER SphereShader;
    POINT3 SphereCenter;
    COLOR3 SphereColor;
    ISTATUS Status;
    PSCENE Scene;
    bool Success;
    VECTOR3 Up;

    MatrixInitializeIdentity(&IdentityMatrix);

    PointInitialize(&SphereCenter, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);

    Color3InitializeFromComponents(&SphereColor, 
                                   (FLOAT) 1.0, 
                                   (FLOAT) 0.0, 
                                   (FLOAT) 0.0);

    ConstantShader = ConstantEmissiveShaderAllocate(&SphereColor);

    SphereShader.EmissiveShader = ConstantShader;
    SphereShader.DirectShader = NULL;
    SphereShader.IndirectShader = NULL;
    SphereShader.TranslucentShader = NULL;

    Sphere = SphereAllocate(&SphereCenter,
                            (FLOAT) 1.0,
                            &SphereShader,
                            NULL,
                            NULL,
                            NULL);

    Scene = ListSceneAllocate();

    Status = SceneAddObject(Scene, Sphere, &IdentityMatrix, FALSE);

    Framebuffer = FramebufferAllocate(&SphereColor, 500, 500);

    RecursiveRayTracer = RecursiveNonRouletteRayTracerAllocate(Scene,
                                                               (PRANDOM) Scene, // HACK
                                                               (FLOAT) 0.0005,
                                                               0);

    PointInitialize(&PinholeLocation, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 4.0);
    VectorInitialize(&CameraDirection, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) -1.0);
    VectorInitialize(&Up, (FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 0.0);

    Status = PinholeCameraRender(&PinholeLocation,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 &CameraDirection,
                                 &Up,
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
    INVERTIBLE_MATRIX IdentityMatrix;
    PEMISSIVE_SHADER ConstantShader;
    PRAYSHADER RecursiveRayTracer;
    PFRAMEBUFFER Framebuffer;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    PDRAWING_SHAPE Sphere;
    SHADER SphereShader;
    POINT3 SphereCenter;
    COLOR3 SphereColor;
    ISTATUS Status;
    PSCENE Scene;
    bool Success;
    VECTOR3 Up;

    MatrixInitializeIdentity(&IdentityMatrix);

    PointInitialize(&SphereCenter, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);

    Color3InitializeFromComponents(&SphereColor, 
                                   (FLOAT) 1.0, 
                                   (FLOAT) 0.0, 
                                   (FLOAT) 0.0);

    ConstantShader = ConstantEmissiveShaderAllocate(&SphereColor);

    SphereShader.EmissiveShader = ConstantShader;
    SphereShader.DirectShader = NULL;
    SphereShader.IndirectShader = NULL;
    SphereShader.TranslucentShader = NULL;

    Sphere = SphereAllocate(&SphereCenter,
                            (FLOAT) 1.0,
                            &SphereShader,
                            NULL,
                            NULL,
                            NULL);

    Scene = ListSceneAllocate();

    Status = SceneAddObject(Scene, Sphere, &IdentityMatrix, TRUE);

    Framebuffer = FramebufferAllocate(&SphereColor, 500, 500);

    RecursiveRayTracer = RecursiveNonRouletteRayTracerAllocate(Scene,
                                                               (PRANDOM) Scene, // HACK
                                                               (FLOAT) 0.0005,
                                                               0);

    PointInitialize(&PinholeLocation, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 4.0);
    VectorInitialize(&CameraDirection, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) -1.0);
    VectorInitialize(&Up, (FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 0.0);

    Status = PinholeCameraRender(&PinholeLocation,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 &CameraDirection,
                                 &Up,
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
    PRAYSHADER RecursiveRayTracer;
    PFRAMEBUFFER Framebuffer;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    PDRAWING_SHAPE Triangle;
    SHADER TriangleShader;
    POINT3 TriangleVertex0;
    POINT3 TriangleVertex1;
    POINT3 TriangleVertex2;
    COLOR3 TriangleColor;
    ISTATUS Status;
    PSCENE Scene;
    bool Success;
    VECTOR3 Up;

    PointInitialize(&TriangleVertex0, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    PointInitialize(&TriangleVertex1, (FLOAT) 0.0, (FLOAT) 1.5, (FLOAT) 0.0);
    PointInitialize(&TriangleVertex2, (FLOAT) 1.5, (FLOAT) 0.0, (FLOAT) 0.0);

    Color3InitializeFromComponents(&TriangleColor, 
                                   (FLOAT) 1.0, 
                                   (FLOAT) 0.0, 
                                   (FLOAT) 0.0);

    ConstantShader = ConstantEmissiveShaderAllocate(&TriangleColor);

    TriangleShader.EmissiveShader = ConstantShader;
    TriangleShader.DirectShader = NULL;
    TriangleShader.IndirectShader = NULL;
    TriangleShader.TranslucentShader = NULL;

    Triangle = TriangleAllocate(&TriangleVertex0,
                                &TriangleVertex1,
                                &TriangleVertex2,
                                &TriangleShader,
                                NULL,
                                NULL,
                                NULL,
                                NULL);

    Scene = ListSceneAllocate();

    Status = SceneAddWorldObject(Scene, Triangle);

    Framebuffer = FramebufferAllocate(&TriangleColor, 500, 500);

    RecursiveRayTracer = RecursiveNonRouletteRayTracerAllocate(Scene,
                                                               (PRANDOM) Scene, // HACK
                                                               (FLOAT) 0.0005,
                                                               0);

    PointInitialize(&PinholeLocation, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 4.0);
    VectorInitialize(&CameraDirection, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) -1.0);
    VectorInitialize(&Up, (FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 0.0);

    Status = PinholeCameraRender(&PinholeLocation,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 &CameraDirection,
                                 &Up,
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
    PRAYSHADER RecursiveRayTracer;
    PFRAMEBUFFER Framebuffer;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    PDRAWING_SHAPE Triangle;
    SHADER TriangleShader;
    POINT3 TriangleVertex0;
    POINT3 TriangleVertex1;
    POINT3 TriangleVertex2;
    COLOR3 Red;
    COLOR3 Green;
    COLOR3 Blue;
    ISTATUS Status;
    PSCENE Scene;
    bool Success;
    VECTOR3 Up;

    PointInitialize(&TriangleVertex0, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);
    PointInitialize(&TriangleVertex1, (FLOAT) 0.0, (FLOAT) 1.5, (FLOAT) 0.0);
    PointInitialize(&TriangleVertex2, (FLOAT) 1.5, (FLOAT) 0.0, (FLOAT) 0.0);

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

    TriangleShader.EmissiveShader = InterpolatedEmissiveShader;
    TriangleShader.DirectShader = NULL;
    TriangleShader.IndirectShader = NULL;
    TriangleShader.TranslucentShader = NULL;

    Triangle = TriangleAllocate(&TriangleVertex0,
                                &TriangleVertex1,
                                &TriangleVertex2,
                                &TriangleShader,
                                NULL,
                                NULL,
                                NULL,
                                NULL);

    Scene = ListSceneAllocate();

    Status = SceneAddWorldObject(Scene, Triangle);

    Framebuffer = FramebufferAllocate(&Red, 500, 500);

    RecursiveRayTracer = RecursiveNonRouletteRayTracerAllocate(Scene,
                                                               (PRANDOM) Scene, // HACK
                                                               (FLOAT) 0.0005,
                                                               0);

    PointInitialize(&PinholeLocation, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 4.0);
    VectorInitialize(&CameraDirection, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) -1.0);
    VectorInitialize(&Up, (FLOAT) 0.0, (FLOAT) 1.0, (FLOAT) 0.0);

    Status = PinholeCameraRender(&PinholeLocation,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 (FLOAT) 1.0,
                                 &CameraDirection,
                                 &Up,
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