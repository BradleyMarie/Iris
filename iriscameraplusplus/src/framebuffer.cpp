/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    framebuffer.cpp

Abstract:

    This file contains the definitions for the 
    IrisCamera++ Framebuffer type.

--*/

#include <iriscameraplusplus.h>

namespace IrisCamera {

_Ret_
PFRAMEBUFFER
Framebuffer::Copy(
    void
    ) const
{
    SIZE_T NumberOfColumns;
    SIZE_T NumberOfRows;
    FramebufferGetDimensions(Data,
                             &NumberOfRows,
                             &NumberOfColumns);

    PFRAMEBUFFER Destination;
    ISTATUS Status = FramebufferAllocate(Color3InitializeBlack(),
                                         NumberOfRows,
                                         NumberOfColumns,
                                         &Destination);

    if (Status != ISTATUS_SUCCESS)
    {
        assert(Status == ISTATUS_ALLOCATION_FAILED);
        throw std::bad_alloc();
    }

    for (SIZE_T Row = 0; Row < NumberOfRows; Row++)
    {
        for (SIZE_T Column = 0; Column < NumberOfColumns; Column++)
        {
            COLOR3 PixelColor;
            FramebufferGetPixel(Data,
                                Row,
                                Column,
                                &PixelColor);

            FramebufferSetPixel(Destination,
                                PixelColor,
                                Row,
                                Column);
        }
    }

    return Destination;
}

} // namespace IrisCamera