/*++

Copyright (c) 2013 Brad Weinberger

Module Name:

    pfm.cpp

Abstract:

    This file contains the definitions for the PFM functions used
    to write and read framebuffers objects.

--*/

#include <iristest.h>

static
bool 
IsLittleEndian()
{
    UINT8 ByteArray[] = { 255, 0, 0, 0 };
    PUINT32 ByteArrayAsInt;

    ByteArrayAsInt = (PUINT32) ByteArray;

    return *ByteArrayAsInt == 255;
}

bool
WritePfm(
    PCFRAMEBUFFER FrameBuffer,
    std::string FileName
    )
{
    COLOR3 PixelColor;
    SIZE_T ColumnIndex;
    SIZE_T RowIndex;
    SIZE_T Height;
    SIZE_T Width;
    FLOAT Scale;
    FILE *File;

    File = fopen(FileName.c_str(), "wb");

    if (File == NULL)
    {
        return false;
    }

    FramebufferGetDimensions(FrameBuffer, &Height, &Width);

    Scale = (IsLittleEndian()) ? (FLOAT) 1.0 : (FLOAT) -1.0;

    fprintf(File, "PF\n%zu %zu\n%lf\n", Width, Height, Scale);

    for (ColumnIndex = 0; ColumnIndex < Height; ColumnIndex++)
    {
        for (RowIndex = 0; RowIndex < Width; RowIndex++)
        {
            FramebufferGetPixel(FrameBuffer,
                                RowIndex,
                                ColumnIndex,
                                &PixelColor);

            fwrite(&PixelColor.Red, sizeof(FLOAT), 1, File);
            fwrite(&PixelColor.Green, sizeof(FLOAT), 1, File);
            fwrite(&PixelColor.Blue, sizeof(FLOAT), 1, File);
        }
    }

    fclose(File);

    return true;
}