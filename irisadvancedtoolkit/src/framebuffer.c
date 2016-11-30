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

    if (Framebuffer == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_00;
    }
    
    if (Path == NULL)
    {
        return ISTATUS_INVALID_ARGUMENT_01;
    }
    
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
    fprintf(File, "%lu %lu\n", (unsigned long) Width, (unsigned long) Height);
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