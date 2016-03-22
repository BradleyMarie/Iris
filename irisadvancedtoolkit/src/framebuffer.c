/*++

Copyright (c) 2016 Brad Weinberger

Module Name:

    irisadvancedtoolkit_framebuffer.c

Abstract:

    This file contains the definitions for the FRAMEBUFFER type.

--*/

#include <irisadvancedtoolkitp.h>
#include <stdio.h>

//
// Types
//

struct _FRAMEBUFFER {
    SIZE_T Rows;
    SIZE_T Columns;
    _Field_size_full_(Rows * Columns) PCOLOR3 Pixels;
};

//
// Static Functions
//

BOOL 
FrambufferIsLittleEndian(
    VOID
    )
{
    UINT8 ByteArray[] = { 255, 0 };
    PUINT16 ByteArrayAsInt;

    ByteArrayAsInt = (PUINT16) ByteArray;

    if (*ByteArrayAsInt == 255)
    {
        return TRUE;
    }
    
    return FALSE;
}

//
// Public Functions
//

_Check_return_
_Success_(return == ISTATUS_SUCCESS)
ISTATUS
FramebufferAllocate(
    _In_ COLOR3 InitialColor,
    _In_ SIZE_T Rows,
    _In_ SIZE_T Columns,
    _Out_ PFRAMEBUFFER *Framebuffer
    )
{
    PFRAMEBUFFER AllocatedFrameBuffer;
    BOOL AllocationSuccessful;
    SIZE_T ArrayLength;
    PVOID DataAllocation;
    PVOID HeaderAllocation;
    SIZE_T Index;
    ISTATUS Status;

    if (Color3Validate(InitialColor) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (Rows == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
    if (Columns == 0)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Status = CheckedMultiplySizeT(Rows, Columns, &ArrayLength);

    if (Status != ISTATUS_SUCCESS)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocationSuccessful = IrisAlignedAllocWithHeader(sizeof(FRAMEBUFFER),
                                                      _Alignof(FRAMEBUFFER),
                                                      &HeaderAllocation,
                                                      sizeof(COLOR3) * ArrayLength,
                                                      _Alignof(COLOR3),
                                                      &DataAllocation,
                                                      NULL);

    if (AllocationSuccessful == FALSE)
    {
        return ISTATUS_ALLOCATION_FAILED;
    }

    AllocatedFrameBuffer = (PFRAMEBUFFER) HeaderAllocation;
    AllocatedFrameBuffer->Pixels = (PCOLOR3) DataAllocation;
    AllocatedFrameBuffer->Columns = Columns;
    AllocatedFrameBuffer->Rows = Rows;

    for (Index = 0; Index < ArrayLength; Index++)
    {
        AllocatedFrameBuffer->Pixels[Index] = InitialColor;
    }

    *Framebuffer = AllocatedFrameBuffer;

    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
FramebufferSetPixel(
    _Inout_ PFRAMEBUFFER Framebuffer,
    _In_ COLOR3 Color,
    _In_ SIZE_T Row,
    _In_ SIZE_T Column
    )
{
    SIZE_T RowSize;

    if (Framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    RowSize = Framebuffer->Rows;

    if (RowSize <= Row)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }
    
    if (Color3Validate(Color) == FALSE)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    if (Framebuffer->Columns <= Column)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    Framebuffer->Pixels[Row * RowSize + Column] = Color;

    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
FramebufferGetPixel(
    _In_ PCFRAMEBUFFER Framebuffer,
    _In_ SIZE_T Row,
    _In_ SIZE_T Column,
    _Out_ PCOLOR3 Color
    )
{
    SIZE_T RowSize;

    if (Framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    RowSize = Framebuffer->Rows;

    if (RowSize <= Row)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (Framebuffer->Columns <= Column)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    if (Color == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_03;
    }

    *Color = Framebuffer->Pixels[Row * RowSize + Column];

    return ISTATUS_SUCCESS;
}

ISTATUS
FramebufferGetDimensions(
    _In_ PCFRAMEBUFFER Framebuffer,
    _Out_ PSIZE_T Rows,
    _Out_ PSIZE_T Columns
    )
{
    if (Framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }

    if (Rows == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }

    if (Columns == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_02;
    }

    *Rows = Framebuffer->Rows;
    *Columns = Framebuffer->Columns;

    return ISTATUS_SUCCESS;
}

_Success_(return == ISTATUS_SUCCESS)
ISTATUS
FramebufferSaveAsPFM(
    _In_ PCFRAMEBUFFER Framebuffer,
    _In_ PCSTR Path
    )
{
    COLOR3 PixelColor;
    SIZE_T ColumnIndex;
    SIZE_T RowIndex;
    SIZE_T Height;
    SIZE_T Width;
    FLOAT Scale;
    FILE *File;

#if defined(_MSC_VER)

    errno_t error = fopen_s(&File, Path, "wb");

    if (error != 0)
    {
        return ISTATUS_IO_ERROR;
    }

#else

    File = fopen(Path, "wb");

    if (File == NULL)
    {
        return ISTATUS_IO_ERROR;
    }

#endif // defined(_MSC_VER)

    FramebufferGetDimensions(Framebuffer, &Height, &Width);

    if (FrambufferIsLittleEndian() != FALSE)
    {
        Scale = (FLOAT) -1.0;
    }
    else
    {
        Scale = (FLOAT) 1.0;
    }

    fprintf(File, "PF\n");
    fprintf(File, "%u %u\n", (unsigned long) Width, (unsigned long) Height);
    fprintf(File, "%lf\n", Scale);

    for (RowIndex = 0; RowIndex < Height; RowIndex++)
    {
        for (ColumnIndex = 0; ColumnIndex < Width; ColumnIndex++)
        {
            FramebufferGetPixel(Framebuffer,
                                Height - RowIndex - 1,
                                ColumnIndex,
                                &PixelColor);

            fwrite(&PixelColor.Red, sizeof(FLOAT), 1, File);
            fwrite(&PixelColor.Green, sizeof(FLOAT), 1, File);
            fwrite(&PixelColor.Blue, sizeof(FLOAT), 1, File);
        }
    }

    fclose(File);

    return ISTATUS_SUCCESS;
}

VOID
FramebufferFree(
    _In_opt_ _Post_invalid_ PFRAMEBUFFER Framebuffer
    )
{
    IrisAlignedFree(Framebuffer);
}