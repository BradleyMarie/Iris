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
// Typedefs
//

typedef SHAPE_HIT_LIST ShapeHitList;
typedef SHAPE_HIT ShapeHit;

//
// Types
//

class ShapeHitAllocator {
public:
    ShapeHitAllocator(
        _In_ PSHAPE_HIT_ALLOCATOR HitAllocator
        )
    : Data(HitAllocator)
    { }
    
    _Ret_
    ShapeHitList *
    Allocate(
        _In_opt_ ShapeHitList * NextShapeHit,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit,
        _In_ const Point & HitPoint
        )
    {
        POINT3 IrisHitPoint = HitPoint.AsPOINT3();

        ShapeHitList * Output = Allocate(NextShapeHit,
                                         Distance,
                                         FaceHit,
                                         IrisHitPoint,
                                         NULL,
                                         0);

        return Output;
    }

    template<typename T>
    _Ret_
    ShapeHitList *
    Allocate(
        _In_opt_ ShapeHitList * NextShapeHit,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit,
        _In_ const Point & HitPoint,
        _In_ const T & AdditionalData
        )
    {
        POINT3 IrisHitPoint = HitPoint.AsPOINT3();

        ShapeHitList * Output = Allocate(NextShapeHit,
                                         Distance,
                                         FaceHit,
                                         IrisHitPoint,
                                         static_cast<PCVOID>(AdditionalData),
                                         sizeof(T));

        return Output;
    }

    _Ret_
    ShapeHitList *
    Allocate(
        _In_opt_ ShapeHitList * NextShapeHit,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit
        )
    {
        ShapeHitList * Output = Allocate(NextShapeHit,
                                         Distance,
                                         FaceHit,
                                         NULL,
                                         0);

        return Output;
    }

    template<typename T>
    _Ret_
    ShapeHitList *
    Allocate(
        _In_opt_ ShapeHitList * NextShapeHit,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit,
        _In_ const T & AdditionalData
        )
    {
        ShapeHitList * Output = Allocate(NextShapeHit,
                                         Distance,
                                         FaceHit,
                                         static_cast<PCVOID>(AdditionalData),
                                         sizeof(T));

        return Output;
    }

private:
    PSHAPE_HIT_ALLOCATOR Data;

    _Ret_
    IRISPLUSPLUSAPI
    ShapeHitList *
    Allocate(
        _In_opt_ ShapeHitList * NextShapeHit,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit,    
        _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
        _In_ SIZE_T AdditionalDataSizeInBytes
        );

    _Ret_
    IRISPLUSPLUSAPI
    ShapeHitList *
    Allocate(
        _In_opt_ ShapeHitList * NextShapeHit,
        _In_ FLOAT Distance,
        _In_ INT32 FaceHit,
        _In_ POINT3 & HitPoint,
        _In_reads_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData,
        _In_ SIZE_T AdditionalDataSizeInBytes
        );
};

} // namespace Iris

#endif // _SHAPEHITALLOCATOR_IRIS_PLUS_PLUS_