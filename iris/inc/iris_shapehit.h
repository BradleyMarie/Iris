/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_shapehit.h

Abstract:

    This module defines the SHAPE_HIT struct.

--*/

#ifndef _IRIS_SHAPE_HIT_
#define _IRIS_SHAPE_HIT_

_Struct_size_bytes_(SizeInBytes)
typedef struct _SHAPE_HIT {
    SIZE_T SizeInBytes;
    FLOAT Distance;
    INT32 FaceHit;
} SHAPE_HIT, *PSHAPE_HIT;

#endif // _IRIS_SHAPE_HIT_