/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    render.c

Abstract:

    This file contains tests for the rendering paths.

--*/

#include <iristest.h>

TEST(RenderConstantRedSphere)
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
                                                               (PRANDOM) Scene,
                                                               (FLOAT) 0.0005,
                                                               0);

    PointInitialize(&PinholeLocation, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) -2.0);
    VectorInitialize(&CameraDirection, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 1.0);
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

    Success = WritePfm(Framebuffer, "RenderConstantRedSphere.pfm");
}
