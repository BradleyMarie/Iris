/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    pfm.h

Abstract:

    This file contains the prototypes for the PFM functions used
    to write and read framebuffers objects.

--*/

#ifndef _IRIS_TEST_PFM_HEADER_
#define _IRIS_TEST_PFM_HEADER_

#include <iristest.h>

bool
WritePfm(
    PCFRAMEBUFFER FrameBuffer,
    std::string FileName
    );

#endif // _IRIS_TEST_PFM_HEADER_