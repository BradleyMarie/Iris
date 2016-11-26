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

//
// Types
//

class Framebuffer final {
public:
    Framebuffer(
        _In_ Framebuffer && ToMove
        )
    : Data(ToMove.Data)
    { 
        ToMove.Data = nullptr;
    }

    static
    Framebuffer
    Create(
        _In_ const IrisAdvanced::Color3 & InitialColor,
        _In_ SIZE_T Rows,
        _In_ SIZE_T Columns
        )
    {
        return Framebuffer(InitialColor, Rows, Columns);
    }

    _Ret_
    PFRAMEBUFFER
    AsPFRAMEBUFFER(
        void
        )
    {
        return Data;
    }

    void
    SetPixel(
        _In_ const IrisAdvanced::Color3 & Color,
        _In_ SIZE_T Row,
        _In_ SIZE_T Column
        )
    {
        ISTATUS Status = FramebufferSetPixel(Data,
                                             Color.AsCOLOR3(),
                                             Row,
                                             Column);

        switch (Status)
        {
            case ISTATUS_SUCCESS:
                return;
            case ISTATUS_INVALID_ARGUMENT_01:
                throw std::invalid_argument("Color");;
            case ISTATUS_INVALID_ARGUMENT_02:
                throw std::out_of_range("Row");
            case ISTATUS_INVALID_ARGUMENT_03:
                throw std::out_of_range("Column");
        }

        assert(false);
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
        const std::string & Path
        ) const
    {
        ISTATUS Status = FramebufferSaveAsPFM(Data,
                                              Path.c_str());

        if (Status != ISTATUS_SUCCESS)
        {
            assert(Status == ISTATUS_IO_ERROR);
            throw std::runtime_error("ISTATUS_IO_ERROR");
        }
    }

    Framebuffer(
        _In_ const Framebuffer & ToCopy
        ) = delete;
        
    Framebuffer &
    operator=(
        _In_ const Framebuffer & ToCopy
        ) = delete;

    ~Framebuffer(
        void
        )
    {
        FramebufferFree(Data);
    }

private:
    PFRAMEBUFFER Data;

    Framebuffer(
        _In_ const IrisAdvanced::Color3 & InitialColor,
        _In_ SIZE_T Rows,
        _In_ SIZE_T Columns
        )
    {
        ISTATUS Status = FramebufferAllocate(InitialColor.AsCOLOR3(),
                                             Rows,
                                             Columns,
                                             &Data);

        if (Status == ISTATUS_SUCCESS)
        {
            return;
        }
        
        switch (Status)
        {
            case ISTATUS_INVALID_ARGUMENT_00:
                throw std::invalid_argument("InitialColor");;
            case ISTATUS_INVALID_ARGUMENT_01:
                throw std::invalid_argument("Rows");
            case ISTATUS_INVALID_ARGUMENT_02:
                throw std::invalid_argument("Columns");
            default:
                assert(Status == ISTATUS_ALLOCATION_FAILED);
                throw std::bad_alloc();
        }
    }
};

} // namespace IrisAdvancedToolkit

#endif // _FRAMEBUFFER_IRIS_ADVANCED_TOOLKIT_PLUS_PLUS_HEADER_
