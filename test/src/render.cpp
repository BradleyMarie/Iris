/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    render.c

Abstract:

    This file contains tests for the rendering paths.

--*/

#include <iristest.h>

TEST(RenderCornellBox)
{
    COLOR3 ShaderColor;
    PFRAMEBUFFER Framebuffer;
    PRANDOM_REFERENCE RngReference;
    PRANDOM Rng;
    PEMISSIVE_SHADER EmissiveShader;
    PINDIRECT_SHADER IndirectShader;
    PTEXTURE Texture;
    PCOLOR_SCENE Scene;
    VECTOR3 CameraDirection;
    POINT3 PinholeLocation;
    VECTOR3 Up;
    PTRACER PathTracer;
    ISTATUS Status;

    Scene = ListSceneAllocate();

    //
    // Light Source
    //

    ShaderColor = Color3InitializeFromComponents(12.0f, 12.0f, 12.0f);

    EmissiveShader = ConstantEmissiveShaderAllocate(ShaderColor);

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

    ShaderColor = Color3InitializeFromComponents(0.75f, 0.75f, 0.75f);

    IndirectShader = LambertianIndirectShaderAllocate(ShaderColor);

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

    ShaderColor = Color3InitializeFromComponents(0.75f, 0.25f, 0.25f);

    IndirectShader = LambertianIndirectShaderAllocate(ShaderColor);

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

    ShaderColor = Color3InitializeFromComponents(0.25f, 0.25f, 0.75f);

    IndirectShader = LambertianIndirectShaderAllocate(ShaderColor);

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

    MultiplyWithCarryRngAllocate(&Rng);
    RngReference = RandomGetRandomReference(Rng);

    ShaderColor = Color3InitializeBlack();

    FramebufferAllocate(ShaderColor, 500, 500, &Framebuffer);

    PathTracer = PathTracerAllocate(Scene,
                                    RngReference,
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

    FramebufferSaveAsPFM(Framebuffer, "RenderCornellBox.pfm");
}