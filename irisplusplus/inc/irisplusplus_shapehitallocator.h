/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_shapehitallocator.h

Abstract:

    This file contains the definitions for the 
    Iris++ shapehitallocator type.

--*/

#include <irisplusplus.h>

#ifndef _SHAPEHITALLOCATOR_IRIS_PLUS_PLUS_
#define _SHAPEHITALLOCATOR_IRIS_PLUS_PLUS_

namespace Iris {

//
// Forward Declarations
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
static
ISTATUS 
ShapeTrace(
    _In_opt_ PCVOID Context, 
    _In_ RAY Ray,
    _Inout_ PSHAPE_HIT_ALLOCATOR IrisHitAllocator,
    _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
    );

//
// Types
//

class ShapeHitAllocator {
public:
    _Ret_
    PSHAPE_HIT_LIST
    Allocate(
        _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit,
        _In_ const Point & HitPoint
        )
    {
        POINT3 IrisHitPoint = HitPoint.AsPOINT3();

        PSHAPE_HIT_LIST Output = Allocate(NextShapeHit,
                                          Distance,
                                          FaceHit,
                                          IrisHitPoint,
                                          NULL,
                                          0);

        return Output;
    }

    template<typename T>
    _Ret_
    PSHAPE_HIT_LIST
    Allocate(
        _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit,
        _In_ const Point & HitPoint,
        _In_ const T & AdditionalData
        )
    {
        POINT3 IrisHitPoint = HitPoint.AsPOINT3();

        PSHAPE_HIT_LIST Output = Allocate(NextShapeHit,
                                          Distance,
                                          FaceHit,
                                          IrisHitPoint,
                                          static_cast<PCVOID>(AdditionalData),
                                          sizeof(T));

        return Output;
    }

    _Ret_
    PSHAPE_HIT_LIST
    Allocate(
        _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit
        )
    {
        PSHAPE_HIT_LIST Output = Allocate(NextShapeHit,
                                          Distance,
                                          FaceHit,
                                          NULL,
                                          0);

        return Output;
    }

    template<typename T>
    _Ret_
    PSHAPE_HIT_LIST
    Allocate(
        _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit,
        _In_ const T & AdditionalData
        )
    {
        PSHAPE_HIT_LIST Output = Allocate(NextShapeHit,
                                          Distance,
                                          FaceHit,
                                          static_cast<PCVOID>(AdditionalData),
                                          sizeof(T));

        return Output;
    }

private:
    PSHAPE_HIT_ALLOCATOR Data;

    ShapeHitAllocator(
        _In_ PSHAPE_HIT_ALLOCATOR HitAllocator
        )
    : Data(HitAllocator)
    { }

    _Ret_
    IRISPLUSPLUSAPI
    PSHAPE_HIT_LIST
    Allocate(
        _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit,    
        _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
        _In_ SIZE_T AdditionalDataSizeInBytes
        );

    _Ret_
    IRISPLUSPLUSAPI
    PSHAPE_HIT_LIST
    Allocate(
        _In_opt_ PSHAPE_HIT_LIST NextShapeHit,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit,
        _In_ POINT3 & HitPoint,
        _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
        _In_ SIZE_T AdditionalDataSizeInBytes
        );
    
    friend
    _Check_return_
    _Success_(return == ISTATUS_SUCCESS)
    ISTATUS 
    ShapeTrace(
        _In_opt_ PCVOID Context, 
        _In_ RAY Ray,
        _Inout_ PSHAPE_HIT_ALLOCATOR IrisHitAllocator,
        _Outptr_result_maybenull_ PSHAPE_HIT_LIST *ShapeHitList
        );
};

} // namespace Iris

#endif // _SHAPEHITALLOCATOR_IRIS_PLUS_PLUS_