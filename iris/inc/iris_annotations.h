/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iris_annotations.h

Abstract:

    This file contains annotations applied to function definitions.

--*/

#ifndef _ANNOTATIONS_IRIS_
#define _ANNOTATIONS_IRIS_

#ifdef _MSC_VER
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE inline
#endif
    
#define STATIC static
#define SFORCEINLINE STATIC FORCEINLINE

#endif // _ANNOTATIONS_IRIS_