/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    brdf.c

Abstract:

    This file contains the definitions for the PHYSX_BRDF type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _PHYSX_BRDF {
    PCPHYSX_BRDF_VTABLE VTable;
    SIZE_T ReferenceCount;
    PVOID Data;
};

//
// Internal Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxBrdfAllocateUsingAllocator(
    _Inout_ PDYNAMIC_MEMORY_ALLOCATOR Allocator,
    _In_ PCPHYSX_BRDF_VTABLE BrdfVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PCPHYSX_BRDF *Brdf
    )
{
    PPHYSX_BRDF AllocatedBrdf;
    PVOID DataDestination;
    PVOID Header;
    
    if (Allocator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (BrdfVTable == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
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
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    Header = DynamicMemoryAllocatorAllocateWithHeader(Allocator,
                                                      sizeof(PHYSX_BRDF),
                                                      _Alignof(PHYSX_BRDF),
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataDestination);
                                                      
    if (Header == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }
    
    AllocatedBrdf = (PPHYSX_BRDF) Header;
    
    AllocatedBrdf->VTable = BrdfVTable;
    AllocatedBrdf->ReferenceCount = 0;
    AllocatedBrdf->Data = DataDestination;
    
    memcpy(DataDestination, Data, DataSizeInBytes);
    
    *Brdf = AllocatedBrdf;

    return ISTATUS_SUCCESS;
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
PhysxBrdfAllocate(
    _In_ PCPHYSX_BRDF_VTABLE BrdfVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PPHYSX_BRDF *Brdf
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PPHYSX_BRDF AllocatedBrdf;

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

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(PHYSX_BRDF),
                                                      _Alignof(PHYSX_BRDF),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedBrdf = (PPHYSX_BRDF) HeaderAllocation;

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
PhysxBrdfSample(
    _In_ PCPHYSX_BRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
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

    Status = Brdf->VTable->SampleRoutine(Brdf->Data,
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
PhysxBrdfSampleWithLambertianFalloff(
    _In_ PCPHYSX_BRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _Inout_ PRANDOM Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
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

    Status = Brdf->VTable->SampleRoutineWithLambertianFalloff(Brdf->Data,
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
PhysxBrdfComputeReflectance(
    _In_ PCPHYSX_BRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
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

    Status = Brdf->VTable->ComputeReflectanceRoutine(Brdf->Data,
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
PhysxBrdfComputeReflectanceWithLambertianFalloff(
    _In_ PCPHYSX_BRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR *Reflector
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

    Status = Brdf->VTable->ComputeReflectanceRoutineWithLambertianFalloff(Brdf->Data,
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
PhysxBrdfComputeReflectanceWithPdf(
    _In_ PCPHYSX_BRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
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

    Status = Brdf->VTable->ComputeReflectanceWithPdfRoutine(Brdf->Data,
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
PhysxBrdfComputeReflectanceWithPdfWithLambertianFalloff(
    _In_ PCPHYSX_BRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 SurfaceNormal,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
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

    Status = Brdf->VTable->ComputeReflectanceWithPdfRoutineWithLambertianFalloff(Brdf->Data,
                                                                                 Incoming,
                                                                                 SurfaceNormal,
                                                                                 Outgoing,
                                                                                 Compositor,
                                                                                 Reflector,
                                                                                 Pdf);

    return Status;
}

VOID
PhysxBrdfRetain(
    _In_opt_ PPHYSX_BRDF Brdf
    )
{
    if (Brdf == NULL)
    {
        return;
    }

    Brdf->ReferenceCount += 1;
}

VOID
PhysxBrdfRelease(
    _In_opt_ _Post_invalid_ PPHYSX_BRDF Brdf
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
