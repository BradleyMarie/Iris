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
    SPECTRUM_RAYTRACER_OWNER RayTracerOwner;
    BRDF_ALLOCATOR BrdfAllocator;
    SPECTRUM_VISIBILITY_TESTER_OWNER VisibilityTesterOwner;
    PSPECTRUM_COMPOSITOR_OWNER SpectrumCompositorOwner;
    PREFLECTOR_COMPOSITOR_OWNER ReflectorCompositorOwner;
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

    Status = SpectrumRayTracerOwnerInitialize(&AllocatedIntegrator->RayTracerOwner);

    if (Status != ISTATUS_SUCCESS)
    {
        IrisAlignedFree(HeaderAllocation);
        return Status;
    }

    Status = SpectrumCompositorOwnerAllocate(&AllocatedIntegrator->SpectrumCompositorOwner);

    if (Status != ISTATUS_SUCCESS)
    {
        SpectrumRayTracerOwnerDestroy(&AllocatedIntegrator->RayTracerOwner);
        IrisAlignedFree(HeaderAllocation);
        return Status;
    }

    Status = ReflectorCompositorOwnerAllocate(&AllocatedIntegrator->ReflectorCompositorOwner);

    if (Status != ISTATUS_SUCCESS)
    {
        SpectrumCompositorOwnerFree(AllocatedIntegrator->SpectrumCompositorOwner);
        SpectrumRayTracerOwnerDestroy(&AllocatedIntegrator->RayTracerOwner);
        IrisAlignedFree(HeaderAllocation);
        return Status;
    }

    Status = SpectrumVisibilityTesterOwnerInitialize(&AllocatedIntegrator->VisibilityTesterOwner);

    if (Status != ISTATUS_SUCCESS)
    {
        ReflectorCompositorOwnerFree(AllocatedIntegrator->ReflectorCompositorOwner);
        SpectrumCompositorOwnerFree(AllocatedIntegrator->SpectrumCompositorOwner);
        SpectrumRayTracerOwnerDestroy(&AllocatedIntegrator->RayTracerOwner);
        IrisAlignedFree(HeaderAllocation);
        return Status;
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
    _In_ PCSPECTRUM_SCENE SpectrumScene,
    _In_ FLOAT VisibilityEpsilon,
    _In_ RAY WorldRay,
    _Inout_ PRANDOM Rng,
    _Out_ PSPECTRUM *Spectrum
    )
{
    PREFLECTOR_COMPOSITOR ReflectorCompositor;
    PSPECTRUM_COMPOSITOR SpectrumCompositor;
    PSPECTRUM_VISIBILITY_TESTER VisibilityTester;
    PSPECTRUM_RAYTRACER RayTracer;
    SIZE_T NumberOfLights;
    PCLIGHT *Lights;
    ISTATUS Status;
    PCSCENE Scene;

    if (Integrator == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (SpectrumScene == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (IsFiniteFloat(VisibilityEpsilon) == FALSE ||
        IsGreaterThanOrEqualToZeroFloat(VisibilityEpsilon) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (RayValidate(WorldRay) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Spectrum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    Scene = (PCSCENE) SpectrumScene;

    Status = SpectrumVisibilityTesterOwnerGetVisibilityTester(&Integrator->VisibilityTesterOwner,
                                                              Scene,
                                                              VisibilityEpsilon,
                                                              &VisibilityTester);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    BrdfAllocatorFreeAll(&Integrator->BrdfAllocator);
    SpectrumCompositorOwnerClear(Integrator->SpectrumCompositorOwner);
    ReflectorCompositorOwnerClear(Integrator->ReflectorCompositorOwner);

    ReflectorCompositor = ReflectorCompositorOwnerGetCompositor(Integrator->ReflectorCompositorOwner);
    SpectrumCompositor = SpectrumCompositorOwnerGetCompositor(Integrator->SpectrumCompositorOwner);

    SpectrumRayTracerOwnerGetRayTracer(&Integrator->RayTracerOwner,
                                       SpectrumScene,
                                       &RayTracer);

    SpectrumSceneGetLights(SpectrumScene,
                           &Lights,
                           &NumberOfLights);

    WorldRay = RayNormalize(WorldRay);

    Status = Integrator->VTable->IntegrateRoutine(Integrator->Data,
                                                  WorldRay,
                                                  Lights,
                                                  NumberOfLights,
                                                  RayTracer,
                                                  VisibilityTester,
                                                  &Integrator->BrdfAllocator,
                                                  SpectrumCompositor,
                                                  ReflectorCompositor,
                                                  Rng,
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

    SpectrumRayTracerOwnerDestroy(&Integrator->RayTracerOwner);
    BrdfAllocatorDestroy(&Integrator->BrdfAllocator);
    SpectrumCompositorOwnerFree(Integrator->SpectrumCompositorOwner);
    ReflectorCompositorOwnerFree(Integrator->ReflectorCompositorOwner);
    SpectrumVisibilityTesterOwnerDestroy(&Integrator->VisibilityTesterOwner);
    IrisAlignedFree(Integrator);
}