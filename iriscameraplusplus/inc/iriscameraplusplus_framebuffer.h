/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    iriscameraplusplus_framebuffer.h

Abstract:

    This file contains the definitions for the 
    IrisCamera++ Framebuffer type.

--*/

#include <iriscameraplusplus.h>

#ifndef _FRAMEBUFFER_IRIS_CAMERA_PLUS_PLUS_HEADER_
#define _FRAMEBUFFER_IRIS_CAMERA_PLUS_PLUS_HEADER_

namespace IrisCamera {

//
// Types
//

class Framebuffer final {
public:
    Framebuffer(
        _In_ PFRAMEBUFFER FramebufferPtr,
        _In_ bool Retain
        )
    : Data(FramebufferPtr)
    { 
        if (FramebufferPtr == nullptr)
        {
            throw std::invalid_argument("FramebufferPtr");
        }
        
        if (Retain)
        {
            FramebufferRetain(Data);
        }
    }

    _Ret_
    PFRAMEBUFFER
    AsPFRAMEBUFFER(
        void
        )
    {
        return Data;
    }

    _Ret_
    PCFRAMEBUFFER
    AsPCFRAMEBUFFER(
        void
        ) const
    {
        return Data;
    }
        
    IrisAdvanced::Color3
    GetPixel(
        _In_ SIZE_T Row,
        _In_ SIZE_T Column
        ) const
    {
        COLOR3 Result;

        ISTATUS Status = FramebufferGetPixel(Data,
                                             Row,
                                             Column,
                                             &Result);

        if (Status == ISTATUS_SUCCESS)
        {
            return IrisAdvanced::Color3(Result);
        }
        
        switch (Status)
        {
            case ISTATUS_INVALID_ARGUMENT_01:
                throw std::out_of_range("Row");
            default:
                assert(Status == ISTATUS_INVALID_ARGUMENT_02);
                throw std::out_of_range("Column");
        }
    }

    std::tuple<SIZE_T, SIZE_T>
    GetDimensions(
        void
        ) const
    {
        SIZE_T Rows, Columns;

        FramebufferGetDimensions(Data,
                                 &Rows,
                                 &Columns);

        return std::make_tuple(Rows, Columns);
    }

    void
    SaveAsPFM(
        _In_ const std::string & Path
        ) const
    {
        ISTATUS Status = FramebufferSaveAsPFM(Data, Path.c_str());

        if (Status != ISTATUS_SUCCESS)
        {
            assert(Status == ISTATUS_IO_ERROR);
            throw std::runtime_error("ISTATUS_IO_ERROR");
        }
    }

    Framebuffer(
        _In_ const Framebuffer & ToCopy
        )
    : Data(ToCopy.Data)
    { 
        FramebufferRetain(Data);
    }
        
    Framebuffer &
    operator=(
        _In_ const Framebuffer & ToCopy
        )
    {
        if (this != &ToCopy)
        {
            FramebufferRelease(Data);
            Data = ToCopy.Data;
            FramebufferRetain(Data);
        }

        return *this;
    }

    ~Framebuffer(
        void
        )
    {
        FramebufferRelease(Data);
    }

private:
    PFRAMEBUFFER Data;
};

} // namespace IrisCamera

#endif // _FRAMEBUFFER_IRIS_CAMERA_PLUS_PLUS_HEADER_
