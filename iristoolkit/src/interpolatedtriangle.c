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
    PEMISSIVE_SHADER EmissiveShaders[IRIS_TOOLKIT_TRIANGLE_VERTICES];
} INTERPOLATED_TRIANGLE_EMISSIVE_SHADER, *PINTERPOLATED_TRIANGLE_EMISSIVE_SHADER;

typedef CONST INTERPOLATED_TRIANGLE_EMISSIVE_SHADER *PCINTERPOLATED_TRIANGLE_EMISSIVE_SHADER;

typedef struct _INTERPOLATED_TRIANGLE_DIRECT_SHADER {
    PDIRECT_SHADER DirectShaders[IRIS_TOOLKIT_TRIANGLE_VERTICES];
} INTERPOLATED_TRIANGLE_DIRECT_SHADER, *PINTERPOLATED_TRIANGLE_DIRECT_SHADER;

typedef CONST INTERPOLATED_TRIANGLE_DIRECT_SHADER *PCINTERPOLATED_TRIANGLE_DIRECT_SHADER;

typedef struct _INTERPOLATED_TRIANGLE_INDIRECT_SHADER {
    PINDIRECT_SHADER IndirectShaders[IRIS_TOOLKIT_TRIANGLE_VERTICES];
} INTERPOLATED_TRIANGLE_INDIRECT_SHADER, *PINTERPOLATED_TRIANGLE_INDIRECT_SHADER;

typedef CONST INTERPOLATED_TRIANGLE_INDIRECT_SHADER *PCINTERPOLATED_TRIANGLE_INDIRECT_SHADER;

typedef struct _INTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER {
    PTRANSLUCENT_SHADER TranslucentShaders[IRIS_TOOLKIT_TRIANGLE_VERTICES];
} INTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER, *PINTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER;

typedef CONST INTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER *PCINTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER;

typedef struct _INTERPOLATED_TRIANGLE_NORMAL {
    PNORMAL Normals[IRIS_TOOLKIT_TRIANGLE_VERTICES];
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
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_ PCVOID AdditionalData,
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
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ VECTOR3 ModelViewer,
    _In_ PCVOID AdditionalData,
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
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ VECTOR3 ModelViewer,
    _In_ PCVOID AdditionalData,
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
                                     WorldViewer,
                                     ModelViewer,
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
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
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
    _In_ POINT3 HitPoint,
    _In_ PCVOID AdditionalData,
    _Out_ PVECTOR3 SurfaceNormal
    )
{
    PCINTERPOLATED_TRIANGLE_NORMAL InterpolatedTriangleNormal;
    PCBARYCENTRIC_COORDINATES BarycentricCoordinates;
    VECTOR3 ComponentNormal;
    PCNORMAL Normal;
    ISTATUS Status;
    UINT32 Index;

    ASSERT(Context != NULL);
    ASSERT(AdditionalData != NULL);
    ASSERT(SurfaceNormal != NULL);

    InterpolatedTriangleNormal = (PCINTERPOLATED_TRIANGLE_NORMAL) Context;
    BarycentricCoordinates = (PCBARYCENTRIC_COORDINATES) AdditionalData;

    *SurfaceNormal = VectorCreate((FLOAT) 0.0, (FLOAT) 0.0, (FLOAT) 0.0);

    for (Index = 0; Index < IRIS_TOOLKIT_TRIANGLE_VERTICES; Index++)
    {
        Normal = InterpolatedTriangleNormal->Normals[Index];

        Status = NormalComputeNormal(Normal,
                                     HitPoint,
                                     AdditionalData,
                                     &ComponentNormal);

        if (Status != ISTATUS_SUCCESS)
        {
            return Status;
        }

        *SurfaceNormal = VectorAddScaled(*SurfaceNormal,
                                         ComponentNormal,
                                         BarycentricCoordinates->Coordinates[Index]);
    }

    return ISTATUS_SUCCESS;
}

VOID
InterpolatedTriangleEmissiveShaderFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PINTERPOLATED_TRIANGLE_EMISSIVE_SHADER InterpolatedEmissiveShader;

    ASSERT(Context != NULL);

    InterpolatedEmissiveShader = (PINTERPOLATED_TRIANGLE_EMISSIVE_SHADER) Context;

    EmissiveShaderDereference(InterpolatedEmissiveShader->EmissiveShaders[0]);
    EmissiveShaderDereference(InterpolatedEmissiveShader->EmissiveShaders[1]);
    EmissiveShaderDereference(InterpolatedEmissiveShader->EmissiveShaders[2]);
}

