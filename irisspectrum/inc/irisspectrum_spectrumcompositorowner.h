/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisspectrum_spectrumcompositorowner.h

Abstract:

    This file contains the definitions for the SPECTRUM_COMPOSITOR_OWNER type.

--*/

#ifndef _SPECTRUM_COMPOSITOR_OWNER_IRIS_SPECTRUM_
#define _SPECTRUM_COMPOSITOR_OWNER_IRIS_SPECTRUM_

#include <irisspectrum.h>

//
// Types
//

typedef struct _SPECTRUM_COMPOSITOR_OWNER SPECTRUM_COMPOSITOR_OWNER, *PSPECTRUM_COMPOSITOR_OWNER;
typedef CONST SPECTRUM_COMPOSITOR_OWNER *PCSPECTRUM_COMPOSITOR_OWNER;

//
// Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
IRISSPECTRUMAPI
ISTATUS
SpectrumCompositorOwnerAllocate(
    _Out_ PSPECTRUM_COMPOSITOR_OWNER *CompositorOwner
    );

_Ret_
IRISSPECTRUMAPI
PSPECTRUM_COMPOSITOR
SpectrumCompositorOwnerGetCompositor(
    _In_ PSPECTRUM_COMPOSITOR_OWNER CompositorOwner
    );

IRISSPECTRUMAPI
VOID
SpectrumCompositorOwnerClear(
    _Inout_opt_ PSPECTRUM_COMPOSITOR_OWNER CompositorOwner
    );

IRISSPECTRUMAPI
VOID
SpectrumCompositorOwnerFree(
    _In_opt_ _Post_invalid_ PSPECTRUM_COMPOSITOR_OWNER CompositorOwner
    );
    
#endif // _SPECTRUM_COMPOSITOR_OWNER_IRIS_SPECTRUM_