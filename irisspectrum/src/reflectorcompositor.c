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

typedef struct _FMA_REFLECTOR {
    REFLECTOR ReflectorHeader;
    PCREFLECTOR Reflector0;
    PCREFLECTOR Reflector1;
    FLOAT Attenuation;
} FMA_REFLECTOR, *PFMA_REFLECTOR;

typedef CONST FMA_REFLECTOR *PCFMA_REFLECTOR;

typedef struct _ATTENUATED_REFLECTOR {
    REFLECTOR ReflectorHeader;
    PCREFLECTOR Reflector;
    FLOAT Attenuation;
} ATTENUATED_REFLECTOR, *PATTENUATED_REFLECTOR;

typedef CONST ATTENUATED_REFLECTOR *PCATTENUATED_REFLECTOR;

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
FmaReflectorReflect(
    _In_ PCVOID Context,
    _In_ FLOAT Wavelength,
    _In_ FLOAT IncomingIntensity,
    _Out_ PFLOAT OutgoingIntensity
    )
{
    PCFMA_REFLECTOR FmaReflector;
    FLOAT ReflectorIntensity0;
    FLOAT ReflectorIntensity1;
    ISTATUS Status;

    ASSERT(Context != NULL);
    ASSERT(IsFiniteFloat(Wavelength) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(Wavelength) != FALSE);
    ASSERT(IsFiniteFloat(IncomingIntensity) != FALSE);
    ASSERT(IsGreaterThanZeroFloat(IncomingIntensity) != FALSE);
    ASSERT(OutgoingIntensity != NULL);

    FmaReflector = (PCFMA_REFLECTOR) Context;

    Status = ReflectorReflect(FmaReflector->Reflector0,
                              Wavelength,
                              IncomingIntensity,
                              &ReflectorIntensity0);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    Status = ReflectorReflect(FmaReflector->Reflector1,
                              Wavelength,
                              IncomingIntensity,
                              &ReflectorIntensity1);

    if (Status != ISTATUS_SUCCESS)
    {
        return Status;
    }

    *OutgoingIntensity = FmaFloat(FmaReflector->Attenuation,
                                  ReflectorIntensity1, 
                                  ReflectorIntensity0);

    return ISTATUS_SUCCESS; 
}

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
    ASSERT(IsGreaterThanZeroFloat(IncomingIntensity) != FALSE);
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
    ASSERT(IsGreaterThanZeroFloat(IncomingIntensity) != FALSE);
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
    ASSERT(IsGreaterThanZeroFloat(IncomingIntensity) != FALSE);
    ASSERT(OutgoingIntensity != NULL);

    *OutgoingIntensity = (FLOAT) 0.0;

    return ISTATUS_SUCCESS; 
}

//
// Static Variables
//

CONST STATIC REFLECTOR_VTABLE FmaReflectorVTable = {
    FmaReflectorReflect,
    NULL
};

