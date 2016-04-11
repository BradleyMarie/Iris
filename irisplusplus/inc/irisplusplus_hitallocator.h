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
        _In_ INT32 FaceHit,
        _In_ const Point & HitPoint
        )
    {
        POINT3 IrisHitPoint = HitPoint.AsPOINT3();

        PHIT_LIST Output = Allocate(NextHit,
                                    DataPtr,
                                    Distance,
                                    FaceHit,
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
        _In_ INT32 FaceHit,
        _In_ const Point & HitPoint,
        _In_ const T & AdditionalData
        )
    {
        static_assert(std::is_trivially_copyable<T>::value,
                      "AdditionalData must be trivially copyable");
                      
        POINT3 IrisHitPoint = HitPoint.AsPOINT3();

        PHIT_LIST Output = Allocate(NextHit,
                                    DataPtr,
                                    Distance,
                                    FaceHit,
                                    IrisHitPoint,
                                    static_cast<PCVOID>(AdditionalData),
                                    sizeof(T),
                                    sizeof(PVOID));

        return Output;
    }

    _Ret_
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ PCVOID DataPtr,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit
        )
    {
        PHIT_LIST Output = Allocate(NextHit,
                                    DataPtr,
                                    Distance,
                                    FaceHit,
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
        _In_ INT32 FaceHit,
        _In_ const T & AdditionalData
        )
    {
        static_assert(std::is_trivially_copyable<T>::value,
                      "AdditionalData must be trivially copyable");
                      
        PHIT_LIST Output = Allocate(NextHit,
                                    DataPtr,
                                    Distance,
                                    FaceHit,
                                    static_cast<PCVOID>(AdditionalData),
                                    sizeof(T),
                                    sizeof(PVOID));

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
        _In_ INT32 FaceHit,
        _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
        _In_ SIZE_T AdditionalDataSizeInBytes,
        _In_ SIZE_T AdditionalDataAlignment
        )
    {
        PHIT_LIST Result;
        ISTATUS Status;

        Status = HitAllocatorAllocate(Data,
                                      NextHit,
                                      DataPtr,
                                      Distance,
                                      FaceHit,
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
            case ISTATUS_ALLOCATION_FAILED:
                throw std::bad_alloc();
            default:
                throw std::runtime_error(ISTATUSToCString(Status));
        }
    }

    _Ret_
    PHIT_LIST
    Allocate(
        _In_opt_ PHIT_LIST NextHit,
        _In_ PCVOID DataPtr,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit,
        _In_ const POINT3 & HitPoint,
        _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
        _In_ SIZE_T AdditionalDataSizeInBytes,
        _In_ SIZE_T AdditionalDataAlignment
        )
    {
        PHIT_LIST Result;
        ISTATUS Status;

        Status = HitAllocatorAllocateWithHitPoint(Data,
                                                  NextHit,
                                                  DataPtr,
                                                  Distance,
                                                  FaceHit,
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
            case ISTATUS_ALLOCATION_FAILED:
                throw std::bad_alloc();
            default:
                throw std::runtime_error(ISTATUSToCString(Status));
        }
    }
};

} // namespace Iris

#endif // _HIT_ALLOCATOR_IRIS_PLUS_PLUS_