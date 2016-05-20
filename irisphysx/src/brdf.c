/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    brdf.c

Abstract:

    This file contains the definitions for the PBR_BRDF type.

--*/

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrBrdfAllocate(
    _In_ PCPBR_BRDF_VTABLE PbrBrdfVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPBR_BRDF *PbrBrdf
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PPBR_BRDF AllocatedBrdf;

    if (PbrBrdfVTable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (DataSizeInBytes != 0)
    {
        if (Data == NULL)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_00;
        }

        if (DataAlignment == 0 ||
            DataAlignment & DataAlignment - 1)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_01;
        }

        if (DataSizeInBytes % DataAlignment != 0)
        {
            return ISTATUS_INVALID_ARGUMENT_COMBINATION_02;
        }
    }

    if (PbrBrdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(PBR_BRDF),
                                                      _Alignof(PBR_BRDF),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedBrdf = (PPBR_BRDF) HeaderAllocation;

    AllocatedBrdf->VTable = PbrBrdfVTable;
    AllocatedBrdf->Data = DataAllocation;
    AllocatedBrdf->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *PbrBrdf = AllocatedBrdf;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrBrdfSample(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PVECTOR3 Outgoing,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (PbrBrdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (VectorValidate(Incoming) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VectorValidate(SurfaceNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (Outgoing == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    Status = PbrBrdf->VTable->SampleRoutine(PbrBrdf->Data,
                                            Incoming,
                                            SurfaceNormal,
                                            Rng, 
                                            Compositor,
                                            Reflector,
                                            Outgoing,
                                            Pdf);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrBrdfSampleWithLambertianFalloff(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PVECTOR3 Outgoing,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (PbrBrdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (VectorValidate(Incoming) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VectorValidate(SurfaceNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (Outgoing == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    Status = PbrBrdf->VTable->SampleRoutineWithLambertianFalloff(PbrBrdf->Data,
                                                                 Incoming,
                                                                 SurfaceNormal,
                                                                 Rng, 
                                                                 Compositor,
                                                                 Reflector,
                                                                 Outgoing,
                                                                 Pdf);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrBrdfComputeReflectance(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
    )
{
    ISTATUS Status;

    if (PbrBrdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (VectorValidate(Incoming) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VectorValidate(SurfaceNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (VectorValidate(Outgoing) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    Status = PbrBrdf->VTable->ComputeReflectanceRoutine(PbrBrdf->Data,
                                                        Incoming,
                                                        SurfaceNormal,
                                                        Outgoing,
                                                        Compositor,
                                                        Reflector);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrBrdfComputeReflectanceWithLambertianFalloff(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
    )
{
    ISTATUS Status;

    if (PbrBrdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (VectorValidate(Incoming) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VectorValidate(SurfaceNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (VectorValidate(Outgoing) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    Status = PbrBrdf->VTable->ComputeReflectanceRoutineWithLambertianFalloff(PbrBrdf->Data,
                                                                             Incoming,
                                                                             SurfaceNormal,
                                                                             Outgoing,
                                                                             Compositor,
                                                                             Reflector);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrBrdfComputeReflectanceWithPdf(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (PbrBrdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (VectorValidate(Incoming) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VectorValidate(SurfaceNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (VectorValidate(Outgoing) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    Status = PbrBrdf->VTable->ComputeReflectanceWithPdfRoutine(PbrBrdf->Data,
                                                               Incoming,
                                                               SurfaceNormal,
                                                               Outgoing,
                                                               Compositor,
                                                               Reflector,
                                                               Pdf);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PbrBrdfComputeReflectanceWithPdfWithLambertianFalloff(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (PbrBrdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (VectorValidate(Incoming) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VectorValidate(SurfaceNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (VectorValidate(Outgoing) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    Status = PbrBrdf->VTable->ComputeReflectanceWithPdfRoutineWithLambertianFalloff(PbrBrdf->Data,
                                                                                    Incoming,
                                                                                    SurfaceNormal,
                                                                                    Outgoing,
                                                                                    Compositor,
                                                                                    Reflector,
                                                                                    Pdf);

    return Status;
}

VOID
PbrBrdfRetain(
    _In_opt_ PPBR_BRDF PbrBrdf
    )
{
    if (PbrBrdf == NULL)
    {
        return;
    }

    PbrBrdf->ReferenceCount += 1;
}

VOID
PbrBrdfRelease(
    _In_opt_ _Post_invalid_ PPBR_BRDF PbrBrdf
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (PbrBrdf == NULL)
    {
        return;
    }

    PbrBrdf->ReferenceCount -= 1;

    if (PbrBrdf->ReferenceCount == 0)
    {
        FreeRoutine = PbrBrdf->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(PbrBrdf->Data);
        }

        IrisAlignedFree(PbrBrdf);
    }
}