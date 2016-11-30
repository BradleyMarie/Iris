/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvancedtoolkitplusplus_framebuffer.h

Abstract:

    This file contains the definitions for the 
    IrisAdvancedToolkit++ Framebuffer type.

--*/

#include <irisadvancedtoolkitplusplus.h>

#ifndef _FRAMEBUFFER_IRIS_ADVANCED_TOOLKIT_PLUS_PLUS_HEADER_
#define _FRAMEBUFFER_IRIS_ADVANCED_TOOLKIT_PLUS_PLUS_HEADER_

namespace IrisAdvancedToolkit {
namespace FramebufferIO {

//
// Types
//

static
inline
void
SaveAsPFM(
    _In_ const IrisAdvanced::Framebuffer & Fb,
    _In_ const std::string & Path
    )
{
    ISTATUS Status = FramebufferSaveAsPFM(Fb.AsPCFRAMEBUFFER(),
                                          Path.c_str());

    if (Status != ISTATUS_SUCCESS)
    {
        assert(Status == ISTATUS_IO_ERROR);
        throw std::runtime_error("ISTATUS_IO_ERROR");
    }
}

} // namespace FramebufferIO
} // namespace IrisAdvancedToolkit

#endif // _FRAMEBUFFER_IRIS_ADVANCED_TOOLKIT_PLUS_PLUS_HEADER_
