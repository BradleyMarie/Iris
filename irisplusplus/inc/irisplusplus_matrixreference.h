/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_matrixreference.h

Abstract:

    This file contains the definitions for the 
    Iris++ matrix reference type.

--*/

#include <irisplusplus.h>

#ifndef _MATRIX_REFERENCE_IRIS_PLUS_PLUS_
#define _MATRIX_REFERENCE_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class MatrixReference final {
public:
    MatrixReference(
        _In_ PCMATRIX MatrixRef
        )
    : Data(MatrixRef)
    { }

    PCMATRIX
    AsPCMATRIX(
        void
        ) const
    {
        return Data;
    }

    MatrixReference
    Inverse(
        void
        )
    {
        PCMATRIX Inverse;

        Inverse = MatrixGetConstantInverse(Data);

        return MatrixReference(Inverse);
    }

    void
    ReadContents(
        _Out_writes_(4) FLOAT Contents[4][4]
        ) const
    {
        if (Contents == nullptr)
        {
            throw std::invalid_argument("Contents");
        }

        MatrixReadContents(Data, Contents);
    } 

    MatrixReference(
        _In_ const MatrixReference & ToCopy
        )
    : Data(ToCopy.Data)
    { }

    MatrixReference & 
    operator=(
        _In_ const MatrixReference & ToCopy
        )
    {
        Data = ToCopy.Data;
        return *this;
    }

private:
    PCMATRIX Data;
};

} // namespace Iris

#endif // _MATRIX_REFERENCE_IRIS_PLUS_PLUS_