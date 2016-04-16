/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    reflectorcompositorreference.c

Abstract:

    This file contains the function definitions 
    for the REFLECTOR_COMPOSITOR type.

--*/

#include <irisspectrump.h>

//
// Types
//

typedef struct _SUM_REFLECTOR {
    REFLECTOR ReflectorHeader;
    PCREFLECTOR Reflector0;
    PCREFLECTOR Reflector1;
} SUM_REFLECTOR, *PSUM_REFLECTOR;

typedef CONST SUM_REFLECTOR *PCSUM_REFLECTOR;

struct _REFLECTOR_COMPOSITOR_REFERENCE {
    STATIC_MEMORY_ALLOCATOR AttenuatedReflectorAllocator;
    STATIC_MEMORY_ALLOCATOR FmaReflectorAllocator;
    STATIC_MEMORY_ALLOCATOR SumReflectorAllocator;
};

struct _REFLECTOR_COMPOSITOR {
    REFLECTOR_COMPOSITOR_REFERENCE CompositorReference;
};

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
AttenuatedReflectorReflect(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity,
    _Out_ PFLOAT OutgoingIntensity
    )
{
    PCATTENUATED_REFLECTOR AttenuatedReflector;
    FLOAT OutputIntensity;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(Wavelength) != FALSE);
    ASSERT(IsFiniteFloat(IncomingIntensity) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(IncomingIntensity) != FALSE);
    ASSERT(OutgoingIntensity != NULL);

    AttenuatedReflector = (PCATTENUATED_REFLECTOR) Context;

    Status = ReflectorReflect(AttenuatedReflector->Reflector,
                              Wavelength,
                              IncomingIntensity,
                              &OutputIntensity);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *OutgoingIntensity = OutputIntensity * AttenuatedReflector->Attenuation;
    return ISTATUS_SUCCESS; 
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
SumReflectorReflect(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity,
    _Out_ PFLOAT OutgoingIntensity
    )
{
    PCSUM_REFLECTOR SumReflector;
    FLOAT ReflectorIntensity0;
    FLOAT ReflectorIntensity1;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(Wavelength) != FALSE);
    ASSERT(IsFiniteFloat(IncomingIntensity) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(IncomingIntensity) != FALSE);
    ASSERT(OutgoingIntensity != NULL);

    SumReflector = (PCSUM_REFLECTOR) Context;

    Status = ReflectorReflect(SumReflector->Reflector0,
                              Wavelength,
                              IncomingIntensity,
                              &ReflectorIntensity0);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = ReflectorReflect(SumReflector->Reflector1,
                              Wavelength,
                              IncomingIntensity,
                              &ReflectorIntensity1);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *OutgoingIntensity = ReflectorIntensity0 + ReflectorIntensity1;
    return ISTATUS_SUCCESS; 
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
STATIC
ISTATUS
ZeroReflectorReflect(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity,
    _Out_ PFLOAT OutgoingIntensity
    )
{
    ASSERT(Context == NULL);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(Wavelength) != FALSE);
    ASSERT(IsFiniteFloat(IncomingIntensity) != FALSE);
    ASSERT(IsGreaterThanOrEqualToZeroFloat(IncomingIntensity) != FALSE);
    ASSERT(OutgoingIntensity != NULL);

    *OutgoingIntensity = (FLOAT) 0.0;

    return ISTATUS_SUCCESS; 
}

//
// Variables
//

CONST REFLECTOR_VTABLE AttenuatedReflectorVTable = {
    AttenuatedReflectorReflect,
    NULL
};

CONST STATIC REFLECTOR_VTABLE SumReflectorVTable = {
    SumReflectorReflect,
    NULL
};

//
// Initialization Functions
//

STATIC
VOID
AttenuatedReflectorInitialize(
    _Out_ PATTENUATED_REFLECTOR AttenuatedReflector,
    _In_ PCREFLECTOR Reflector,
    _In_ FLOAT Attenuation
    )
{
    ASSERT(AttenuatedReflector != NULL);
    ASSERT(Reflector != NULL);
    ASSERT(IsFiniteFloat(Attenuation) != FALSE);
    ASSERT(IsNotZeroFloat(Attenuation) != FALSE);

    ReflectorInitialize(&AttenuatedReflectorVTable,
                        AttenuatedReflector,
                        &AttenuatedReflector->ReflectorHeader);

    AttenuatedReflector->Reflector = Reflector;
    AttenuatedReflector->Attenuation = Attenuation;
}

