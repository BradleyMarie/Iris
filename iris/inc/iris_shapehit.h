/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shapehit.h

Abstract:

    This module defines the SHAPE_HIT struct.

--*/

#ifndef _IRIS_SHAPE_HIT_
#define _IRIS_SHAPE_HIT_

_Struct_size_bytes_(sizeof(SHAPE_HIT) + AdditionalSizeInBytes)
typedef struct _SHAPE_HIT {
    FLOAT Distance;
    INT32 FaceHit;
    SIZE_T AdditionalSizeInBytes;
} SHAPE_HIT, *PSHAPE_HIT;

#endif // _IRIS_SHAPE_HIT_