/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisphysx_integrator.h

Abstract:

    This file contains the definitions for the INTEGRATOR type.

--*/

#include <irisphysxp.h>

//
// Types
//

struct _INTEGRATOR {
    PCINTEGRATOR_VTABLE VTable;
    SPECTRUM_RAYTRACER RayTracer;
    BRDF_ALLOCATOR BrdfAllocator;
    PSPECTRUM_COMPOSITOR SpectrumCompositor;
    PVOID Data;
};

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
IntegratorAllocate(
    _In_ PCINTEGRATOR_VTABLE IntegratorVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PINTEGRATOR *Integrator
    )
{
    BOOL AllocationSuccessful;
    PVOID HeaderAllocation;
    PVOID DataAllocation;
    PINTEGRATOR AllocatedIntegrator;
    ISTATUS Status;

    if (IntegratorVTable == NULL)
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

    if (Integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(INTEGRATOR),
                                                      _Alignof(INTEGRATOR),
                                                      &HeaderAllocation,
                                                      DataSizeInBytes,
                                                      DataAlignment,
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedIntegrator = (PINTEGRATOR) HeaderAllocation;

    AllocatedIntegrator->VTable = IntegratorVTable;
    AllocatedIntegrator->Data = DataAllocation;

    if (DataSizeInBytes != 0)
    {
        memcpy(DataAllocation, Data, DataSizeInBytes);
    }

    Status = SpectrumRayTracerInitialize(&AllocatedIntegrator->RayTracer);

    if (Status != ISTATUS_SUCCESS)
    {
        IrisAlignedFree(HeaderAllocation);
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedIntegrator->SpectrumCompositor = SpectrumCompositorAllocate();

    if (AllocatedIntegrator->SpectrumCompositor == NULL)
    {
        SpectrumRayTracerDestroy(&AllocatedIntegrator->RayTracer);
        IrisAlignedFree(HeaderAllocation);
        return ISTATUS_ALLOCATION_FAILED;
    }

    BrdfAllocatorInitialize(&AllocatedIntegrator->BrdfAllocator);

    *Integrator = AllocatedIntegrator;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
IntegratorIntegrate(
    _In_ PINTEGRATOR Integrator,
    _In_ RAY WorldRay,
    _Out_ PSPECTRUM *Spectrum
    )
{
    ISTATUS Status;

    if (Integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (RayValidate(WorldRay) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (Spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    BrdfAllocatorFreeAll(&Integrator->BrdfAllocator);
    SpectrumCompositorClear(Integrator->SpectrumCompositor);

    Status = Integrator->VTable->IntegrateRoutine(Integrator->Data,
                                                  WorldRay,
                                                  &Integrator->RayTracer,
                                                  &Integrator->BrdfAllocator,
                                                  Integrator->SpectrumCompositor,
                                                  Spectrum);

    return Status;
}

VOID
IntegratorFree(
    _In_opt_ _Post_invalid_ PINTEGRATOR Integrator
    )
{
    if (Integrator == NULL)
    {
        return;
    }

    SpectrumRayTracerDestroy(&Integrator->RayTracer);
    BrdfAllocatorDestroy(&Integrator->BrdfAllocator);
    SpectrumCompositorFree(Integrator->SpectrumCompositor);
    IrisAlignedFree(Integrator);
}