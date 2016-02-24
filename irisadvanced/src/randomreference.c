/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    randomreference.c

Abstract:

    This file contains the definitions for the RANDOM_REFERENCE type.

--*/

#define _IRIS_ADVANCED_EXPORT_RANDOM_REFERENCE_ROUTINES_
#include <irisadvancedp.h>

ISTATUS
RandomReferenceGenerateFloat(
    _In_ PRANDOM_REFERENCE Rng,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum,
    _Out_range_(Minimum, Maximum) PFLOAT RandomValue
    )
{
    ISTATUS Status;
    
    Status = StaticRandomReferenceGenerateFloat(Rng,
                                                Minimum,
                                                Maximum,
                                                RandomValue);
                                          
    return Status;
}

ISTATUS
RandomReferenceGenerateIndex(
    _In_ PRANDOM_REFERENCE Rng,
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum,
    _Out_range_(Minimum, Maximum) PSIZE_T RandomValue
    )
{
    ISTATUS Status;
    
    Status = StaticRandomReferenceGenerateIndex(Rng,
                                                Minimum,
                                                Maximum,
                                                RandomValue);

    return Status;
}