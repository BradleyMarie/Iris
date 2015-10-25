/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    iristoolkit_framebuffer.c

Abstract:

    This file contains the definitions for the FRAMEBUFFER type.

--*/

#include <iristoolkitp.h>

//
// Types
//

struct _FRAMEBUFFER {
    SIZE_T Rows;
    SIZE_T Columns;
    _Field_size_full_(Rows * Columns) PCOLOR3 Pixels;
};

//
// Prototypes
//

_Check_return_
_Ret_maybenull_
PFRAMEBUFFER
FramebufferAllocate(
    _In_ COLOR3 InitialColor,
    _In_ SIZE_T Rows,
    _In_ SIZE_T Columns
    )
{
    PFRAMEBUFFER Framebuffer;
    SIZE_T ArrayLength;
    PCOLOR3 Pixels;
    ISTATUS Status;
    SIZE_T Index;

    if (Rows == 0 ||
        Columns == 0)
    {
        return NULL;
    }

    Status = CheckedMultiplySizeT(Rows, Columns, &ArrayLength);

    if (Status != ISTATUS_SUCCESS)
    {
        return NULL;
    }

    Framebuffer = (PFRAMEBUFFER) malloc(sizeof(FRAMEBUFFER));

    if (Framebuffer == NULL)
    {
        return NULL;
    }

    Pixels = (PCOLOR3) malloc(sizeof(COLOR3) * ArrayLength);

    if (Pixels == NULL)
    {
        free(Framebuffer);
        return NULL;
    }

    for (Index = 0; Index < ArrayLength; Index++)
    {
        Pixels[Index] = InitialColor;
    }

    Framebuffer->Rows = Rows;
    Framebuffer->Columns = Columns;
    Framebuffer->Pixels = Pixels;

    return Framebuffer;
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

VOID
FramebufferFree(
    _In_opt_ _Post_invalid_ PFRAMEBUFFER Framebuffer
    )
{
    if (Framebuffer == NULL)
    {
        return;
    }

    free(Framebuffer->Pixels);
    free(Framebuffer);
}