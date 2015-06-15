/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    shader.c

Abstract:

    This file contains the definitions for the SPECTRUM_SHADER type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _SPECTRUM_SHADER {
    PCSPECTRUM_SHADER_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Ret_maybenull_
PSPECTRUM_SHADER
SpectrumShaderAllocate(
    _In_ PCSPECTRUM_SHADER_VTABLE SpectrumShaderVTable,
    _In_reads_bytes_(DataSizeInBytes) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _In_ SIZE_T DataAlignment
    )
{
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PSPECTRUM_SHADER SpectrumShader;

    if (SpectrumShaderVTable == NULL)
    {
        return NULL;
    }

    if (Data == NULL && DataSizeInBytes == 0)
    {
        return NULL;
    }

    HeaderAllocation = IrisAlignedMallocWithHeader(sizeof(SPECTRUM_SHADER),
                                                   sizeof(PVOID),
                                                   DataSizeInBytes,
                                                   DataAlignment,
                                                   &DataAllocation);

    if (HeaderAllocation == NULL)
    {
        return NULL;
    }

    SpectrumShader = (PSPECTRUM_SHADER) HeaderAllocation;

    SpectrumShader->VTable = SpectrumShaderVTable;
    SpectrumShader->Data = DataAllocation;
    SpectrumShader->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    return SpectrumShader;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumShaderForwardShade(
    _In_ PCSPECTRUM_SHADER SpectrumShader,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ RAY WorldXDifferential,
    _In_ RAY WorldYDifferential,
    _In_ FLOAT Distance,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Inout_ PVOID RayTracer,
    _Out_ PCSPECTRUM *Output
    )
{
    ISTATUS Status;
    
    Status = SpectrumShader->VTable->ForwardShadingRoutine(SpectrumShader->Data,
                                                           WorldHitPoint,
                                                           ModelHitPoint,
                                                           WorldViewer,
                                                           WorldXDifferential,
                                                           WorldYDifferential,
                                                           Distance,
                                                           AdditionalData,
                                                           Rng,
                                                           VisibilityTester,
                                                           Compositor,
                                                           RayTracer,
                                                           Output);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
SpectrumShaderBackwardShade(
    _In_ PCSPECTRUM_SHADER SpectrumShader,
    _In_ POINT3 WorldHitPoint,
    _In_ POINT3 ModelHitPoint,
    _In_ VECTOR3 WorldViewer,
    _In_ RAY WorldXDifferential,
    _In_ RAY WorldYDifferential,
    _In_ FLOAT Distance,
    _In_opt_ PCVOID AdditionalData,
    _Inout_ PRANDOM Rng,
    _Inout_ PVISIBILITY_TESTER VisibilityTester,
    _Inout_ PSPECTRUM_COMPOSITOR Compositor,
    _Inout_ PVOID RayTracer,
    _Inout_ PVOID BackwardRayTracer,
    _In_ PCSPECTRUM IncomingLight
    )
{
    ISTATUS Status;
    
    Status = SpectrumShader->VTable->BackwardShadingRoutine(SpectrumShader->Data,
                                                            WorldHitPoint,
                                                            ModelHitPoint,
                                                            WorldViewer,
                                                            WorldXDifferential,
                                                            WorldYDifferential,
                                                            Distance,
                                                            AdditionalData,
                                                            Rng,
                                                            VisibilityTester,
                                                            Compositor,
                                                            BackwardRayTracer,
                                                            IncomingLight);

    return Status;
}

VOID
SpectrumShaderReference(
    _In_opt_ PSPECTRUM_SHADER SpectrumShader
    )
{
    if (SpectrumShader == NULL)
    {
        return;
    }

    SpectrumShader->ReferenceCount += 1;
}

VOID
SpectrumShaderDereference(
    _In_opt_ _Post_invalid_ PSPECTRUM_SHADER SpectrumShader
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (SpectrumShader == NULL)
    {
        return;
    }

    SpectrumShader->ReferenceCount -= 1;

    if (SpectrumShader->ReferenceCount == 0)
    {
        FreeRoutine = SpectrumShader->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(SpectrumShader->Data);
        }

        IrisAlignedFree(SpectrumShader);
    }
}