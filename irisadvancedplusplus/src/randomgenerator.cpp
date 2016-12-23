/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    randomgenerator.cpp

Abstract:

    This file contains the definitions for the 
    IrisAdvanced++ RandomGenerator type.

--*/

#include <irisadvancedplusplusp.h>

namespace IrisAdvanced {

//
// Adapter Functions
//

_Success_(return == ISTATUS_SUCCESS)
static
VOID
RandomGeneratorGenerateAdapter(
    _Inout_opt_ PVOID Context,
    _In_ PRANDOM RngPtr
    )
{
    assert(Context != nullptr);
    assert(RngPtr != nullptr);

    auto Callback = static_cast<std::function<void(Random &)> *>(Context);
    Random Rng(RngPtr);
    (*Callback)(Rng);
}

//
// Functions
//

void
RandomGenerator::Generate(
    _In_ std::function<void(Random &)> & Callback
    ) const
{
    ISTATUS Status = RandomGeneratorGenerate(Data.get(),
                                             RandomGeneratorGenerateAdapter,
                                             &Callback);
    
    if (Status != ISTATUS_SUCCESS)
    {
        assert(Status == ISTATUS_ALLOCATION_FAILED);
        throw std::bad_alloc();
    }
}

} // namespace IrisAdvanced