VOID
InterpolatedTriangleDirectShaderFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PINTERPOLATED_TRIANGLE_DIRECT_SHADER InterpolatedDirectShader;

    ASSERT(Context != NULL);

    InterpolatedDirectShader = (PINTERPOLATED_TRIANGLE_DIRECT_SHADER) Context;

    DirectShaderDereference(InterpolatedDirectShader->DirectShaders[0]);
    DirectShaderDereference(InterpolatedDirectShader->DirectShaders[1]);
    DirectShaderDereference(InterpolatedDirectShader->DirectShaders[2]);
}

VOID
InterpolatedTriangleIndirectShaderFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PINTERPOLATED_TRIANGLE_INDIRECT_SHADER InterpolatedIndirectShader;

    ASSERT(Context != NULL);

    InterpolatedIndirectShader = (PINTERPOLATED_TRIANGLE_INDIRECT_SHADER) Context;

    IndirectShaderDereference(InterpolatedIndirectShader->IndirectShaders[0]);
    IndirectShaderDereference(InterpolatedIndirectShader->IndirectShaders[1]);
    IndirectShaderDereference(InterpolatedIndirectShader->IndirectShaders[2]);
}

VOID
InterpolatedTriangleTranslucentShaderFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PINTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER InterpolatedTranslucentShader;

    ASSERT(Context != NULL);

    InterpolatedTranslucentShader = (PINTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER) Context;

    TranslucentShaderDereference(InterpolatedTranslucentShader->TranslucentShaders[0]);
    TranslucentShaderDereference(InterpolatedTranslucentShader->TranslucentShaders[1]);
    TranslucentShaderDereference(InterpolatedTranslucentShader->TranslucentShaders[2]);
}

VOID
InterpolatedTriangleNormalFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PINTERPOLATED_TRIANGLE_NORMAL InterpolatedNormal;

    ASSERT(Context != NULL);

    InterpolatedNormal = (PINTERPOLATED_TRIANGLE_NORMAL) Context;

    NormalDereference(InterpolatedNormal->Normals[0]);
    NormalDereference(InterpolatedNormal->Normals[1]);
    NormalDereference(InterpolatedNormal->Normals[2]);
}

//
// Static variables
//

CONST STATIC EMISSIVE_SHADER_VTABLE InterpolatedTriangleEmissiveShaderVTable = {
    InterpolatedTriangleEmissiveShaderShade,
    InterpolatedTriangleEmissiveShaderFree
};

CONST STATIC DIRECT_SHADER_VTABLE InterpolatedTriangleDirectShaderVTable = {
    InterpolatedTriangleDirectShaderShade,
    InterpolatedTriangleDirectShaderFree
};

CONST STATIC INDIRECT_SHADER_VTABLE InterpolatedTriangleIndirectShaderVTable = {
    InterpolatedTriangleIndirectShaderShade,
    InterpolatedTriangleIndirectShaderFree
};

CONST STATIC TRANSLUCENT_SHADER_VTABLE InterpolatedTriangleTranslucentShaderVTable = {
    InterpolatedTriangleTranslucentShaderShade,
    InterpolatedTriangleTranslucentShaderFree
};

CONST STATIC NORMAL_VTABLE InterpolatedTriangleNormalVTable = {
    InterpolatedTriangleNormalComputeNormal,
    InterpolatedTriangleNormalFree,
    FALSE
};

//
// Exports
//

_Check_return_
_Ret_maybenull_
PEMISSIVE_SHADER
InterpolatedTriangleEmissiveShaderAllocate(
    _In_ PEMISSIVE_SHADER Shader0,
    _In_ PEMISSIVE_SHADER Shader1,
    _In_ PEMISSIVE_SHADER Shader2
    )
{
    INTERPOLATED_TRIANGLE_EMISSIVE_SHADER InterpolatedEmissiveShader;
    PEMISSIVE_SHADER EmissiveShader;

    if (Shader0 == NULL &&
        Shader1 == NULL &&
        Shader2 == NULL)
    {
        return NULL;
    }

    InterpolatedEmissiveShader.EmissiveShaders[0] = Shader0;
    InterpolatedEmissiveShader.EmissiveShaders[1] = Shader1;
    InterpolatedEmissiveShader.EmissiveShaders[2] = Shader2;

    EmissiveShader = EmissiveShaderAllocate(&InterpolatedTriangleEmissiveShaderVTable,
                                            &InterpolatedEmissiveShader,
                                            sizeof(INTERPOLATED_TRIANGLE_EMISSIVE_SHADER),
                                            sizeof(PVOID));

    if (EmissiveShader != NULL)
    {
        EmissiveShaderReference(Shader0);
        EmissiveShaderReference(Shader1);
        EmissiveShaderReference(Shader2);
    }

    return EmissiveShader;
}

