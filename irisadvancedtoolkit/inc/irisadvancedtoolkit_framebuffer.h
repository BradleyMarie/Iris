/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvancedtoolkit_framebuffer.h

Abstract:

    This file contains the prototypes for the FRAMEBUFFER extension methods.

--*/

#ifndef _FRAMEBUFFER_IRIS_ADVANCED_TOOLKIT_
#define _FRAMEBUFFER_IRIS_ADVANCED_TOOLKIT_

#include <irisadvancedtoolkit.h>

//
// Prototypes
//

_Success_(return == ISTATUS_SUCCESS)
IRISADVANCEDTOOLKITAPI
ISTATUS
FramebufferSaveAsPFM(
    _In_ PCFRAMEBUFFER Framebuffer,
    _In_ PCSTR Path
    );

#endif // _FRAMEBUFFER_IRIS_ADVANCED_TOOLKIT_