STATIC
VOID
SumReflectorInitialize(
    _Out_ PSUM_REFLECTOR SumReflector,
    _In_ PCREFLECTOR Reflector0,
    _In_ PCREFLECTOR Reflector1
    )
{
    ASSERT(SumReflector != NULL);
    ASSERT(Reflector0 != NULL);
    ASSERT(Reflector1 != NULL);

    ReflectorInitialize(&SumReflectorVTable,
                        SumReflector,
                        &SumReflector->ReflectorHeader);

    SumReflector->Reflector0 = Reflector0;
    SumReflector->Reflector1 = Reflector1;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
SFORCEINLINE
ISTATUS
ReflectorCompositorReferenceInitialize(
    _Out_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor
    )
{
    ISTATUS Status;

    ASSERT(Compositor != NULL);

    Status = StaticMemoryAllocatorInitialize(&Compositor->AttenuatedReflectorAllocator,
                                             sizeof(ATTENUATED_REFLECTOR));

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    Status = StaticMemoryAllocatorInitialize(&Compositor->SumReflectorAllocator,
                                             sizeof(SUM_REFLECTOR));

    if (Status != ISTATUS_SUCCESS)
    {
        StaticMemoryAllocatorDestroy(&Compositor->AttenuatedReflectorAllocator);
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    return ISTATUS_SUCCESS;    
}

SFORCEINLINE
VOID
ReflectorCompositorReferenceClear(
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor
    )
{
    ASSERT(Compositor != NULL);

    StaticMemoryAllocatorFreeAll(&Compositor->AttenuatedReflectorAllocator);
    StaticMemoryAllocatorFreeAll(&Compositor->SumReflectorAllocator);
}

SFORCEINLINE
VOID
ReflectorCompositorReferenceDestroy(
    _In_ _Post_invalid_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor
    )
{
    ASSERT(Compositor != NULL);

    StaticMemoryAllocatorDestroy(&Compositor->AttenuatedReflectorAllocator);
    StaticMemoryAllocatorDestroy(&Compositor->SumReflectorAllocator);
}

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ReflectorCompositorReferenceAddReflections(
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCREFLECTOR Reflector0,
    _In_opt_ PCREFLECTOR Reflector1,
    _Out_ PCREFLECTOR *Sum
    )
{
    PCATTENUATED_REFLECTOR AttenuatedReflector0;
    PCATTENUATED_REFLECTOR AttenuatedReflector1;
    PCSUM_REFLECTOR ConstSumReflector0;
    PCSUM_REFLECTOR ConstSumReflector1;
    PCREFLECTOR IntermediateReflector0;
    PCREFLECTOR IntermediateReflector1;
    PSUM_REFLECTOR SumReflector;
    PVOID Allocation;
    ISTATUS Status;

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Sum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Reflector0 == NULL)
    {
        *Sum = Reflector1;
        return ISTATUS_SUCCESS;
    }

    if (Reflector1 == NULL)
    {
        *Sum = Reflector0;
        return ISTATUS_SUCCESS;
    }

    if (Reflector0 == Reflector1)
    {
        Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                                 Reflector0,
                                                                 (FLOAT) 2.0, 
                                                                 Sum);

        ASSERT(Status == ISTATUS_SUCCESS || Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    if (Reflector0->VTable == &AttenuatedReflectorVTable)
    {
        AttenuatedReflector0 = (PCATTENUATED_REFLECTOR) Reflector0;

        if (Reflector1->VTable == &AttenuatedReflectorVTable)
        {
            AttenuatedReflector1 = (PCATTENUATED_REFLECTOR) Reflector1;

            if (AttenuatedReflector0->Reflector == AttenuatedReflector1->Reflector)
            {
                Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                                         Reflector0,
                                                                         AttenuatedReflector0->Attenuation + AttenuatedReflector1->Attenuation,
                                                                         Sum);
            }

            if (AttenuatedReflector0->Attenuation == AttenuatedReflector1->Attenuation)
            {
                Status = ReflectorCompositorReferenceAddReflections(Compositor,
                                                                    AttenuatedReflector0->Reflector,
                                                                    AttenuatedReflector1->Reflector,
                                                                    &IntermediateReflector0);

                if (Status != ISTATUS_SUCCESS)
                {
                    ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
                    return Status;
                }

                Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                                         IntermediateReflector0,
                                                                         AttenuatedReflector0->Attenuation,
                                                                         Sum);

                ASSERT(Status == ISTATUS_SUCCESS || Status == ISTATUS_ALLOCATION_FAILED);
                return Status;
            }
        }
    }
    else if (Reflector0->VTable == &SumReflectorVTable)
    {
        ConstSumReflector0 = (PCSUM_REFLECTOR) Reflector0;

        if (Reflector0->VTable == &SumReflectorVTable)
        {
            ConstSumReflector1 = (PCSUM_REFLECTOR) Reflector1;

            if (ConstSumReflector0->Reflector0 == ConstSumReflector1->Reflector0)
            {
                if (ConstSumReflector0->Reflector1 == ConstSumReflector1->Reflector1)
                {
                    Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                                             Reflector0,
                                                                             (FLOAT) 2.0,
                                                                             Sum);

                    ASSERT(Status == ISTATUS_SUCCESS || Status == ISTATUS_ALLOCATION_FAILED);
                    return Status;
                }
                else
                {
                    Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                                             ConstSumReflector0->Reflector0,
                                                                             (FLOAT) 2.0,
                                                                             &IntermediateReflector0);

                    if (Status != ISTATUS_SUCCESS)
                    {
                        ASSERT(Status == ISTATUS_SUCCESS);
                        return Status;
                    }

                    Status = ReflectorCompositorReferenceAddReflections(Compositor,
                                                                        ConstSumReflector0->Reflector1,
                                                                        ConstSumReflector1->Reflector1,
                                                                        &IntermediateReflector1);

                    if (Status != ISTATUS_SUCCESS)
                    {
                        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
                        return Status;
                    }

                    Status = ReflectorCompositorReferenceAddReflections(Compositor,
                                                                        IntermediateReflector0,
                                                                        IntermediateReflector1,
                                                                        Sum);

                    ASSERT(Status == ISTATUS_SUCCESS || Status == ISTATUS_ALLOCATION_FAILED);
                    return Status;
                }
            }
            else if (ConstSumReflector0->Reflector0 == ConstSumReflector1->Reflector1)
            {
                if (ConstSumReflector0->Reflector1 == ConstSumReflector1->Reflector0)
                {
                    Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                                             Reflector0,
                                                                             (FLOAT) 2.0,
                                                                             Sum);

                    ASSERT(Status == ISTATUS_SUCCESS || Status == ISTATUS_ALLOCATION_FAILED);
                    return Status;
                }
                else
                {
                    Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                                             ConstSumReflector0->Reflector0,
                                                                             (FLOAT) 2.0,
                                                                             &IntermediateReflector0);

                    if (Status != ISTATUS_SUCCESS)
                    {
                        ASSERT(Status == ISTATUS_SUCCESS);
                        return Status;
                    }

                    Status = ReflectorCompositorReferenceAddReflections(Compositor,
                                                                        ConstSumReflector0->Reflector1,
                                                                        ConstSumReflector1->Reflector0,
                                                                        &IntermediateReflector1);

                    if (Status != ISTATUS_SUCCESS)
                    {
                        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
                        return Status;
                    }

                    Status = ReflectorCompositorReferenceAddReflections(Compositor,
                                                                        IntermediateReflector0,
                                                                        IntermediateReflector1,
                                                                        Sum);

                    ASSERT(Status == ISTATUS_SUCCESS || Status == ISTATUS_ALLOCATION_FAILED);
                    return Status;
                }
            }
            else if (ConstSumReflector0->Reflector1 == ConstSumReflector1->Reflector1)
            {
                Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                                         ConstSumReflector0->Reflector1,
                                                                         (FLOAT) 2.0,
                                                                         &IntermediateReflector0);

                if (Status != ISTATUS_SUCCESS)
                {
                    ASSERT(Status == ISTATUS_SUCCESS);
                    return Status;
                }

                Status = ReflectorCompositorReferenceAddReflections(Compositor,
                                                                    ConstSumReflector0->Reflector0,
                                                                    ConstSumReflector1->Reflector0,
                                                                    &IntermediateReflector1);

                if (Status != ISTATUS_SUCCESS)
                {
                    ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
                    return Status;
                }

                Status = ReflectorCompositorReferenceAddReflections(Compositor,
                                                                    IntermediateReflector0,
                                                                    IntermediateReflector1,
                                                                    Sum);

                ASSERT(Status == ISTATUS_SUCCESS || Status == ISTATUS_ALLOCATION_FAILED);
                return Status;
            }
            else if (ConstSumReflector0->Reflector1 == ConstSumReflector1->Reflector0)
            {
                Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                                         ConstSumReflector0->Reflector1,
                                                                         (FLOAT) 2.0,
                                                                         &IntermediateReflector0);

                if (Status != ISTATUS_SUCCESS)
                {
                    ASSERT(Status == ISTATUS_SUCCESS);
                    return Status;
                }

                Status = ReflectorCompositorReferenceAddReflections(Compositor,
                                                                    ConstSumReflector0->Reflector0,
                                                                    ConstSumReflector1->Reflector1,
                                                                    &IntermediateReflector1);

                if (Status != ISTATUS_SUCCESS)
                {
                    ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
                    return Status;
                }

                Status = ReflectorCompositorReferenceAddReflections(Compositor,
                                                                    IntermediateReflector0,
                                                                    IntermediateReflector1,
                                                                    Sum);

                ASSERT(Status == ISTATUS_SUCCESS || Status == ISTATUS_ALLOCATION_FAILED);
                return Status;
            }
        }
    }

    Allocation = StaticMemoryAllocatorAllocate(&Compositor->SumReflectorAllocator);

    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    SumReflector = (PSUM_REFLECTOR) Allocation;

    SumReflectorInitialize(SumReflector,
                           Reflector0,
                           Reflector1);

    *Sum = (PCREFLECTOR) SumReflector;
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ReflectorCompositorReferenceAttenuateReflection(
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCREFLECTOR Reflector,
    _In_ FLOAT Attenuation,
    _Out_ PCREFLECTOR *AttenuatedReflectorOutput
    )
{
    PATTENUATED_REFLECTOR AttenuatedReflector;
    PATTENUATED_REFLECTOR ReflectorAsAttenuatedReflector;
    PVOID Allocation;
    ISTATUS Status;

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if(IsFiniteFloat(Attenuation) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (AttenuatedReflectorOutput == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Reflector == NULL ||
        IsNotZeroFloat(Attenuation) == FALSE)
    {
        *AttenuatedReflectorOutput = NULL;
        return ISTATUS_SUCCESS;
    }

    if (Attenuation == (FLOAT) 1.0)
    {
        *AttenuatedReflectorOutput = Reflector;
        return ISTATUS_SUCCESS;
    }

    if (Reflector->VTable == &AttenuatedReflectorVTable)
    {
        ReflectorAsAttenuatedReflector = (PATTENUATED_REFLECTOR) Reflector;

        Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                                 ReflectorAsAttenuatedReflector->Reflector,
                                                                 Attenuation * ReflectorAsAttenuatedReflector->Attenuation,
                                                                 AttenuatedReflectorOutput);
        
        ASSERT(Status == ISTATUS_SUCCESS || Status == ISTATUS_ALLOCATION_FAILED);
        return Status;
    }

    Allocation = StaticMemoryAllocatorAllocate(&Compositor->AttenuatedReflectorAllocator);

    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AttenuatedReflector = (PATTENUATED_REFLECTOR) Allocation;

    AttenuatedReflectorInitialize(AttenuatedReflector,
                                  Reflector,
                                  Attenuation);

    *AttenuatedReflectorOutput = (PCREFLECTOR) AttenuatedReflector;
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ReflectorCompositorAllocate(
    _Out_ PREFLECTOR_COMPOSITOR *Result
    )
{
    PREFLECTOR_COMPOSITOR Compositor;
    ISTATUS Status;

    if (Result == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    Compositor = (PREFLECTOR_COMPOSITOR) malloc(sizeof(REFLECTOR_COMPOSITOR));

    if (Compositor == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = ReflectorCompositorReferenceInitialize(&Compositor->CompositorReference);

    if (Status != ISTATUS_SUCCESS)
    {
        ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
        free(Compositor);
        return Status;
    }

    *Result = Compositor;

    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ReflectorCompositorAddReflections(
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _In_opt_ PCREFLECTOR Reflector0,
    _In_opt_ PCREFLECTOR Reflector1,
    _Out_ PCREFLECTOR *Sum
    )
{
    ISTATUS Status;
    
    //
    // &Compositor->CompositorReference should be safe to do even if
    // Compositor == NULL.
    //
    
    Status = ReflectorCompositorReferenceAddReflections(&Compositor->CompositorReference,
                                                        Reflector0,
                                                        Reflector1,
                                                        Sum);
    
    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ReflectorCompositorAttenuateReflection(
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _In_opt_ PCREFLECTOR Reflector,
    _In_ FLOAT Attenuation,
    _Out_ PCREFLECTOR *AttenuatedReflectorOutput
    )
{
    ISTATUS Status;
    
    //
    // &Compositor->CompositorReference should be safe to do even if
    // Compositor == NULL.
    //
    
    Status = ReflectorCompositorReferenceAttenuateReflection(&Compositor->CompositorReference,
                                                             Reflector,
                                                             Attenuation,
                                                             AttenuatedReflectorOutput);
    
    return Status;
}

_Ret_
PREFLECTOR_COMPOSITOR_REFERENCE
ReflectorCompositorGetReflectorCompositorReference(
    _In_ PREFLECTOR_COMPOSITOR Compositor
    )
{
    if (Compositor == NULL)
    {
        return NULL;
    }

    return &Compositor->CompositorReference;
}

VOID
ReflectorCompositorClear(
    _Inout_ PREFLECTOR_COMPOSITOR Compositor
    )
{
    if (Compositor == NULL)
    {
        return;
    }

    ReflectorCompositorReferenceClear(&Compositor->CompositorReference);
}

VOID
ReflectorCompositorFree(
    _In_opt_ _Post_invalid_ PREFLECTOR_COMPOSITOR Compositor
    )
{
    if (Compositor == NULL)
    {
        return;
    }

    ReflectorCompositorReferenceDestroy(&Compositor->CompositorReference);
}