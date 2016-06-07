/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_lightlist.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ LightList type.

--*/

#include <irisphysxplusplus.h>

#ifndef _LIGHT_LIST_IRIS_PHYSX_PLUS_PLUS_
#define _LIGHT_LIST_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class LightList final {
public:
    LightList(
        _In_ LightList && ToMove
        )
    : Data(ToMove.Data)
    {
        ToMove.Data = nullptr;
    }
    
    static
    LightList
    Create(
        void
        )
    { 
        return LightList();
    }

    _Ret_
    PPHYSX_LIGHT_LIST
    AsPPHYSX_LIGHT_LIST(
        void
        )
    {
        return Data;
    }
    
    _Ret_
    PCPHYSX_LIGHT_LIST
    AsPCPHYSX_LIGHT_LIST(
        void
        ) const
    {
        return Data;
    }

    LightListReference
    AsLightListReference(
        void
        )
    {
        return LightListReference(Data);
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
        PCPBR_LIGHT LightPtr;

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
        PCPBR_LIGHT LightPtr;

        ISTATUS Status = PhysxLightListGetRandomLight(Data,
                                                      Rng.AsPRANDOM_REFERENCE(),
                                                      &LightPtr);

        if (Status != ISTATUS_SUCCESS)
        {
            throw std::runtime_error(Iris::ISTATUSToCString(Status));
        }

        return LightReference(LightPtr);
    }

    void
    Add(
        _In_ Light LightRef
        )
    {
        ISTATUS Status = PhysxLightListAddLight(Data,
                                                LightRef.AsPPBR_LIGHT());

        switch (Status)
        {
            case ISTATUS_SUCCESS:
                break;
            case ISTATUS_ALLOCATION_FAILED:
                throw std::bad_alloc();
            case ISTATUS_INVALID_ARGUMENT_COMBINATION_00:
                throw std::invalid_argument("LightRef is already in List");
        }
        
        assert(Status == ISTATUS_SUCCESS);
    }

    void
    Clear(
        void
        )
    {
        PhysxLightListClear(Data);
    }

    LightList(
        _In_ LightList & ToCopy
        ) = delete;
        
    LightList &
    operator=(
        _In_ const LightList & ToCopy
        ) = delete;
    
    ~LightList(
        void
        )
    {
        PhysxLightListFree(Data);
    }

private:
    PPHYSX_LIGHT_LIST Data;

    LightList(
        void
        )
    { 
        ISTATUS Status = PhysxLightListAllocate(&Data);

        if (Status != ISTATUS_SUCCESS)
        {
            ASSERT(Status == ISTATUS_ALLOCATION_FAILED);
            throw std::bad_alloc();
        }
    }
};

} // namespace Iris

#endif // _LIGHT_LIST_IRIS_PHYSX_PLUS_PLUS_