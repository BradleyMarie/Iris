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
        _In_ PCMATRIX_REFERENCE MatrixRef
        )
    : Data(MatrixRef)
    { }

    PCMATRIX_REFERENCE
    AsPCMATRIX_REFERENCE(
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
        PCMATRIX_REFERENCE Inverse;

        Inverse = MatrixReferenceGetInverse(Data);

        return MatrixReference(Inverse);
    }

    IRISPLUSPLUSAPI
    void
    ReadContents(
        _Out_writes_(4) FLOAT Contents[4][4]
        ) const;

private:
    PCMATRIX_REFERENCE Data;
};

} // namespace Iris

#endif // _MATRIX_REFERENCE_IRIS_PLUS_PLUS_