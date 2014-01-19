/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_geometryhit.h

Abstract:

    This module defines the GEOMETRY_HIT struct.

--*/

#ifndef _IRIS_GEOMETRY_HIT_
#define _IRIS_GEOMETRY_HIT_

//
// Types
//

typedef struct _SHARED_GEOMETRY_HIT SHARED_GEOMETRY_HIT, *PSHARED_GEOMETRY_HIT;
typedef CONST SHARED_GEOMETRY_HIT *PCSHARED_GEOMETRY_HIT;

typedef struct _GEOMETRY_HIT {
    PCSHARED_GEOMETRY_HIT SharedGeometryHit;
    PCSHAPE Shape;
    FLOAT Distance;
    INT32 FaceHit;
    _Field_size_bytes_opt_(AdditionalDataSizeInBytes) PCVOID AdditionalData;
    SIZE_T AdditionalDataSizeInBytes;
} GEOMETRY_HIT, *PGEOMETRY_HIT;

typedef CONST GEOMETRY_HIT *PCGEOMETRY_HIT;

//
// Functions
//

IRISAPI
VOID
SharedGeometryHitComputeModelViewer(
    _In_ PCSHARED_GEOMETRY_HIT GeometryHit,
    _In_ PCVECTOR3 WorldViewer,
    _Out_ PVECTOR3 ModelViewer
    );

IRISAPI
VOID
SharedGeometryHitComputeModelHit(
    _In_ PCSHARED_GEOMETRY_HIT GeometryHit,
    _In_ PCPOINT3 WorldHitPoint,
    _Out_ PPOINT3 ModelHitPoint
    );

IRISAPI
PCMATRIX
SharedGeometryHitGetModelToWorld(
    _In_ PCSHARED_GEOMETRY_HIT GeometryHit
    );

#endif // _IRIS_GEOMETRY_HIT_