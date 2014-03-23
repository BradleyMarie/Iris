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
    PDRAWING_SHAPE Sphere;
    POINT3 PinholeLocation;
    SHADER SphereShader;
    POINT3 SphereCenter;
    COLOR3 SphereColor;
    ISTATUS Status;
    PSCENE Scene;
    bool Success;

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

    Success = WritePfm(Framebuffer, "RenderConstantRedSphere.pfm");
}
