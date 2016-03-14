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
BrdfSample(
    _In_ PCBRDF Brdf,
    _In_ VECTOR3 Incoming,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR_REFERENCE *Reflector,
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

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Outgoing == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    Status = Brdf->VTable->SampleRoutine(Brdf->Data,
                                         Incoming,
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
BrdfSampleWithLambertianFalloff(
    _In_ PCBRDF Brdf,
    _In_ VECTOR3 Incoming,
    _Inout_ PRANDOM_REFERENCE Rng,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR_REFERENCE *Reflector,
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

    if (Rng == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Outgoing == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_06;
    }

    Status = Brdf->VTable->SampleRoutineWithLambertianFalloff(Brdf->Data,
                                                              Incoming,
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
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR_REFERENCE *Reflector
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

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    Status = Brdf->VTable->ComputeReflectanceRoutine(Brdf->Data,
                                                     Incoming,
                                                     Outgoing,
                                                     Compositor,
                                                     Reflector);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
BrdfComputeReflectanceWithLambertianFalloff(
    _In_ PCBRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR_REFERENCE *Reflector
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

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    Status = Brdf->VTable->ComputeReflectanceRoutineWithLambertianFalloff(Brdf->Data,
                                                                          Incoming,
                                                                          Outgoing,
                                                                          Compositor,
                                                                          Reflector);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
BrdfComputeReflectanceWithPdf(
    _In_ PCBRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR_REFERENCE *Reflector,
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

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    Status = Brdf->VTable->ComputeReflectanceWithPdfRoutine(Brdf->Data,
                                                            Incoming,
                                                            Outgoing,
                                                            Compositor,
                                                            Reflector,
                                                            Pdf);

    return Status;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
BrdfComputeReflectanceWithPdfWithLambertianFalloff(
    _In_ PCBRDF Brdf,
    _In_ VECTOR3 Incoming,
    _In_ VECTOR3 Outgoing,
    _Inout_ PREFLECTOR_COMPOSITOR_REFERENCE Compositor,
    _Out_ PCREFLECTOR_REFERENCE *Reflector,
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

    if (Compositor == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Reflector == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_04;
    }

    if (Pdf == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_05;
    }

    Status = Brdf->VTable->ComputeReflectanceWithPdfRoutineWithLambertianFalloff(Brdf->Data,
                                                                                 Incoming,
                                                                                 Outgoing,
                                                                                 Compositor,
                                                                                 Reflector,
                                                                                 Pdf);

    return Status;
}