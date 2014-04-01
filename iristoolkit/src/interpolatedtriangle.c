/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    interpolatedtriangle.c

Abstract:

    This file contains the definitions for the INTERPOLATED_TRIANGLE types.

--*/

#include <iristoolkitp.h>

//
// Types
//

typedef struct _INTERPOLATED_TRIANGLE_EMISSIVE_SHADER {
    EMISSIVE_SHADER EmissiveShaderHeader;
    PCEMISSIVE_SHADER EmissiveShaders[IRIS_TOOLKIT_TRIANGLE_VERTICES];
} INTERPOLATED_TRIANGLE_EMISSIVE_SHADER, *PINTERPOLATED_TRIANGLE_EMISSIVE_SHADER;

typedef CONST INTERPOLATED_TRIANGLE_EMISSIVE_SHADER *PCINTERPOLATED_TRIANGLE_EMISSIVE_SHADER;

typedef struct _INTERPOLATED_TRIANGLE_DIRECT_SHADER {
    DIRECT_SHADER DirectShaderHeader;
    PCDIRECT_SHADER DirectShaders[IRIS_TOOLKIT_TRIANGLE_VERTICES];
} INTERPOLATED_TRIANGLE_DIRECT_SHADER, *PINTERPOLATED_TRIANGLE_DIRECT_SHADER;

typedef CONST INTERPOLATED_TRIANGLE_DIRECT_SHADER *PCINTERPOLATED_TRIANGLE_DIRECT_SHADER;

typedef struct _INTERPOLATED_TRIANGLE_INDIRECT_SHADER {
    INDIRECT_SHADER IndirectShaderHeader;
    PCINDIRECT_SHADER IndirectShaders[IRIS_TOOLKIT_TRIANGLE_VERTICES];
} INTERPOLATED_TRIANGLE_INDIRECT_SHADER, *PINTERPOLATED_TRIANGLE_INDIRECT_SHADER;

typedef CONST INTERPOLATED_TRIANGLE_INDIRECT_SHADER *PCINTERPOLATED_TRIANGLE_INDIRECT_SHADER;

typedef struct _INTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER {
    TRANSLUCENT_SHADER TranslucentShaderHeader;
    PCTRANSLUCENT_SHADER TranslucentShaders[IRIS_TOOLKIT_TRIANGLE_VERTICES];
} INTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER, *PINTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER;

typedef CONST INTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER *PCINTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER;

typedef struct _INTERPOLATED_TRIANGLE_NORMAL {
    NORMAL NormalHeader;
    PCNORMAL Normals[IRIS_TOOLKIT_TRIANGLE_VERTICES];
} INTERPOLATED_TRIANGLE_NORMAL, *PINTERPOLATED_TRIANGLE_NORMAL;

typedef CONST INTERPOLATED_TRIANGLE_NORMAL *PCINTERPOLATED_TRIANGLE_NORMAL;

