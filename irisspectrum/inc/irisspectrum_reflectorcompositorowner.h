/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisspectrum_reflectorcompositorowner.h

Abstract:

    This file contains the definitions for the REFLECTOR_COMPOSITOR_OWNER type.

--*/

#ifndef _REFLECTOR_COMPOSITOR_OWNER_IRIS_SPECTRUM_
#define _REFLECTOR_COMPOSITOR_OWNER_IRIS_SPECTRUM_

#include <irisspectrum.h>

//
// Types
//

typedef struct _REFLECTOR_COMPOSITOR_OWNER REFLECTOR_COMPOSITOR_OWNER, *PREFLECTOR_COMPOSITOR_OWNER;
typedef CONST REFLECTOR_COMPOSITOR_OWNER *PCREFLECTOR_COMPOSITOR_OWNER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
ReflectorCompositorOwnerAllocate(
    _Out_ PREFLECTOR_COMPOSITOR_OWNER *CompositorOwner
    );

_Ret_
IRISSPECTRUMAPI
PREFLECTOR_COMPOSITOR
ReflectorCompositorOwnerGetCompositor(
    _In_ PREFLECTOR_COMPOSITOR_OWNER CompositorOwner
    );

IRISSPECTRUMAPI
VOID
ReflectorCompositorOwnerClear(
    _Inout_ PREFLECTOR_COMPOSITOR_OWNER CompositorOwner
    );

IRISSPECTRUMAPI
VOID
ReflectorCompositorOwnerFree(
    _In_opt_ _Post_invalid_ PREFLECTOR_COMPOSITOR_OWNER CompositorOwner
    );

#endif // _REFLECTOR_COMPOSITOR_OWNER_IRIS_SPECTRUM_