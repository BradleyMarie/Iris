/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisplusplus_shape.h

Abstract:

    This file contains the definitions for the 
    Iris++ ShapeBase type.

--*/

#include <irisplusplus.h>

#ifndef _SHAPE_BASE_IRIS_PLUS_PLUS_
#define _SHAPE_BASE_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class ShapeBase {
protected:
    class HitAllocator final {
    public:
        _Ret_
        PHIT_LIST
        Allocate(
            _In_opt_ PHIT_LIST NextHit,
            _In_ FLOAT Distance,
            _In_ INT32 FaceHit,
            _In_ const Point & HitPoint
            )
        {
            POINT3 IrisHitPoint = HitPoint.AsPOINT3();

            PHIT_LIST Output = Allocate(NextHit,
                                        Distance,
                                        FaceHit,
                                        IrisHitPoint,
                                        NULL,
                                        0,
                                        0);

            return Output;
        }

        template<typename T>
        _Ret_
        PHIT_LIST
        Allocate(
            _In_opt_ PHIT_LIST NextHit,
            _In_ FLOAT Distance,
            _In_ INT32 FaceHit,
            _In_ const Point & HitPoint,
            _In_ const T & AdditionalData
            )
        {
            POINT3 IrisHitPoint = HitPoint.AsPOINT3();

            PHIT_LIST Output = Allocate(NextHit,
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
            _In_ FLOAT Distance,
            _In_ INT32 FaceHit
            )
        {
            PHIT_LIST Output = Allocate(NextHit,
                                        Distance,
                                        FaceHit,
                                        NULL,
                                        0,
                                        0);

            return Output;
        }

        template<typename T>
        _Ret_
        PHIT_LIST
        Allocate(
            _In_opt_ PHIT_LIST NextHit,
            _In_ FLOAT Distance,
            _In_ INT32 FaceHit,
            _In_ const T & AdditionalData
            )
        {
            PHIT_LIST Output = Allocate(NextHit,
                                        Distance,
                                        FaceHit,
                                        static_cast<PCVOID>(AdditionalData),
                                        sizeof(T),
                                        sizeof(PVOID));

            return Output;
        }

        HitAllocator(
            _In_ const HitAllocator & HitAllocatorRef
            ) = delete;

        HitAllocator &
        operator=(
            _In_ const HitAllocator & HitAllocatorRef
            ) = delete;

    private:
        PHIT_ALLOCATOR Data;

        HitAllocator(
            _In_ PHIT_ALLOCATOR HitAllocator
            )
            : Data(HitAllocator)
        { }

        _Ret_
        IRISPLUSPLUSAPI
        PHIT_LIST
        Allocate(
            _In_opt_ PHIT_LIST NextHit,
            _In_ FLOAT Distance,
            _In_ INT32 FaceHit,
            _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
            _In_ SIZE_T AdditionalDataSizeInBytes,
            _In_ SIZE_T AdditionalDataAlignment
            );

        _Ret_
        IRISPLUSPLUSAPI
        PHIT_LIST
        Allocate(
            _In_opt_ PHIT_LIST NextHit,
            _In_ FLOAT Distance,
            _In_ INT32 FaceHit,
            _In_ const POINT3 & HitPoint,
            _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
            _In_ SIZE_T AdditionalDataSizeInBytes,
            _In_ SIZE_T AdditionalDataAlignment
            );

        friend class ShapeBase;
    };

    _Ret_
    virtual
    PHIT_LIST
    Test(
        _In_ Ray ModelRay,
        _Inout_ HitAllocator & Allocator
        ) const = 0;

    IRISPLUSPLUSAPI
    static
    Shape
    Create(
        _In_ std::unique_ptr<ShapeBase> ShapeBasePtr
        );

public:    
    virtual
    ~ShapeBase(
        void
        )
    { }
    
private:
    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    static
    ISTATUS 
    Test(
        _In_opt_ PCVOID Context, 
        _In_ RAY Ray,
        _Inout_ PHIT_ALLOCATOR IrisHitAllocator,
        _Outptr_result_maybenull_ PHIT_LIST *HitList
        );
    
    static
    VOID 
    Free(
        _In_ _Post_invalid_ PVOID Context
        );
    
    const static SHAPE_VTABLE InteropVTable;
};

} // namespace Iris

#endif // _SHAPE_BASE_IRIS_PLUS_PLUS_