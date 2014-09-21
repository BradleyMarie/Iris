/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    random.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ random number generator type.

--*/

#include <irisplusplus.h>
#include <sstream>

namespace Iris {

//
// Static Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
RandomGenerateFloat(
    _In_ PVOID Context,
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum,
    _Out_range_(Minimum, Maximum) PFLOAT RandomValue
    )
{
    assert(Context != NULL);
    assert(RandomValue != NULL);

    const Random *Rng = static_cast<const Random*>(Context);
    *RandomValue = Rng->GenerateFloat(Minimum, Maximum);
    return ISTATUS_SUCCESS;
}

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
RandomGenerateIndex(
    _In_ PVOID Context,
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum,
    _Out_range_(Minimum, Maximum) PSIZE_T RandomValue
    )
{
    assert(Context != NULL);
    assert(RandomValue != NULL);

    const Random *Rng = static_cast<const Random*>(Context);
    *RandomValue = Rng->GenerateIndex(Minimum, Maximum);
    return ISTATUS_SUCCESS;
}

static
VOID
RandomFree(
    _In_ _Post_invalid_ PVOID Context
    )
{
    assert(Context != NULL);

    Random *Rng = static_cast<Random*>(Context);
    delete Rng;
}

//
// Static Variables
//

const static RANDOM_VTABLE InteropVTable = {
    RandomGenerateFloat,
    RandomGenerateIndex,
    RandomFree
};

//
// Functions
//

Random::Random(
    void
    )
{
    Random *RandomPointer = this;

    PRANDOM Rng = RandomAllocate(&InteropVTable,
                                 &RandomPointer,
                                 sizeof(Random*),
                                 sizeof(Random*));

    if (Rng == NULL)
    {
        throw std::bad_alloc();
    }

    Data = Rng;
}

Random::Random(
    _In_ PRANDOM Rng
    )
: Data(Rng)
{ }

void
Random::Reference(
    void
    )
{
    RandomReference(Data);
}

void 
Random::Dereference(
    void
    )
{
    RandomDereference(Data);
}

CRandom::CRandom(
    _In_ PRANDOM Rng
    )
: Random(Rng), References(1)
{ }

_Ret_range_(Minimum, Maximum)
FLOAT
CRandom::GenerateFloat(
    _In_ FLOAT Minimum,
    _In_ FLOAT Maximum
    ) const
{
    FLOAT RandomNumber;

    ISTATUS Status = RandomGenerateFloat(Data,
                                         Minimum,
                                         Maximum,
                                         &RandomNumber);

    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }

    return RandomNumber;
}

_Ret_range_(Minimum, Maximum)
SIZE_T
CRandom::GenerateIndex(
    _In_ SIZE_T Minimum,
    _In_ SIZE_T Maximum
    ) const
{
    SIZE_T RandomNumber;

    ISTATUS Status = RandomGenerateIndex(Data,
                                         Minimum,
                                         Maximum,
                                         &RandomNumber);
    
    if (Status != ISTATUS_SUCCESS)
    {
        std::ostringstream error;
        error << "Iris Error: " << Status;
        throw std::runtime_error(error.str());
    }

    return RandomNumber;
}

IrisPointer<Random>
CRandom::Create(
    _In_ PRANDOM Rng
    )
{
    if (Rng == NULL)
    {
        throw std::invalid_argument("Rng");
    }

    CRandom* Allocated = new CRandom(Rng);
    return IrisPointer<Random>(Allocated);
}

void
CRandom::Reference(
    void
    )
{
    Random::Reference();
    References += 1;
}

void 
CRandom::Dereference(
    void
    )
{
    Random::Dereference();
    References -= 1;

    if (References == 0)
    {
        delete this;
    }
}

} // namespace Iris