CONST STATIC REFLECTOR_VTABLE AttenuatedReflectorVTable = {
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
FmaReflectorInitialize(
    _Out_ PFMA_REFLECTOR FmaReflector,
    _In_ PCREFLECTOR Reflector0,
    _In_ PCREFLECTOR Reflector1,
    _In_ FLOAT Attenuation
    )
{
    ASSERT(FmaReflector != NULL);
    ASSERT(Reflector0 != NULL);
    ASSERT(Reflector1 != NULL);
    ASSERT(IsFiniteFloat(Attenuation) != FALSE);
    ASSERT(IsNotZeroFloat(Attenuation) != FALSE);

    ReflectorInitialize(&FmaReflectorVTable,
                        FmaReflector,
                        &FmaReflector->ReflectorHeader);
    
    FmaReflector->Reflector0 = Reflector0;
    FmaReflector->Reflector1 = Reflector1;
    FmaReflector->Attenuation = Attenuation;
}

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
        return Status;
    }

    Status = StaticMemoryAllocatorInitialize(&Compositor->FmaReflectorAllocator,
                                             sizeof(FMA_REFLECTOR));

    if (Status != ISTATUS_SUCCESS)
    {
        StaticMemoryAllocatorDestroy(&Compositor->AttenuatedReflectorAllocator);
        return Status;
    }

    Status = StaticMemoryAllocatorInitialize(&Compositor->SumReflectorAllocator,
                                             sizeof(SUM_REFLECTOR));

    if (Status != ISTATUS_SUCCESS)
    {
        StaticMemoryAllocatorDestroy(&Compositor->AttenuatedReflectorAllocator);
        StaticMemoryAllocatorDestroy(&Compositor->FmaReflectorAllocator);
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
    StaticMemoryAllocatorFreeAll(&Compositor->FmaReflectorAllocator);
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
    StaticMemoryAllocatorDestroy(&Compositor->FmaReflectorAllocator);
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
    PATTENUATED_REFLECTOR AttenuatedReflector;
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

    if (Reflector0->VTable == &AttenuatedReflectorVTable)
    {
        AttenuatedReflector = (PATTENUATED_REFLECTOR) Reflector0;

        Status = ReflectorCompositorReferenceAttenuatedAddReflections(Compositor,
                                                                      Reflector1,
                                                                      AttenuatedReflector->Reflector,
                                                                      AttenuatedReflector->Attenuation,
                                                                      Sum);
                                                           
        return Status;
    }

    if (Reflector1->VTable == &AttenuatedReflectorVTable)
    {
        AttenuatedReflector = (PATTENUATED_REFLECTOR) Reflector1;

        Status = ReflectorCompositorReferenceAttenuatedAddReflections(Compositor,
                                                                      Reflector0,
                                                                      AttenuatedReflector->Reflector,
                                                                      AttenuatedReflector->Attenuation,
                                                                      Sum);
        return Status;
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

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if(IsFiniteFloat(Attenuation) == FALSE);
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
        Reflector = ReflectorAsAttenuatedReflector->Reflector;
        Attenuation *= ReflectorAsAttenuatedReflector->Attenuation;
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
ReflectorCompositorReferenceAttenuatedAddReflections(
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _In_opt_ PCREFLECTOR Reflector0,
    _In_opt_ PCREFLECTOR Reflector1,
    _In_ FLOAT Attenuation,
    _Out_ PCREFLECTOR *AttenuatedSum
    )
{
    PATTENUATED_REFLECTOR AttenuatedReflector;
    PFMA_REFLECTOR FmaReflector;
    PVOID Allocation;
    ISTATUS Status;

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if(IsFiniteFloat(Attenuation) == FALSE);
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (AttenuatedSum == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Reflector0 == NULL)
    {
        Status = ReflectorCompositorReferenceAttenuateReflection(Compositor,
                                                                 Reflector1,
                                                                 Attenuation,
                                                                 AttenuatedSum);
        return Status;
    }

    if (Reflector1 == NULL ||
        IsNotZeroFloat(Attenuation) == FALSE)
    {
        *AttenuatedSum = Reflector0;
        return ISTATUS_SUCCESS;
    }

    if (Attenuation == (FLOAT) 1.0)
    {
        Status = ReflectorCompositorReferenceAddReflections(Compositor,
                                                            Reflector0,
                                                            Reflector1,
                                                            AttenuatedSum);   
        return Status;
    }

    if (Reflector1->VTable == &AttenuatedReflectorVTable)
    {
        AttenuatedReflector = (PATTENUATED_REFLECTOR) Reflector1;
        Reflector1 = AttenuatedReflector->Reflector;
        Attenuation *= AttenuatedReflector->Attenuation;
    }

    Allocation = StaticMemoryAllocatorAllocate(&Compositor->FmaReflectorAllocator);

    if (Allocation == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    FmaReflector = (PFMA_REFLECTOR) Allocation;

    FmaReflectorInitialize(FmaReflector,
                           Reflector0,
                           Reflector1,
                           Attenuation);

    *AttenuatedSum = (PCREFLECTOR) FmaReflector;
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

    Compositor = (PREFLECTOR_COMPOSITOR) malloc(sizeof(PREFLECTOR_COMPOSITOR));

    if (Compositor == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    Status = ReflectorCompositorReferenceInitialize(&Compositor->CompositorReference);

    if (Status != ISTATUS_SUCCESS)
    {
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

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
ReflectorCompositorAttenuatedAddReflections(
    _Inout_ PREFLECTOR_COMPOSITOR Compositor,
    _In_opt_ PCREFLECTOR Reflector0,
    _In_opt_ PCREFLECTOR Reflector1,
    _In_ FLOAT Attenuation,
    _Out_ PCREFLECTOR *AttenuatedSum
    )
{
    ISTATUS Status;
    
    //
    // &Compositor->CompositorReference should be safe to do even if
    // Compositor == NULL.
    //
    
    Status = ReflectorCompositorReferenceAttenuatedAddReflections(&Compositor->CompositorReference,
                                                                  Reflector0,
                                                                  Reflector1,
                                                                  Attenuation,
                                                                  AttenuatedSum);
    
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