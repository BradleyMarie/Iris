/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    mwcgenerator.c

Abstract:

    This file contains the definitions for the MWC_RNG_GENERATOR type.

--*/

#include <iriscameratoolkit.h>
#include <irisadvancedtoolkit.h>

//
// Static functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
MultiplyWithCarryGeneratorGenerate(
    _In_ PCVOID Context,
    _Out_ PRANDOM *Rng
    )
{
    ISTATUS Status;

    ASSERT(Context == NULL);
    ASSERT(Rng != NULL);

    Status = MultiplyWithCarryRngAllocate(Rng);

    return Status;
}

//
// Static variables
//

CONST STATIC RANDOM_GENERATOR_VTABLE MwcGeneratorVTable = {
    MultiplyWithCarryGeneratorGenerate,
    NULL
};

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERATOOLKITAPI
ISTATUS
MultiplyWithCarryGeneratorAllocate(
    _Out_ PRANDOM_GENERATOR *RngGenerator
    )
{
    ISTATUS Status;

    if (RngGenerator != NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    Status = RandomGeneratorAllocate(&MwcGeneratorVTable,
                                     NULL,
                                     0,
                                     0,
                                     RngGenerator);
    
    return Status;
}
