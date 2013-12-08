/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shapehit.h

Abstract:

    This module defines the SHAPE_HIT struct.

--*/

#ifndef _IRIS_SHAPE_HIT_
#define _IRIS_SHAPE_HIT_

typedef struct _SHAPE_HIT {
    FLOAT Distance;
    INT32 FaceHit;
    _Field_size_bytes_opt_(AdditionalDataSizeInBytes) PVOID AdditionalData;
    SIZE_T AdditionalDataSizeInBytes;
} SHAPE_HIT, *PSHAPE_HIT;

#endif // _IRIS_SHAPE_HIT_