_Check_return_
_Ret_maybenull_
PDIRECT_SHADER
InterpolatedTriangleDirectShaderAllocate(
    _In_ PDIRECT_SHADER Shader0,
    _In_ PDIRECT_SHADER Shader1,
    _In_ PDIRECT_SHADER Shader2
    )
{
    INTERPOLATED_TRIANGLE_DIRECT_SHADER InterpolatedDirectShader;
    PDIRECT_SHADER DirectShader;

    if (Shader0 == NULL &&
        Shader1 == NULL &&
        Shader2 == NULL)
    {
        return NULL;
    }

    InterpolatedDirectShader.DirectShaders[0] = Shader0;
    InterpolatedDirectShader.DirectShaders[1] = Shader1;
    InterpolatedDirectShader.DirectShaders[2] = Shader2;

    DirectShader = DirectShaderAllocate(&InterpolatedTriangleDirectShaderVTable,
                                        &InterpolatedDirectShader,
                                        sizeof(INTERPOLATED_TRIANGLE_DIRECT_SHADER),
                                        sizeof(PVOID));

    if (DirectShader != NULL)
    {
        DirectShaderReference(Shader0);
        DirectShaderReference(Shader1);
        DirectShaderReference(Shader2);
    }

    return DirectShader;
}

_Check_return_
_Ret_maybenull_
PINDIRECT_SHADER
InterpolatedTriangleIndirectShaderAllocate(
    _In_ PINDIRECT_SHADER Shader0,
    _In_ PINDIRECT_SHADER Shader1,
    _In_ PINDIRECT_SHADER Shader2
    )
{
    INTERPOLATED_TRIANGLE_INDIRECT_SHADER InterpolatedIndirectShader;
    PINDIRECT_SHADER IndirectShader;

    if (Shader0 == NULL &&
        Shader1 == NULL &&
        Shader2 == NULL)
    {
        return NULL;
    }

    InterpolatedIndirectShader.IndirectShaders[0] = Shader0;
    InterpolatedIndirectShader.IndirectShaders[1] = Shader1;
    InterpolatedIndirectShader.IndirectShaders[2] = Shader2;

    IndirectShader = IndirectShaderAllocate(&InterpolatedTriangleIndirectShaderVTable,
                                            &InterpolatedIndirectShader,
                                            sizeof(INTERPOLATED_TRIANGLE_INDIRECT_SHADER),
                                            sizeof(PVOID));

    if (IndirectShader != NULL)
    {
        IndirectShaderReference(Shader0);
        IndirectShaderReference(Shader1);
        IndirectShaderReference(Shader2);
    }

    return IndirectShader;
}

_Check_return_
_Ret_maybenull_
PTRANSLUCENT_SHADER
InterpolatedTriangleTranslucentShaderAllocate(
    _In_ PTRANSLUCENT_SHADER Shader0,
    _In_ PTRANSLUCENT_SHADER Shader1,
    _In_ PTRANSLUCENT_SHADER Shader2
    )
{
    INTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER InterpolatedShader;
    PTRANSLUCENT_SHADER TranslucentShader;

    if (Shader0 == NULL &&
        Shader1 == NULL &&
        Shader2 == NULL)
    {
        return NULL;
    }

    InterpolatedShader.TranslucentShaders[0] = Shader0;
    InterpolatedShader.TranslucentShaders[1] = Shader1;
    InterpolatedShader.TranslucentShaders[2] = Shader2;

    TranslucentShader = TranslucentShaderAllocate(&InterpolatedTriangleTranslucentShaderVTable,
                                                  &InterpolatedShader,
                                                  sizeof(INTERPOLATED_TRIANGLE_TRANSLUCENT_SHADER),
                                                  sizeof(PCTRANSLUCENT_SHADER));

    if (TranslucentShader != NULL)
    {
        TranslucentShaderReference(Shader0);
        TranslucentShaderReference(Shader1);
        TranslucentShaderReference(Shader2);
    }

    return TranslucentShader;
}

_Check_return_
_Ret_maybenull_
PNORMAL
InterpolatedTriangleNormalAllocate(
    _In_ PNORMAL Normal0,
    _In_ PNORMAL Normal1,
    _In_ PNORMAL Normal2
    )
{
    INTERPOLATED_TRIANGLE_NORMAL InterpolatedNormal;
    PNORMAL Normal;

    if (Normal0 == NULL ||
        Normal1 == NULL ||
        Normal2 == NULL)
    {
        return NULL;
    }

    InterpolatedNormal.Normals[0] = Normal0;
    InterpolatedNormal.Normals[1] = Normal1;
    InterpolatedNormal.Normals[2] = Normal2;

    Normal = NormalAllocate(&InterpolatedTriangleNormalVTable,
                            &InterpolatedNormal,
                            sizeof(INTERPOLATED_TRIANGLE_NORMAL),
                            sizeof(PVOID));

    if (Normal != NULL)
    {
        NormalReference(Normal0);
        NormalReference(Normal1);
        NormalReference(Normal2);
    }

    return Normal;
}