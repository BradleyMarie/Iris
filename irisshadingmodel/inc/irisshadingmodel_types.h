/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    irisshadingmodel_types.h

Abstract:

    This file contains the definitions for the types used by Iris Shading Model.

--*/

#ifndef _TYPES_IRIS_SHADING_MODEL_
#define _TYPES_IRIS_SHADING_MODEL_

typedef
VOID
(*PFREE_ROUTINE)(
    _Pre_maybenull_ _Post_invalid_ PVOID Pointer
    );

#endif // _TYPES_IRIS_SHADING_MODEL_