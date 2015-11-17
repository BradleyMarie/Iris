/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    brdf.c

Abstract:

    This file contains the definitions for the BRDF type.

--*/

#include <irisphysxp.h>

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
BrdfAllocate(
    _In_ PCBRDF_VTABLE BrdfVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PBRDF *Brdf
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PBRDF AllocatedBrdf;

    if (BrdfVTable == NULL)
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

    if (Brdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(BRDF),
                                                      _Alignof(BRDF),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedBrdf = (PBRDF) HeaderAllocation;

    AllocatedBrdf->VTable = BrdfVTable;
    AllocatedBrdf->Data = DataAllocation;
    AllocatedBrdf->ReferenceCount = 1;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    *Brdf = AllocatedBrdf;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
BrdfSample(
    _In_ PCBRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 ShapeNormal,
    _In_ VECTOR3 ShadingNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PVECTOR3 Outgoing,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (Brdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (VectorValidate(Incoming) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VectorValidate(ShapeNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (VectorValidate(ShadingNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Rng == NULL)
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

    if (Outgoing == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_07;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_08;
    }

    Status = Brdf->VTable->SampleRoutine(Brdf->Data,
                                         Incoming,
                                         ShapeNormal,
                                         ShadingNormal,
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
BrdfComputeReflectance(
    _In_ PCBRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _In_ VECTOR3 ShadingNormal,
    _In_ VECTOR3 ShapeNormal,
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _Out_ PCREFLECTOR *Reflector,
    _Out_ PFLOAT Pdf
    )
{
    ISTATUS Status;

    if (Brdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (VectorValidate(Incoming) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (VectorValidate(Outgoing) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (VectorValidate(ShadingNormal) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (VectorValidate(ShapeNormal) == FALSE)
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

    Status = Brdf->VTable->ComputeReflectanceRoutine(Brdf->Data,
                                                     Incoming,
                                                     Outgoing,
                                                     ShadingNormal,
                                                     ShapeNormal,
                                                     Compositor,
                                                     Reflector,
                                                     Pdf);

    return Status;
}

VOID
BrdfReference(
    _In_opt_ PBRDF Brdf
    )
{
    if (Brdf == NULL)
    {
        return;
    }

    Brdf->ReferenceCount += 1;
}

VOID
BrdfDereference(
    _In_opt_ _Post_invalid_ PBRDF Brdf
    )
{
    PFREE_ROUTINE FreeRoutine;

    if (Brdf == NULL)
    {
        return;
    }

    Brdf->ReferenceCount -= 1;

    if (Brdf->ReferenceCount == 0)
    {
        FreeRoutine = Brdf->VTable->FreeRoutine;

        if (FreeRoutine != NULL)
        {
            FreeRoutine(Brdf->Data);
        }

        IrisAlignedFree(Brdf);
    }
}