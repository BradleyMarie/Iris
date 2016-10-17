/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisplusplus_shape.h

Abstract:

    This file contains the definitions for the 
    Iris++ ShapeBase type.

--*/

#include <irisplusplus.h>

#ifndef _HIT_ALLOCATOR_IRIS_PLUS_PLUS_
#define _HIT_ALLOCATOR_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class HitAllocator final {
public:
    HitAllocator(
        _In_ PHIT_ALLOCATOR HitAllocator
        )
    : Data(HitAllocator)
    { }

    _Ret_
    PHIT_ALLOCATOR
    AsPHIT_ALLOCATOR(
        void
        )
    {
        return Data;
    }
    
    _Ret_
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ PCVOID DataPtr,
        _In_ FLOAT Distance,
        _In_ UINT32 FrontFace,
        _In_ UINT32 BackFace,
        _In_ const Point & HitPoint
        )
    {
        POINT3 IrisHitPoint = HitPoint.AsPOINT3();

        PHIT_LIST Output = Allocate(NextHit,
                                    DataPtr,
                                    Distance,
                                    FrontFace,
                                    BackFace,
                                    IrisHitPoint,
                                    nullptr,
                                    0,
                                    0);

        return Output;
    }

    template<typename T>
    _Ret_
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ PCVOID DataPtr,
        _In_ FLOAT Distance,
        _In_ UINT32 FrontFace,
        _In_ UINT32 BackFace,
        _In_ const Point & HitPoint,
        _In_ const T & AdditionalData
        )
    {
        static_assert(std::is_trivially_copyable<T>::value,
                      "AdditionalData must be trivially copyable");
        
        static_assert(sizeof(T) != 0, "sizeof(T) == 0");
        static_assert(alignof(T) != 0, "alignof(T) == 0");
        static_assert((alignof(T) & (alignof(T) - 1)) == 0, "(alignof(T) & (alignof(T) - 1)) != 0");
        static_assert(sizeof(T) % alignof(T) == 0, "sizeof(T) % alignof(T) != 0");

        POINT3 IrisHitPoint = HitPoint.AsPOINT3();

        PHIT_LIST Output = Allocate(NextHit,
                                    DataPtr,
                                    Distance,
                                    FrontFace,
                                    BackFace,
                                    IrisHitPoint,
                                    static_cast<PCVOID>(AdditionalData),
                                    sizeof(T),
                                    alignof(T));

        return Output;
    }

    _Ret_
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ PCVOID DataPtr,
        _In_ FLOAT Distance,
        _In_ UINT32 FrontFace,
        _In_ UINT32 BackFace
        )
    {
        PHIT_LIST Output = Allocate(NextHit,
                                    DataPtr,
                                    Distance,
                                    FrontFace,
                                    BackFace,
                                    nullptr,
                                    0,
                                    0);

        return Output;
    }

    template<typename T>
    _Ret_
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ PCVOID DataPtr,
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

        PHIT_LIST Output = Allocate(NextHit,
                                    DataPtr,
                                    Distance,
                                    FrontFace,
                                    BackFace,
                                    static_cast<PCVOID>(AdditionalData),
                                    sizeof(T),
                                    alignof(T));

        return Output;
    }

private:
    PHIT_ALLOCATOR Data;

    _Ret_
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ PCVOID DataPtr,
        _In_ FLOAT Distance,
        _In_ UINT32 FrontFace,
        _In_ UINT32 BackFace,
        _When_(AdditionalDataSizeInBytes != 0, _In_reads_bytes_opt_(AdditionalDataSizeInBytes)) PCVOID AdditionalData,
        _In_ SIZE_T AdditionalDataSizeInBytes,
        _When_(AdditionalDataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && AdditionalDataSizeInBytes % _Curr_ == 0)) SIZE_T AdditionalDataAlignment
        )
    {
        PHIT_LIST Result;
        ISTATUS Status;

        assert(AdditionalDataSizeInBytes == 0 ||
               (AdditionalData != nullptr &&
                AdditionalDataSizeInBytes != 0 &&
                AdditionalDataAlignment != 0 &&
                (AdditionalDataAlignment & (AdditionalDataAlignment - 1)) == 0 &&
                AdditionalDataSizeInBytes % AdditionalDataAlignment == 0));

        Status = HitAllocatorAllocate(Data,
                                      NextHit,
                                      DataPtr,
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
            case ISTATUS_INVALID_ARGUMENT_03:
                throw std::invalid_argument("Distance");
            default:
                assert(Status == ISTATUS_ALLOCATION_FAILED);
                throw std::bad_alloc();
        }
    }

    _Ret_
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ PCVOID DataPtr,
        _In_ FLOAT Distance,
        _In_ UINT32 FrontFace,
        _In_ UINT32 BackFace,
        _In_ const POINT3 & HitPoint,
        _When_(AdditionalDataSizeInBytes != 0, _In_reads_bytes_opt_(AdditionalDataSizeInBytes)) PCVOID AdditionalData,
        _In_ SIZE_T AdditionalDataSizeInBytes,
        _When_(AdditionalDataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && AdditionalDataSizeInBytes % _Curr_ == 0)) SIZE_T AdditionalDataAlignment
        )
    {
        PHIT_LIST Result;
        ISTATUS Status;

        assert(AdditionalDataSizeInBytes == 0 ||
               (AdditionalData != nullptr &&
                AdditionalDataSizeInBytes != 0 &&
                AdditionalDataAlignment != 0 &&
                (AdditionalDataAlignment & (AdditionalDataAlignment - 1)) == 0 &&
                AdditionalDataSizeInBytes % AdditionalDataAlignment == 0));

        Status = HitAllocatorAllocateWithHitPoint(Data,
                                                  NextHit,
                                                  DataPtr,
                                                  Distance,
                                                  FrontFace,
                                                  BackFace,
                                                  AdditionalData,
                                                  AdditionalDataSizeInBytes,
                                                  AdditionalDataAlignment,
                                                  HitPoint,
                                                  &Result);

        if (Status == ISTATUS_SUCCESS)
        {
            return Result;
        }

        switch (Status)
        {
            case ISTATUS_INVALID_ARGUMENT_03:
                throw std::invalid_argument("Distance");
            case ISTATUS_INVALID_ARGUMENT_09:
                throw std::invalid_argument("HitPoint");
            default:
                assert(Status == ISTATUS_ALLOCATION_FAILED);
                throw std::bad_alloc();
        }
    }
};

} // namespace Iris

#endif // _HIT_ALLOCATOR_IRIS_PLUS_PLUS_