//
// Static functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
InterpolatedTriangleEmissiveShaderShade(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Out_ PCOLOR3 Emissive
    )
{
    PCINTERPOLATED_TRIANGLE_EMISSIVE_SHADER InterpolatedTriangleEmissiveShader;
    PCBARYCENTRIC_COORDINATES BarycentricCoordinates;
    PCEMISSIVE_SHADER EmissiveShader;
    COLOR3 ComponentColor;
    ISTATUS Status;
    UINT32 Index;

    ASSERT(Context != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(AdditionalData != NULL);
    ASSERT(Emissive != NULL);

    InterpolatedTriangleEmissiveShader = (PCINTERPOLATED_TRIANGLE_EMISSIVE_SHADER) Context;
    BarycentricCoordinates = (PCBARYCENTRIC_COORDINATES) AdditionalData;

    Color3InitializeBlack(Emissive);

    for (Index = 0; Index < IRIS_TOOLKIT_TRIANGLE_VERTICES; Index++)
    {
        EmissiveShader = InterpolatedTriangleEmissiveShader->EmissiveShaders[Index];

        if (EmissiveShader == NULL)
        {
            continue;
        }

        Status = EmissiveShaderShade(EmissiveShader,
                                     WorldHitPoint,
                                     ModelHitPoint,
                                     AdditionalData,
                                     &ComponentColor);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        Color3AddScaled(Emissive,
                        &ComponentColor,
                        BarycentricCoordinates->Coordinates[Index],
                        Emissive);
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
InterpolatedTriangleDirectShaderShade(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVECTOR3 WorldViewer,
    _In_ PCVECTOR3 ModelViewer,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Out_ PCOLOR3 Direct
    )
{
    PCINTERPOLATED_TRIANGLE_DIRECT_SHADER InterpolatedTriangleDirectShader;
    PCBARYCENTRIC_COORDINATES BarycentricCoordinates;
    PCDIRECT_SHADER DirectShader;
    COLOR3 ComponentColor;
    ISTATUS Status;
    UINT32 Index;

    ASSERT(Context != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(AdditionalData != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(Direct != NULL);

    InterpolatedTriangleDirectShader = (PCINTERPOLATED_TRIANGLE_DIRECT_SHADER) Context;
    BarycentricCoordinates = (PCBARYCENTRIC_COORDINATES) AdditionalData;

    Color3InitializeBlack(Direct);

    for (Index = 0; Index < IRIS_TOOLKIT_TRIANGLE_VERTICES; Index++)
    {
        DirectShader = InterpolatedTriangleDirectShader->DirectShaders[Index];

        if (DirectShader == NULL)
        {
            continue;
        }

        Status = DirectShaderShade(DirectShader,
                                   WorldHitPoint,
                                   ModelHitPoint,
                                   WorldViewer,
                                   ModelViewer,
                                   AdditionalData,
                                   SurfaceNormal,
                                   Rng,
                                   VisibilityTester,
                                   &ComponentColor);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        Color3AddScaled(Direct,
                        &ComponentColor,
                        BarycentricCoordinates->Coordinates[Index],
                        Direct);
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
InterpolatedTriangleIndirectShaderShade(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PSURFACE_NORMAL SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_opt_ PRAYSHADER RayTracer,
    _Out_ PCOLOR3 Indirect
    )
{
    PCINTERPOLATED_TRIANGLE_INDIRECT_SHADER InterpolatedTriangleIndirectShader;
    PCBARYCENTRIC_COORDINATES BarycentricCoordinates;
    PCINDIRECT_SHADER IndirectShader;
    COLOR3 ComponentColor;
    ISTATUS Status;
    UINT32 Index;

    ASSERT(Context != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(AdditionalData != NULL);
    ASSERT(SurfaceNormal != NULL);
    ASSERT(Rng != NULL);
    ASSERT(VisibilityTester != NULL);
    ASSERT(Indirect != NULL);

    InterpolatedTriangleIndirectShader = (PCINTERPOLATED_TRIANGLE_INDIRECT_SHADER) Context;
    BarycentricCoordinates = (PCBARYCENTRIC_COORDINATES) AdditionalData;

    Color3InitializeBlack(Indirect);

    for (Index = 0; Index < IRIS_TOOLKIT_TRIANGLE_VERTICES; Index++)
    {
        IndirectShader = InterpolatedTriangleIndirectShader->IndirectShaders[Index];

        if (IndirectShader == NULL)
        {
            continue;
        }

        Status = IndirectShaderShade(IndirectShader,
                                     WorldHitPoint,
                                     ModelHitPoint,
                                     AdditionalData,
                                     SurfaceNormal,
                                     Rng,
                                     VisibilityTester,
                                     RayTracer,
                                     &ComponentColor);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        Color3AddScaled(Indirect,
                        &ComponentColor,
                        BarycentricCoordinates->Coordinates[Index],
                        Indirect);
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
InterpolatedTriangleTranslucentShaderShade(
    _In_ PCVOID Context,
    _In_ PCPOINT3 WorldHitPoint,
    _In_ PCPOINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PFLOAT Alpha
    )
{
    PCINTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER InterpolatedTriangleTranslucentShader;
    PCBARYCENTRIC_COORDINATES BarycentricCoordinates;
    PCTRANSLUCENT_SHADER TranslucentShader;
    FLOAT Component;
    ISTATUS Status;
    UINT32 Index;

    ASSERT(Context != NULL);
    ASSERT(WorldHitPoint != NULL);
    ASSERT(ModelHitPoint != NULL);
    ASSERT(AdditionalData != NULL);
    ASSERT(Alpha != NULL);

    InterpolatedTriangleTranslucentShader = (PCINTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER) Context;
    BarycentricCoordinates = (PCBARYCENTRIC_COORDINATES) AdditionalData;

    *Alpha = (FLOAT) 0.0;

    for (Index = 0; Index < IRIS_TOOLKIT_TRIANGLE_VERTICES; Index++)
    {
        TranslucentShader = InterpolatedTriangleTranslucentShader->TranslucentShaders[Index];

        if (TranslucentShader == NULL)
        {
            *Alpha = FmaFloat(BarycentricCoordinates->Coordinates[Index],
                              (FLOAT) 1.0,
                              *Alpha);
            continue;
        }

        Status = TranslucentShaderShade(TranslucentShader,
                                        WorldHitPoint,
                                        ModelHitPoint,
                                        AdditionalData,
                                        &Component);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        *Alpha = FmaFloat(BarycentricCoordinates->Coordinates[Index],
                          Component,
                          *Alpha);
    }

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
InterpolatedTriangleNormalComputeNormal(
    _In_ PCVOID Context, 
    _In_ PCPOINT3 HitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCINTERPOLATED_TRIANGLE_NORMAL InterpolatedTriangleNormal;
    PCBARYCENTRIC_COORDINATES BarycentricCoordinates;
    PNORMAL_COMPUTE_NORMAL_ROUTINE ComputeNormalRoutine;
    VECTOR3 ComponentNormal;
    PCNORMAL Normal;
    ISTATUS Status;
    UINT32 Index;

    ASSERT(Context != NULL);
    ASSERT(HitPoint != NULL);
    ASSERT(AdditionalData != NULL);
    ASSERT(SurfaceNormal != NULL);

    InterpolatedTriangleNormal = (PCINTERPOLATED_TRIANGLE_NORMAL) Context;
    BarycentricCoordinates = (PCBARYCENTRIC_COORDINATES) AdditionalData;

    VectorInitialize(SurfaceNormal, (FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);

    for (Index = 0; Index < IRIS_TOOLKIT_TRIANGLE_VERTICES; Index++)
    {
        Normal = InterpolatedTriangleNormal->Normals[Index];
        ComputeNormalRoutine = Normal->NormalVTable->ComputeNormalRoutine;

        Status = ComputeNormalRoutine(Normal,
                                      HitPoint,
                                      AdditionalData,
                                      &ComponentNormal);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        VectorAddScaled(SurfaceNormal,
                        &ComponentNormal,
                        BarycentricCoordinates->Coordinates[Index],
                        SurfaceNormal);
    }

    return ISTATUS_SUCCESS;
}

//
// Static variables
//

CONST STATIC EMISSIVE_SHADER_VTABLE InterpolatedTriangleEmissiveShaderVTable = {
    InterpolatedTriangleEmissiveShaderShade,
    free
};

CONST STATIC DIRECT_SHADER_VTABLE InterpolatedTriangleDirectShaderVTable = {
    InterpolatedTriangleDirectShaderShade,
    free
};

CONST STATIC INDIRECT_SHADER_VTABLE InterpolatedTriangleIndirectShaderVTable = {
    InterpolatedTriangleIndirectShaderShade,
    free
};

CONST STATIC TRANSLUCENT_SHADER_VTABLE InterpolatedTriangleTranslucentShaderVTable = {
    InterpolatedTriangleTranslucentShaderShade,
    free
};

CONST STATIC NORMAL_VTABLE InterpolatedTriangleNormalHeader = {
    InterpolatedTriangleNormalComputeNormal,
    free,
    FALSE
};

//
// Exports
//

_Check_return_
_Ret_maybenull_
PEMISSIVE_SHADER
InterpolatedTriangleEmissiveShaderAllocate(
    _In_ PCEMISSIVE_SHADER Shader0,
    _In_ PCEMISSIVE_SHADER Shader1,
    _In_ PCEMISSIVE_SHADER Shader2
    )
{
    PINTERPOLATED_TRIANGLE_EMISSIVE_SHADER EmissiveShader;

    if (Shader0 == NULL &&
        Shader1 == NULL &&
        Shader2 == NULL)
    {
        return NULL;
    }

    EmissiveShader = (PINTERPOLATED_TRIANGLE_EMISSIVE_SHADER) malloc(sizeof(INTERPOLATED_TRIANGLE_EMISSIVE_SHADER));

    if (EmissiveShader == NULL)
    {
        return NULL;
    }

    EmissiveShader->EmissiveShaderHeader.EmissiveShaderVTable = &InterpolatedTriangleEmissiveShaderVTable;
    EmissiveShader->EmissiveShaders[0] = Shader0;
    EmissiveShader->EmissiveShaders[1] = Shader1;
    EmissiveShader->EmissiveShaders[2] = Shader2;

    return (PEMISSIVE_SHADER) EmissiveShader;
}

_Check_return_
_Ret_maybenull_
PDIRECT_SHADER
InterpolatedTriangleDirectShaderAllocate(
    _In_ PCDIRECT_SHADER Shader0,
    _In_ PCDIRECT_SHADER Shader1,
    _In_ PCDIRECT_SHADER Shader2
    )
{
    PINTERPOLATED_TRIANGLE_DIRECT_SHADER DirectShader;

    if (Shader0 == NULL &&
        Shader1 == NULL &&
        Shader2 == NULL)
    {
        return NULL;
    }

    DirectShader = (PINTERPOLATED_TRIANGLE_DIRECT_SHADER) malloc(sizeof(INTERPOLATED_TRIANGLE_DIRECT_SHADER));

    if (DirectShader == NULL)
    {
        return NULL;
    }

    DirectShader->DirectShaderHeader.DirectShaderVTable = &InterpolatedTriangleDirectShaderVTable;
    DirectShader->DirectShaders[0] = Shader0;
    DirectShader->DirectShaders[1] = Shader1;
    DirectShader->DirectShaders[2] = Shader2;

    return (PDIRECT_SHADER) DirectShader;
}

_Check_return_
_Ret_maybenull_
PINDIRECT_SHADER
InterpolatedTriangleIndirectShaderAllocate(
    _In_ PCINDIRECT_SHADER Shader0,
    _In_ PCINDIRECT_SHADER Shader1,
    _In_ PCINDIRECT_SHADER Shader2
    )
{
    PINTERPOLATED_TRIANGLE_INDIRECT_SHADER IndirectShader;

    if (Shader0 == NULL &&
        Shader1 == NULL &&
        Shader2 == NULL)
    {
        return NULL;
    }

    IndirectShader = (PINTERPOLATED_TRIANGLE_INDIRECT_SHADER) malloc(sizeof(INTERPOLATED_TRIANGLE_INDIRECT_SHADER));

    if (IndirectShader == NULL)
    {
        return NULL;
    }

    IndirectShader->IndirectShaderHeader.IndirectShaderVTable = &InterpolatedTriangleIndirectShaderVTable;
    IndirectShader->IndirectShaders[0] = Shader0;
    IndirectShader->IndirectShaders[1] = Shader1;
    IndirectShader->IndirectShaders[2] = Shader2;

    return (PINDIRECT_SHADER) IndirectShader;
}

_Check_return_
_Ret_maybenull_
PTRANSLUCENT_SHADER
InterpolatedTriangleTranslucentShaderAllocate(
    _In_ PCTRANSLUCENT_SHADER Shader0,
    _In_ PCTRANSLUCENT_SHADER Shader1,
    _In_ PCTRANSLUCENT_SHADER Shader2
    )
{
    PINTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER TranslucentShader;

    if (Shader0 == NULL &&
        Shader1 == NULL &&
        Shader2 == NULL)
    {
        return NULL;
    }

    TranslucentShader = (PINTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER) malloc(sizeof(INTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER));

    if (TranslucentShader == NULL)
    {
        return NULL;
    }

    TranslucentShader->TranslucentShaderHeader.TranslucentShaderVTable = &InterpolatedTriangleTranslucentShaderVTable;
    TranslucentShader->TranslucentShaders[0] = Shader0;
    TranslucentShader->TranslucentShaders[1] = Shader1;
    TranslucentShader->TranslucentShaders[2] = Shader2;

    return (PTRANSLUCENT_SHADER) TranslucentShader;
}

_Check_return_
_Ret_maybenull_
PNORMAL
InterpolatedTriangleNormalAllocate(
    _In_ PCNORMAL Normal0,
    _In_ PCNORMAL Normal1,
    _In_ PCNORMAL Normal2
    )
{
    PINTERPOLATED_TRIANGLE_NORMAL Normal;

    if (Normal0 == NULL ||
        Normal1 == NULL ||
        Normal2 == NULL)
    {
        return NULL;
    }

    Normal = (PINTERPOLATED_TRIANGLE_NORMAL) malloc(sizeof(INTERPOLATED_TRIANGLE_NORMAL));

    if (Normal == NULL)
    {
        return NULL;
    }

    Normal->NormalHeader.NormalVTable = &InterpolatedTriangleNormalHeader;
    Normal->Normals[0] = Normal0;
    Normal->Normals[1] = Normal1;
    Normal->Normals[2] = Normal2;

    return (PNORMAL) Normal;
}