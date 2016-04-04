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
PbrBrdfSample(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
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

    Status = PbrBrdf->VTable->SampleRoutine(PbrBrdf->Data,
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
PbrBrdfSampleWithLambertianFalloff(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
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

    Status = PbrBrdf->VTable->SampleRoutineWithLambertianFalloff(PbrBrdf->Data,
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
PbrBrdfComputeReflectance(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
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

    Status = PbrBrdf->VTable->ComputeReflectanceRoutine(PbrBrdf->Data,
                                                        Incoming,
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

    Status = PbrBrdf->VTable->ComputeReflectanceRoutineWithLambertianFalloff(PbrBrdf->Data,
                                                                             Incoming,
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

    Status = PbrBrdf->VTable->ComputeReflectanceWithPdfRoutine(PbrBrdf->Data,
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
PbrBrdfComputeReflectanceWithPdfWithLambertianFalloff(
    _In_ PCPBR_BRDF PbrBrdf,
    _In_ VECTOR3 Incoming,
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

    Status = PbrBrdf->VTable->ComputeReflectanceWithPdfRoutineWithLambertianFalloff(PbrBrdf->Data,
                                                                                    Incoming,
                                                                                    Outgoing,
                                                                                    Compositor,
                                                                                    Reflector,
                                                                                    Pdf);

    return Status;
}