/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    iristoolkit_types.h

Abstract:

    This file contains the definitions for the types used by IrisToolkit.

--*/

#ifndef _TYPES_IRISTOOLKIT_
#define _TYPES_IRISTOOLKIT_

typedef
VOID
(*PFREE_ROUTINE)(
    _Pre_maybenull_ _Post_invalid_ PVOID Pointer
    );

#endif // _TYPES_IRISTOOLKIT_