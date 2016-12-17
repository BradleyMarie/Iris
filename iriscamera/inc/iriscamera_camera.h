/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscamera_camera.h

Abstract:

    This file contains the definitions for the CAMERA type.

--*/

#ifndef _CAMERA_IRIS_CAMERA_
#define _CAMERA_IRIS_CAMERA_

#include <iriscamera.h>

//
// Types
//

typedef
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
(*PCAMERA_COMPUTE_RAY_ROUTINE)(
    _In_ PCVOID Context,
    _In_range_(0, NumberOfRows - 1) SIZE_T Row,
    _In_ SIZE_T NumberOfRows,
    _In_range_(0, NumberOfColumns - 1) SIZE_T Column,
    _In_ SIZE_T NumberOfColumns,
    _In_ FLOAT PixelU,
    _In_ FLOAT PixelV,
    _In_ FLOAT LensU,
    _In_ FLOAT LensV,
    _Out_ PRAY WorldRay
    );

typedef
VOID
(*PCAMERA_GET_PARAMETERS_ROUTINE)(
    _In_ PCVOID Context,
    _Out_ PBOOL SamplePixel,
    _Out_ PBOOL SampleLens,
    _Out_ PFLOAT MinPixelU,
    _Out_ PFLOAT MaxPixelU,
    _Out_ PFLOAT MinPixelV,
    _Out_ PFLOAT MaxPixelV,
    _Out_ PFLOAT MinLensU,
    _Out_ PFLOAT MaxLensU,
    _Out_ PFLOAT MinLensV,
    _Out_ PFLOAT MaxLensV
    );

typedef struct _CAMERA_VTABLE {
    PCAMERA_COMPUTE_RAY_ROUTINE ComputeRayRoutine;
    PCAMERA_GET_PARAMETERS_ROUTINE GetParamtersRoutine;
    PFREE_ROUTINE FreeRoutine;
} CAMERA_VTABLE, *PCAMERA_VTABLE;

typedef CONST CAMERA_VTABLE *PCCAMERA_VTABLE;

typedef struct _CAMERA CAMERA, *PCAMERA;
typedef CONST CAMERA *PCCAMERA;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISCAMERAAPI
ISTATUS
CameraAllocate(
    _In_ PCCAMERA_VTABLE CameraVTable,
    _When_(DataSizeInBytes != 0, _In_reads_bytes_opt_(DataSizeInBytes)) PCVOID Data,
    _In_ SIZE_T DataSizeInBytes,
    _When_(DataSizeInBytes != 0, _Pre_satisfies_(_Curr_ != 0 && (_Curr_ & (_Curr_ - 1)) == 0 && DataSizeInBytes % _Curr_ == 0)) SIZE_T DataAlignment,
    _Out_ PCAMERA *Camera
    );

IRISCAMERAAPI
VOID
CameraFree(
    _In_opt_ _Post_invalid_ PCAMERA Camera
    );

#endif // _CAMERA_IRIS_CAMERA_