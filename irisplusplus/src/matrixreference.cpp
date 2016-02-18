/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    matrix.cpp

Abstract:

    This file contains the definitions for the 
    Iris++ matrix type.

--*/

#include <irisplusplus.h>

namespace Iris {

void
MatrixReference::ReadContents(
    _Out_writes_(4) FLOAT Contents[4][4]
    )
{
    if (Contents == nullptr)
    {
        throw std::invalid_argument("Contents");
    }

    if (Data == nullptr)
    {
        Contents[0][0] = (FLOAT) 1.0;
        Contents[0][1] = (FLOAT) 0.0;
        Contents[0][2] = (FLOAT) 0.0;
        Contents[0][3] = (FLOAT) 0.0;
        Contents[1][0] = (FLOAT) 0.0;
        Contents[1][1] = (FLOAT) 1.0;
        Contents[1][2] = (FLOAT) 0.0;
        Contents[1][3] = (FLOAT) 0.0;
        Contents[2][0] = (FLOAT) 0.0;
        Contents[2][1] = (FLOAT) 0.0;
        Contents[2][2] = (FLOAT) 1.0;
        Contents[2][3] = (FLOAT) 0.0;
        Contents[3][0] = (FLOAT) 0.0;
        Contents[3][1] = (FLOAT) 0.0;
        Contents[3][2] = (FLOAT) 0.0;
        Contents[3][3] = (FLOAT) 1.0;
    }
    else
    {
        MatrixReferenceReadContents(Data, Contents);
    }
}

} // namespace Iris