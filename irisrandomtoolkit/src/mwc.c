/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    mwc.c

Abstract:

    This file contains the definitions for the MWC_RNG type.

--*/

#include <irisrandomtoolkit.h>
#include <time.h>
#include <math.h>

//
// Constants
//

#define RANDOM_STATE_SIZE 4096
#define INITIAL_C_VALUE 362436
#define INITIAL_I_VALUE 4095
#define PHI 0x9e3779b9

//
// Types
//

typedef struct _MWC_RNG {
    _Field_size_(RANDOM_STATE_SIZE) PUINT32 RngState;
    UINT32 C;
    SIZE_T I;
} MWC_RNG, *PMWC_RNG;

typedef CONST MWC_RNG *PCMWC_RNG;

//
// Static functions
//

SFORCEINLINE
UINT32
MultiplyWithCarryGenerateRandom(
    _In_ PMWC_RNG Rng
    )
{
    SIZE_T Index;
    UINT32 Ret;
    UINT64 T;

    ASSERT(Rng != NULL);

    Index = Rng->I = (Rng->I + 1) & INITIAL_I_VALUE;
    T = ((UINT64) 18705 * (UINT64) Rng->RngState[Index]) + (UINT64) Rng->C;
    
    Rng->C = (UINT32) (T >> 32);
    Ret = Rng->RngState[Index] = (UINT32) 0xfffffffe - (UINT32) T;

    return Ret;
}

STATIC
ISTATUS
MultiplyWithCarryRngGenerateFloat(
    _In_ PVOID Rng,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum,
    _Out_range_(Minimum, Maximum) PFLOAT RandomValue
    )
{
    PMWC_RNG MwcRng;
    UINT32 Rand;
    FLOAT RandomFloat;

    ASSERT(Rng != NULL);
    ASSERT(IsFiniteFloat(Minimum));
    ASSERT(IsFiniteFloat(Maximum));
    ASSERT(Minimum < Maximum);
    ASSERT(RandomValue != NULL);

    MwcRng = (PMWC_RNG) Rng;

    Rand = MultiplyWithCarryGenerateRandom(MwcRng);

    RandomFloat = (FLOAT) Rand / (FLOAT) UINT32_MAX;

    *RandomValue = RandomFloat * (Maximum - Minimum) + Minimum;

    return ISTATUS_SUCCESS;
}

STATIC
ISTATUS
MultiplyWithCarryRngGenerateIndex(
    _In_ PVOID Rng,
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum,
    _Out_range_(Minimum, Maximum) PSIZE_T RandomValue
    )
{
    PMWC_RNG MwcRng;
    UINT32 Rand;

    ASSERT(Rng != NULL);
    ASSERT(Minimum < Maximum);
    ASSERT(RandomValue != NULL);

    MwcRng = (PMWC_RNG) Rng;

    Rand = MultiplyWithCarryGenerateRandom(MwcRng);

    *RandomValue = (SIZE_T) ((Rand % (Maximum - Minimum)) + Minimum);

    return ISTATUS_SUCCESS;
}

VOID
MultiplyWithCarryRngFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    PMWC_RNG Rng;

    ASSERT(Context != NULL);

    Rng = (PMWC_RNG) Context;

    free(Rng->RngState);
}

//
// Static variables
//

CONST STATIC RANDOM_VTABLE MwcVTable = {
    MultiplyWithCarryRngGenerateFloat,
    MultiplyWithCarryRngGenerateIndex,
    MultiplyWithCarryRngFree
};

//
// Public Functions
//

_Check_return_
_Ret_maybenull_
ISTATUS
MultiplyWithCarryRngAllocate(
    _Out_ PRANDOM *Random
    )
{
    SIZE_T Index;
    MWC_RNG Rng;
    PUINT32 RngState;
    double Seconds;
    UINT32 Seed;
    ISTATUS Status;
    time_t Timer;
    struct tm YearTwoThousand;

    RngState = (PUINT32) malloc(sizeof(UINT32) * RANDOM_STATE_SIZE);

    if (RngState == NULL)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    YearTwoThousand.tm_hour = 0;
    YearTwoThousand.tm_min = 0;
    YearTwoThousand.tm_sec = 0;
    YearTwoThousand.tm_year = 100;
    YearTwoThousand.tm_mon = 0;
    YearTwoThousand.tm_mday = 1;

    time(&Timer);

    Seconds = difftime(Timer, mktime(&YearTwoThousand));

    Seed = (UINT32) (Seconds * 1000.0);

    RngState[0] = Seed;
    RngState[1] = Seed + PHI;
    RngState[2] = Seed + PHI + PHI;

    for (Index = 3; Index < RANDOM_STATE_SIZE; Index++)
    {
        RngState[Index] = RngState[Index - 3] ^ 
                          RngState[Index - 2] ^ 
                          PHI ^ (UINT32) Index;
    }

    Rng.RngState = RngState;
    Rng.C = INITIAL_C_VALUE;
    Rng.I = INITIAL_I_VALUE;

    Status = RandomAllocate(&MwcVTable,
                            &Rng,
                            sizeof(MWC_RNG),
                            sizeof(PVOID),
                            Random);

    if (Status != ISTATUS_SUCCESS)
    {
        MultiplyWithCarryRngFree(&Rng);
    }

    return Status;
}