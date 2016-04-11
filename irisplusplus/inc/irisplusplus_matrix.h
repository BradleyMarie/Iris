/*++

Copyright (c) 2014 Brad Weinberger

Module Name:

    irisplusplus_matrix.h

Abstract:

    This file contains the definitions for the 
    Iris++ matrix type.

--*/

#include <irisplusplus.h>

#ifndef _MATRIX_IRIS_PLUS_PLUS_
#define _MATRIX_IRIS_PLUS_PLUS_

namespace Iris {

//
// Types
//

class Matrix final {
public:
    IRISPLUSPLUSAPI
    Matrix(
        _In_ FLOAT M00,
        _In_ FLOAT M01,
        _In_ FLOAT M02,
        _In_ FLOAT M03,
        _In_ FLOAT M10,
        _In_ FLOAT M11,
        _In_ FLOAT M12,
        _In_ FLOAT M13,
        _In_ FLOAT M20,
        _In_ FLOAT M21,
        _In_ FLOAT M22,
        _In_ FLOAT M23,
        _In_ FLOAT M30,
        _In_ FLOAT M31,
        _In_ FLOAT M32,
        _In_ FLOAT M33
        );

    IRISPLUSPLUSAPI
    static
    Matrix
    Translation(
        _In_ FLOAT X, 
        _In_ FLOAT Y, 
        _In_ FLOAT Z
        );

    IRISPLUSPLUSAPI
    static
    Matrix
    Scalar(
        _In_ FLOAT X, 
        _In_ FLOAT Y, 
        _In_ FLOAT Z
        );

    IRISPLUSPLUSAPI
    static
    Matrix
    Rotation(
        _In_ FLOAT Theta, 
        _In_ FLOAT X, 
        _In_ FLOAT Y, 
        _In_ FLOAT Z
        );

    IRISPLUSPLUSAPI
    static
    Matrix
    Orthographic(
        _In_ FLOAT Left,
        _In_ FLOAT Right,
        _In_ FLOAT Bottom,
        _In_ FLOAT Top,
        _In_ FLOAT Near,
        _In_ FLOAT Far
        );

    IRISPLUSPLUSAPI
    static
    Matrix
    Frustum(
        _In_ FLOAT Left,
        _In_ FLOAT Right,
        _In_ FLOAT Bottom,
        _In_ FLOAT Top,
        _In_ FLOAT Near,
        _In_ FLOAT Far
        );

    static 
    Matrix
    Identity(
        void
        )
    {
        return Matrix(nullptr);
    }

    PMATRIX
    AsPMATRIX(
        void
        )
    {
        return Data;
    }

    PCMATRIX
    AsPCMATRIX(
        void
        ) const
    {
        return Data;
    }

    Matrix
    Inverse(
        void
        ) const
    {
        PMATRIX IrisMatrix;

        IrisMatrix = MatrixGetInverse(Data);

        return Matrix(IrisMatrix);
    }

    void
    ReadContents(
        _Out_writes_(4) FLOAT Contents[4][4]
        ) const
    {
		ISTATUS Status = MatrixReadContents(Data, Contents);

        if (Status != ISTATUS_SUCCESS)
        {
			assert(Status == ISTATUS_INVALID_ARGUMENT_01);
            throw std::invalid_argument("Contents");
        }
    } 

    Matrix(
        _In_ const Matrix & ToCopy
        )
    : Data(ToCopy.Data)
    {
        MatrixRetain(Data);
    }

    Matrix(
        _In_ Matrix && ToMove
        )
    : Data(ToMove.Data)
    {
        ToMove.Data = nullptr;
    }

    Matrix & 
    operator=(
        _In_ const Matrix & ToCopy
        )
    {
        if (this != &ToCopy)
        {
            MatrixRelease(Data);
            Data = ToCopy.Data;
            MatrixRetain(Data);
        }

        return *this;
    }

    ~Matrix(
        void
        )
    {
        MatrixRelease(Data);
    }

    friend
    static
    inline
    Matrix
    operator*(
        _In_ const Matrix & Multiplicand0,
        _In_ const Matrix & Multiplicand1
        );

private:
    PMATRIX Data;

    Matrix(
        _In_ PMATRIX IrisMatrix
        )
    : Data(IrisMatrix)
    { }
};

static
inline
Matrix
operator*(
    _In_ const Matrix & Multiplicand0,
    _In_ const Matrix & Multiplicand1
    )
{
    PMATRIX IrisMatrix;
    ISTATUS Status;

    Status = MatrixAllocateProduct(Multiplicand0.Data,
                                   Multiplicand1.Data,
                                   &IrisMatrix);

    if (Status == ISTATUS_SUCCESS)
    {
        return Matrix(IrisMatrix);
    }

    switch (Status)
    {
        case ISTATUS_ARITHMETIC_ERROR:
            throw std::invalid_argument("Product Not Invertible");
        default:
            assert(Status == ISTATUS_ALLOCATION_FAILED);
            throw std::bad_alloc();
    }
}

} // namespace Iris

#endif // _MATRIX_IRIS_PLUS_PLUS_
