/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisphysxplusplus_hitallocator.h

Abstract:

    This file contains the declarations for the 
    IrisPysx++ HitAllocator type.

--*/

#include <irisphysxplusplus.h>

#ifndef _HIT_ALLOCATOR_IRIS_PHYSX_PLUS_PLUS_
#define _HIT_ALLOCATOR_IRIS_PHYSX_PLUS_PLUS_

namespace IrisPhysx {

//
// Types
//

class HitAllocator final {
public:
    HitAllocator(
        _In_ PPBR_HIT_ALLOCATOR HitAllocatorPtr
        )
    : Data(HitAllocatorPtr)
    { 
        if (HitAllocatorPtr == nullptr)
        {
            throw std::invalid_argument("HitAllocatorPtr");
        }
    }
    
    _Ret_
    PPBR_HIT_ALLOCATOR
    AsPPBR_HIT_ALLOCATOR(
        void
        )
    {
        return Data;
    }

    _Ret_
    template<typename T>
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ FLOAT Distance,
        _In_ UINT32 FrontFace,
        _In_ UINT32 BackFace,
        _In_ const T & AdditionalData
        )
    {
        static_assert(std::is_trivially_copyable<T>::value,
                      "AdditionalData must be trivially copyable");
        
        static_assert(sizeof(T) != 0, "sizeof(T) == 0");
        static_assert(alignof(T) != 0, "alignof(T) == 0");
        static_assert((alignof(T) & (alignof(T) - 1)) == 0, "(alignof(T) & (alignof(T) - 1)) != 0");
        static_assert(sizeof(T) % alignof(T) == 0, "sizeof(T) % alignof(T) != 0");

        PHIT_LIST Result = Allocate(NextHit,
                                    Distance,
                                    FrontFace,
                                    BackFace,
                                    &AdditionalData,
                                    sizeof(T),
                                    alignof(T));
    
        return Result;
    }
    
    _Ret_
    template<typename T>
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ FLOAT Distance,
        _In_ UINT32 FrontFace,
        _In_ UINT32 BackFace,
        _In_ const T & AdditionalData,
        _In_ const Iris::Point & HitPoint
        )
    {
        static_assert(std::is_trivially_copyable<T>::value,
                      "AdditionalData must be trivially copyable");
        
        static_assert(sizeof(T) != 0, "sizeof(T) == 0");
        static_assert(alignof(T) != 0, "alignof(T) == 0");
        static_assert((alignof(T) & (alignof(T) - 1)) == 0, "(alignof(T) & (alignof(T) - 1)) != 0");
        static_assert(sizeof(T) % alignof(T) == 0, "sizeof(T) % alignof(T) != 0");

        PHIT_LIST Result = Allocate(NextHit,
                                    Distance,
                                    FrontFace,
                                    BackFace,
                                    &AdditionalData,
                                    sizeof(T),
                                    alignof(T),
                                    HitPoint);
    
        return Result;        
    }
    
    _Ret_
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ FLOAT Distance,
        _In_ UINT32 FrontFace,
        _In_ UINT32 BackFace
        )
    {
        PHIT_LIST Result = Allocate(NextHit,
                                    Distance,
                                    FrontFace,
                                    BackFace,
                                    nullptr,
                                    0,
                                    0);
    
        return Result;
    }
    
    _Ret_
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ FLOAT Distance,
        _In_ UINT32 FrontFace,
        _In_ UINT32 BackFace,
        _In_ const Iris::Point & HitPoint
        )
    {
        PHIT_LIST Result = Allocate(NextHit,
                                    Distance,
                                    FrontFace,
                                    BackFace,
                                    nullptr,
                                    0,
                                    0,
                                    HitPoint);
    
        return Result;        
    }
    
private:
    PPBR_HIT_ALLOCATOR Data;

    _Ret_
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ FLOAT Distance,
        _In_ UINT32 FrontFace,
        _In_ UINT32 BackFace,
        _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
        _In_ SIZE_T AdditionalDataSizeInBytes,
        _In_ SIZE_T AdditionalDataAlignment
        )
    {
        PHIT_LIST Result;
        
        assert(AdditionalDataSizeInBytes == 0 ||
               (AdditionalData != nullptr &&
                AdditionalDataSizeInBytes != 0 &&
                AdditionalDataAlignment != 0 &&
                (AdditionalDataAlignment & (AdditionalDataAlignment - 1)) == 0 &&
                AdditionalDataSizeInBytes % AdditionalDataAlignment == 0));

        ISTATUS Status = PBRHitAllocatorAllocate(Data,
                                                 NextHit,
                                                 Distance,
                                                 FrontFace,
                                                 BackFace,
                                                 AdditionalData,
                                                 AdditionalDataSizeInBytes,
                                                 AdditionalDataAlignment,
                                                 &Result);

        if (Status == ISTATUS_SUCCESS)
        {
            return Result;
        }

        switch (Status)
        {
            case ISTATUS_INVALID_ARGUMENT_02:
                throw std::invalid_argument("Distance");
                break;
            default:
                assert(Status == ISTATUS_ALLOCATION_FAILED);
                throw std::bad_alloc();
        }
    }
    
    _Ret_
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ FLOAT Distance,
        _In_ UINT32 FrontFace,
        _In_ UINT32 BackFace,
        _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
        _In_ SIZE_T AdditionalDataSizeInBytes,
        _In_ SIZE_T AdditionalDataAlignment,
        _In_ const Iris::Point & HitPoint
        )
    {
        PHIT_LIST Result;
        
        assert(AdditionalDataSizeInBytes == 0 ||
               (AdditionalData != nullptr &&
                AdditionalDataSizeInBytes != 0 &&
                AdditionalDataAlignment != 0 &&
                (AdditionalDataAlignment & (AdditionalDataAlignment - 1)) == 0 &&
                AdditionalDataSizeInBytes % AdditionalDataAlignment == 0));

        ISTATUS Status = PBRHitAllocatorAllocateWithHitPoint(Data,
                                                             NextHit,
                                                             Distance,
                                                             FrontFace,
                                                             BackFace,
                                                             AdditionalData,
                                                             AdditionalDataSizeInBytes,
                                                             AdditionalDataAlignment,
                                                             HitPoint.AsPOINT3(),
                                                             &Result);

        if (Status == ISTATUS_SUCCESS)
        {
            return Result;
        }

        switch (Status)
        {
            case ISTATUS_INVALID_ARGUMENT_02:
                throw std::invalid_argument("Distance");
                break;
            case ISTATUS_INVALID_ARGUMENT_08:
                throw std::invalid_argument("HitPoint");
                break;
            default:
                assert(Status == ISTATUS_ALLOCATION_FAILED);
                throw std::bad_alloc();
        }
    }
};

} // namespace Iris

#endif // _HIT_ALLOCATOR_IRIS_PHYSX_PLUS_PLUS_
