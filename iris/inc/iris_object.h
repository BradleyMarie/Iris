/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_object.h

Abstract:

    This module declares the iris object management routines.

--*/

#ifndef _IRIS_OBJECT_
#define _IRIS_OBJECT_

#include <iris.h>

#define IRIS_TYPE_SHAPE           0
#define IRIS_TYPE_SHAPE_VTABLE    1
#define IRIS_TYPE_GEOMETRY        2
#define IRIS_TYPE_GEOMETRY_VTABLE 3

typedef VOID (*PFREE_ROUTINE)(_Pre_maybenull_ _Post_invalid_ PVOID);

typedef struct _IRIS_OBJECT IRIS_OBJECT, *PIRIS_OBJECT;

UINT8
IrisType(
    _In_ PIRIS_OBJECT Object
    );

PVOID
IrisUserData(
    _In_ PIRIS_OBJECT Object
    );

VOID
IrisFree(
    _Pre_maybenull_ _Post_invalid_ PIRIS_OBJECT Object
    );

#endif // _IRIS_OBJECT_