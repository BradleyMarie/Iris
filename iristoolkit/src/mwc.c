/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    mwc.c

Abstract:

    This file contains the definitions for the MWC_RNG type.

--*/

#include <iristoolkitp.h>
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
    RANDOM RandomHeader;
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

_Ret_range_(Minimum, Maximum)
SFORCEINLINE
FLOAT
MultiplyWithCarryRngGenerateFloat(
    _In_ PVOID Rng,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum
    )
{
    PMWC_RNG MwcRng;
    UINT32 RandomValue;
    FLOAT RandomFloat;

    ASSERT(Rng != NULL);
    ASSERT(IsNormalFloat(Minimum));
    ASSERT(IsFiniteFloat(Minimum));
    ASSERT(IsNormalFloat(Maximum));
    ASSERT(IsFiniteFloat(Minimum));
    ASSERT(Minimum < Maximum);

    MwcRng = (PMWC_RNG) Rng;

    RandomValue = MultiplyWithCarryGenerateRandom(MwcRng);

    RandomFloat = (FLOAT) RandomValue / (FLOAT) UINT32_MAX;

    return RandomFloat * (Maximum - Minimum) + Minimum;
}

_Ret_range_(Minimum, Maximum) 
SFORCEINLINE
SIZE_T
MultiplyWithCarryRngGenerateIndex(
    _In_ PVOID Rng,
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum
    )
{
    PMWC_RNG MwcRng;
    UINT32 RandomValue;

    ASSERT(Rng != NULL);
    ASSERT(Minimum < Maximum);

    MwcRng = (PMWC_RNG) Rng;

    RandomValue = MultiplyWithCarryGenerateRandom(MwcRng);

    return (SIZE_T) ((RandomValue % (Maximum - Minimum)) + Minimum);
}

VOID
MultiplyWithCarryRngFree(
    _Pre_maybenull_ _Post_invalid_ PVOID Pointer
    )
{
    PMWC_RNG Rng;

    if (Pointer == NULL)
    {
        return;
    }

    Rng = (PMWC_RNG) Pointer;

    free(Rng->RngState);
    free(Rng);
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
IRISTOOLKITAPI
PRANDOM
MultiplyWithCarryRngAllocate(
    VOID
    )
{
    PUINT32 RngState;
    PMWC_RNG Rng;
    SIZE_T Index;
    UINT32 Seed;
    time_t Timer;
    struct tm YearTwoThousand;
    double Seconds;

    RngState = (PUINT32) malloc(sizeof(UINT32) * RANDOM_STATE_SIZE);

    if (RngState == NULL)
    {
        return NULL;
    }

    Rng = (PMWC_RNG) malloc(sizeof(MWC_RNG));

    if (Rng == NULL)
    {
        free(RngState);
        return NULL;
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
                          PHI ^ Index;
    }

    Rng->RandomHeader.RandomVTable = &MwcVTable;
    Rng->RngState = RngState;
    Rng->C = INITIAL_C_VALUE;
    Rng->I = INITIAL_I_VALUE;

    return (PRANDOM) Rng;
}