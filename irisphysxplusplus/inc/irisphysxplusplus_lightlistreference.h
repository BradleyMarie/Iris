/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_lightlistreference.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ LightListReference type.

--*/

#include <irisphysxplusplus.h>

#ifndef _LIGHT_LIST_REFERENCE_IRIS_PHYSX_PLUS_PLUS_
#define _LIGHT_LIST_REFERENCE_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class LightListReference final {
public:
    LightListReference(
        _In_ PCPHYSX_LIGHT_LIST Lights
        )
    : Data(Lights)
    { 
        if (Lights == nullptr)
        {
            throw std::invalid_argument("Lights");
        }
    }
    
    _Ret_
    PCPHYSX_LIGHT_LIST
    AsPCPHYSX_LIGHT_LIST(
        void
        ) const
    {
        return Data;
    }

    SIZE_T
    Size(
        void
        ) const
    {
        SIZE_T Result;

        PhysxLightListGetSize(Data, &Result);

        return Result;
    }

    LightReference
    Get(
        _In_ SIZE_T Index
        ) const
    {
        PCPHYSX_LIGHT LightPtr;

        ISTATUS Status = PhysxLightListGetLight(Data,
                                                Index,
                                                &LightPtr);

        if (Status != ISTATUS_SUCCESS)
        {
            ASSERT(Status == ISTATUS_INVALID_ARGUMENT_01);
            throw std::invalid_argument("Index");
        }

        return LightReference(LightPtr);
    }

    LightReference
    GetRandom(
        _In_ IrisAdvanced::RandomReference Rng
        ) const
    {
        PCPHYSX_LIGHT LightPtr;

        ISTATUS Status = PhysxLightListGetRandomLight(Data,
                                                      Rng.AsPRANDOM_REFERENCE(),
                                                      &LightPtr);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        return LightReference(LightPtr);
    }

private:
    PCPHYSX_LIGHT_LIST Data;
};

} // namespace Iris

#endif // _LIGHT_LIST_REFERENCE_IRIS_PHYSX_PLUS_PLUS_
